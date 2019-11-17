#include "intro_screen.h"


//Static initializers
bool intro_screen::s_active = false;
glm::vec3 intro_screen::center{ 0,0,0 };

bool intro_screen::active()
{
	return s_active;
}

void intro_screen::active(bool b)
{
	s_active = b;
}

//Renders the intro screen text
void intro_screen::render(engine::ref<engine::text_manager>& text_manager)
{
	float x_pos = (float)engine::application::window().width() / 2 - 80.f;
	float y_pos = (float)(engine::application::window().height()) * (.95f);

	glm::vec4 text_colour = glm::vec4(.5f, 1.f, 0.5f, 1.f);
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	text_manager->render_text(text_shader, "3D_TD", x_pos, y_pos, 1.f, text_colour);
	text_manager->render_text(text_shader, "Press ENTER to continue", x_pos -100.f, y_pos -50.f, .75f, text_colour);

	//Render controls list
	x_pos = 10.f;
	y_pos = (float)(engine::application::window().height()) * (3.f / 5.f);	
	text_manager->render_text(text_shader, "Controls:", x_pos, y_pos, .5f,text_colour);
	//Not all controls actually functional yet
	std::vector<std::string> content{
		"Movement->W,A,S,D",
		"Sprint->L_SHIFT",
		"Jump->SPACE",
		"Shoot/Place block->LMB",
		"Remove block->RMB",		
		"Switch blocks->1,2",
		"Zoom->L_CTRL + SCROLL"
	};

	for (auto str : content)
	{
		//Wrap text column along the screen if control list is too long to fit in one column
		if (y_pos - 25.f < 0)
		{
			y_pos = (float)(engine::application::window().height()) * (3.f / 5.f);
			x_pos += (float)(engine::application::window().width())/3.f -10.f;
		}
		y_pos -= 25.f;
		text_manager->render_text(text_shader, str, x_pos, y_pos, .5f, text_colour);
	}
}

//Increment the position of the camera in such a way that it orbits the center point
void intro_screen::update(engine::perspective_camera& cam, const engine::timestep& ts)
{
	cam.look_at(center);
	cam.move(cam.left, ts);
}

//Set up the camera to be some distance from the centerpoint with sufficient elevation to give a good view.
void intro_screen::init(engine::perspective_camera& cam,glm::vec3& center_point)
{
	cam.position({ -20,14,-20 });
	center = center_point;
	s_active = true;
}


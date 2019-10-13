#include "intro_screen.h"


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

void intro_screen::render(engine::ref<engine::text_manager>& text_manager)
{
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	text_manager->render_text(text_shader, "3D_TD", (float)engine::application::window().width()/2 -80.f, (float)(engine::application::window().height())*(4.f/5.f), 1.f, glm::vec4(0.f, 0.5f, 0.5f, 1.f));
}

void intro_screen::update(engine::perspective_camera& cam, const engine::timestep& ts)
{
	cam.look_at(center);
	cam.move(cam.left, ts);
}

void intro_screen::init(engine::perspective_camera& cam,glm::vec3& center_point)
{
	cam.position({ 0,8,0 });
	center = center_point;
	s_active = true;
}


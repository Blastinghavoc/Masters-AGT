#include "hud_manager.h"

std::vector<engine::ref<hud_element>> hud_manager::m_hud_elements;
std::vector<engine::ref<text_hud_element>> hud_manager::m_text_elements;

void hud_manager::render(engine::orthographic_camera& cam_2d,engine::ref<engine::shader> shader)
{
	/*
	NOTE: HUD elements that are inative will do nothing when render() is called on them.
	If many more HUD elements were used it would be most efficient to separate the storage of
	active and inactive elements to prevent unecessary iteration, but so far there are few enough
	elements that this optimisation is irrelevant.
	*/

	for (auto& element : m_text_elements) {		
		element->render(shader);
	}

	engine::renderer::begin_scene(cam_2d, shader);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("lighting_on", false);//HUD needs no lighting
	for (auto& element : m_hud_elements) {		
		element->render(shader);
	}
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("lighting_on", true);
	engine::renderer::end_scene();
}

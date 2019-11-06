#include "hud_manager.h"

std::vector<engine::ref<hud_element>> hud_manager::m_hud_elements;
std::vector<engine::ref<text_hud_element>> hud_manager::m_text_elements;

void hud_manager::render(engine::orthographic_camera& cam_2d,engine::ref<engine::shader> shader)
{
	for (auto& element : m_text_elements) {		
		element->render(shader);
	}

	engine::renderer::begin_scene(cam_2d, shader);
	for (auto& element : m_hud_elements) {
		element->render(shader);
	}
	engine::renderer::end_scene();
}

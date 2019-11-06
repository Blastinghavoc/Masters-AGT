#include "hud_manager.h"

std::vector<engine::ref<hud_element>> hud_manager::m_elements;

void hud_manager::render(engine::ref<engine::shader> shader)
{
	for (auto& element : m_elements) {		
		element->render(shader);
	}
}

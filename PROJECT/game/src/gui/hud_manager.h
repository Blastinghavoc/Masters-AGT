#pragma once

#include "engine.h"
#include "hud_element.h"

//Static class to manage the HUD
class hud_manager {
public:

	//static void on_update(engine::timestep time_step);

	static void render(engine::ref<engine::shader> shader);

	static void add_element(engine::ref<hud_element> element) { m_elements.push_back(element); };
private:
	static std::vector<engine::ref<hud_element>> m_elements;
};

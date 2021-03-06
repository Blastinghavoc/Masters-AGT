#pragma once

#include "engine.h"
#include "abstract_hud_element.h"
#include "text_hud_element.h"
#include "hud_element.h"

/*
Static class to manage the HUD.
Keeps a reference to every HUD element and renders them.
Other classes can keep their own references to change the state of HUD elements, but this
class separates out the concerns of rendering.
*/
class hud_manager {
public:

	static void render(engine::orthographic_camera& cam_2d,engine::ref<engine::shader> shader);

	static void add_element(engine::ref<hud_element> element) { m_hud_elements.push_back(element); };
	static void add_element(engine::ref<text_hud_element> element) { m_text_elements.push_back(element); };
private:
	static std::vector<engine::ref<hud_element>> m_hud_elements;
	static std::vector<engine::ref<text_hud_element>> m_text_elements;
};

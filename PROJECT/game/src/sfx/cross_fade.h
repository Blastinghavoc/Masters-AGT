#pragma once
#include "../gui/hud_element.h"

class cross_fade {
public:
	cross_fade(engine::ref<hud_element> panel) :m_panel{ panel } {};
	~cross_fade() {};

	void activate(float duration);
	void on_update(const engine::timestep& ts);
private:
	engine::ref<hud_element> m_panel;
	float m_remaining{ 0 };
	float m_duration{ 0 };
};

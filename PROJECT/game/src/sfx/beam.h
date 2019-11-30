#pragma once
#include <engine.h>
#include "engine/utils/timer.h"

/*
Transparent beam class based on lightning effect from SFX template.
*/
class beam {
public:
	beam();
	~beam() {};

	//See note in on_render for why these defaults exist
	void activate(glm::vec3 start, glm::vec3 end, glm::vec3 rgb, float duration, float width = 10.f, int nesting = 2);

	void on_update(const engine::timestep& ts);

	void on_render(engine::ref<engine::shader> shader);

	bool is_active() { return m_active; };
private:
	bool m_active = false;

	engine::timer m_timer;
	float m_duration{0.f};

	float m_width{0.f};

	int m_nesting{ 2 };

	engine::material m_material;

	glm::vec3 m_colour{ 1.f };

	engine::ref<engine::mesh> m_line;
};

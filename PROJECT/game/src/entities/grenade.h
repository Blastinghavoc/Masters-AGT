#pragma once
#include "engine.h"
#include "engine/utils/timer.h"
#include "trigger_box.h"
#include "abstract_actor.h"

class grenade : public abstract_actor  {
public:
	grenade(float scale = 1.f, float lifetime = 3.f);
	~grenade() {};

	void on_update(const engine::timestep& time_step) override;

	void launch(glm::vec3 start,glm::vec3 direction,float force);

	static engine::ref<grenade> create(float scale = 1.f, float lifetime = 3.f) {
		return std::make_shared<grenade>(scale, lifetime);
	};

	void render(const engine::ref<engine::shader>& shader);

	bool is_active() { return m_active; };
	void set_active(bool b) { m_active = b; };
	trigger_box& get_trigger_box() { return m_box; };
private:
	static engine::game_object_properties prefab;
	static bool prefab_ready;

	float m_lifetime;

	bool m_active{ false };
	engine::timer m_life_timer{};
	trigger_box m_box{};
};

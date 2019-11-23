#pragma once
#include "abstract_actor.h"
#include "trigger_box.h"
#include "engine/utils/timer.h"

/*
Class representing the projectiles fired by NPCs
*/
class projectile : public abstract_actor {
public:
	projectile() :m_is_enemy{false} {};//Default constructor

	projectile(bool is_enemy);

	~projectile() {};

	void on_update(const engine::timestep& time_step) override;

	bool is_enemy() { return m_is_enemy; };

	static engine::ref<projectile> create(bool is_enemy) {
		return std::make_shared<projectile>(is_enemy);
	}

	void render(const engine::ref<engine::shader>& shader);

	trigger_box& get_trigger_box() { return m_box; };

	void reset();

	bool is_active() { return m_active; };

	void deactivate();

private:
	bool m_is_enemy;//Flag indicating whether the projectile is affiliated with enemies or allies
	trigger_box m_box;
	engine::timer m_lifetimer;
	float m_max_lifetime = { 50.f };
	bool m_active = true;

	glm::mat4 m_transform{};//Custom transform for rendering

	static bool m_ally_prefab_ready;
	static bool m_enemy_prefab_ready;
	static engine::game_object_properties m_ally_prefab;
	static engine::game_object_properties m_enemy_prefab;
};

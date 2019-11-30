#pragma once
#include "engine.h"

/*
Static class to manage physics-enabled objects.
Mainly serves as a globally accessible wrapper to the bullet manager,
and a centralized repository for references to physics-enable game_objects
*/
class physics_manager {
public:
	static void init();
	static void update(const engine::timestep& ts);
	static void add(engine::ref<engine::game_object> obj) {
		m_physical_gameobjects.push_back(obj);
		m_bullet_manager->add_physical_object(obj, m_bullet_manager->get_dynamics_world());
	};
private:
	static std::vector<engine::ref<engine::game_object>> m_physical_gameobjects;
	static engine::ref<engine::bullet_manager> m_bullet_manager;
};

#pragma once
#include "../entities/pickup.h"
#include "engine/utils/timer.h"
#include "../lighting/light_manager.h"

/*
Static class to manage the gameplay effects of pickups
*/
class pickup_manager {
public:

	static void init();

	static void on_update(const engine::timestep& time_step, trigger_box& player_trigger_box);

	static void render(const engine::ref<engine::shader>& shader);

	static bool powerup_active() { return m_powerup_active; };

	static void roll_for_powerup(glm::vec3 position);

	static void make_powerup_at(glm::vec3 position);

private:
	static pickup m_freeze_time_pickup;

	static bool m_powerup_active;

	static engine::timer m_freeze_timer;
	static constexpr float m_freeze_duration = 10.f;
	static bool m_freeze_active;

	//Percentage probability to create a powerup when roll_for_powerup is called
	static constexpr int m_powerup_probability = 5;


	static void freeze_enemies();
	static void unfreeze_enemies();

	//Highlights the active powerup
	static engine::ref<engine::SpotLight> m_spot_light;
};

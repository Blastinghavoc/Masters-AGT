#pragma once
#include "../entities/grenade.h"

/*
Static (singleton) class to manage weapons used by the player.
Currently only one weapon, a "grenade".
The grenade can be fired at different levels of "charge" by holding and
releasing the mouse button. This gives the grenade larger initial force,
making it go further.
*/
class weapon_manager {
public:
	static void init();


	static int remaining_grenades() { return (int)m_grenades.size(); };

	static void update(const engine::timestep& ts);

	static void on_render(const engine::ref<engine::shader>& shader);

	static void render_trigger_boxes(const engine::ref<engine::shader>& shader);

	//Start the charge timer
	static void start_charging();

	static void launch_charged(glm::vec3 start, glm::vec3 direction, const engine::timestep& ts, float force = m_base_launch_force);

	static float charge_percentage();
private:
	static void launch_grenade(glm::vec3 start, glm::vec3 direction, const engine::timestep& ts, float force = m_base_launch_force);
	static std::vector<engine::ref<grenade>> m_grenades;//Pool of grenades
	static std::vector<engine::ref<grenade>> m_active_grenades;

	static constexpr float m_grenade_damage = 25.f;

	static bool can_fire_grenade() {
		return m_grenade_cooldown_timer.total() > m_grenade_cooldown;
	};

	static engine::timer m_grenade_cooldown_timer;
	static constexpr float m_grenade_cooldown = 0.5f;

	static bool m_is_charging;
	static engine::timer m_charge_timer;
	static constexpr float m_max_charge_multiplier = 2.f;
	static constexpr float m_max_charge_time = 1.f;
	static constexpr float m_base_launch_force = 15.f;
	static constexpr float m_base_charge_time = 0.25f;//All charged shots are treated as having had at least this much charge time
};

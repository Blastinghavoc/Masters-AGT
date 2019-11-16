#pragma once
#include "../entities/grenade.h"

class weapon_manager {
public:
	static void init(engine::ref<engine::audio_manager> audio_manager);

	static void launch_grenade(glm::vec3 start, glm::vec3 direction, float force);

	static int remaining_grenades() { return (int)m_grenades.size(); };

	static void update(const engine::timestep& ts);

	static void on_render(const engine::ref<engine::shader>& shader);

	static void render_trigger_boxes(const engine::ref<engine::shader>& shader);

private:
	static std::vector<engine::ref<grenade>> m_grenades;
	static std::vector<engine::ref<grenade>> m_active_grenades;
	static engine::ref<engine::audio_manager> m_audio_manager;

	static constexpr float m_grenade_damage = 50.f;
};

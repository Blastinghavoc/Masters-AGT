#pragma once

#include "engine/utils/timer.h"
#include <map>
#include <string>
#include "../entities/player.h"
#include "../gui/hud_manager.h"
#include "../ai/enemy_manager.h"
#include "../entities/interactable.h"
#include "../entities/enemy_type.h"
#include "wave_definition.h"
/*
Static class to manage gameplay elements
like health, money, score etc, as well as the
flow of the gameplay
*/
class gameplay_manager {
public:

	static void init(player* playr, engine::ref<engine::text_manager> text_manager, engine::perspective_camera* camera,
		engine::ref<grid> level_grid,
		engine::ref<engine::audio_manager> audio_manager,
		std::vector<engine::ref<engine::game_object>>* decorational_objects);
	static void update(const engine::timestep& ts);

	static int score() { return m_score; };
	static int health() { return (int)m_player_ptr->health(); };
	static int money() { return m_money; };
	static int portal_health() { return m_portal_health; };//The health of the portal the player is defending.
	static int build_time() { return m_max_build_time - (int)m_build_timer.total(); };

	static void damage_portal();
	static void damage_player(float amnt = 10.f);

	static void add_score(int amnt) {
		m_score += (int)(m_score_multiplier*amnt);
	};
	static bool buy(std::string name) {
		if (m_prices.count(name) > 0)
		{
			auto tmp = m_prices[name];
			if (tmp <= m_money)
			{
				m_money -= tmp;
				return true;
			}
		}
		return false;
	};

	static void next_build_phase();

	static bool is_wave_active() { return m_wave_active; };

	static void on_event(engine::event& event);

private:
	static int m_score, m_money, m_portal_health;
	static float m_score_multiplier;
	static engine::timer m_build_timer;
	static int m_max_build_time;
	static std::map<std::string, int> m_prices;
	static player* m_player_ptr;
	static engine::ref<text_hud_element> m_top_display, m_score_display,
		m_money_display, m_health_display,
		m_portal_health_display,m_tool_display,
		m_message_display,
		m_weapon_charge_display;
	static bool m_wave_active;

	static std::deque<wave_definition> m_waves;
	static int m_max_waves;
	static int m_wave_number;
	static engine::perspective_camera* m_camera;
	static engine::ref<grid> m_level_grid;
	static wave_definition m_current_wave_definition;
	static engine::ref<engine::audio_manager> m_audio_manager;
	static constexpr int m_max_turrets = 5;
	static int m_available_blocks;
	static bool m_fire_weapon;//Indicates whether the weapon should be fired this update
	static engine::timer m_immunity_timer;
	static constexpr float m_immunity_duration = 1.f;
	static bool m_hardmode_active;

	enum class tool
	{
		block,
		turret
	};

	static std::string string_of(tool t) {
		switch (t)
		{
		case gameplay_manager::tool::block:
			return "block";
			break;
		case gameplay_manager::tool::turret:
			return "turret";
			break;
		default:
			break;
		}
	};

	static tool m_current_tool;

	static interactable m_hard_mode_switch;

	static void mouse1_event_handler(bool press);
	static void mouse2_event_handler();
	static void start_combat_phase();
	static void place_block(int x, int z);
	static void remove_block(int x, int z);
	static void place_turret(int x, int z);
	static void remove_turret(int x, int z);

	static void kill_player();
	static void check_player_in_bounds();
	static void check_enemies_touching_player();
	static void check_interaction_with_hardmode_switch();
};

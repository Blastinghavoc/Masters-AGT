#pragma once

#include "engine/utils/timer.h"
#include <map>
#include <string>
#include "../player.h"
#include "../gui/hud_manager.h"
#include "../enemy_manager.h"
/*
Static class to manage gameplay elements
like health, money, score etc, as well as the
flow of the gameplay
*/
class gameplay_manager {
public:

	static void init(player* playr, engine::ref<engine::text_manager> text_manager);
	static void update(const engine::timestep& ts);

	static int score() { return m_score; };
	static int health() { return m_health; };
	static int money() { return m_money; };
	static int portal_health() { return m_portal_health; };//The health of the portal the player is defending.
	static int build_time() { return m_max_build_time - (int)m_build_timer.total(); };

	static void damage_portal() { m_portal_health -= 10; };

	static void add_score(int amnt) { m_score += amnt; };
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

	static void begin_wave();

	static bool is_wave_active() { return m_wave_active; };

private:
	static int m_score, m_health, m_money, m_portal_health;
	static engine::timer m_build_timer;
	static int m_max_build_time;
	static std::map<std::string, int> m_prices;
	static player* m_player_ptr;
	static glm::vec3 m_player_spawnpoint;
	static engine::ref<text_hud_element> m_top_display, m_score_display, m_money_display, m_health_display,
		m_portal_health_display;
	static bool m_wave_active;

	struct wave_definition {
		int num_enemies;
		float enemy_spacing;
		int prep_time;
	};

	static std::vector<wave_definition> m_waves;
	static int m_max_waves;
	static int m_wave_number;
};

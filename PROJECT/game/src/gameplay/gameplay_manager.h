#pragma once

#include "engine/utils/timer.h"
#include <map>
#include <string>
#include "../player.h"
/*
Static class to manage gameplay elements
like health, money, score etc.
*/
class gameplay_manager {
public:

	static void init(player* playr);
	static void update(const engine::timestep& ts);

	static int score() { return m_score; };
	static int health() { return m_health; };
	static int money() { return m_money; };
	static int build_time() { return m_max_build_time - (int)m_build_timer.elapsed(); };

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

private:
	static int m_score, m_health, m_money;
	static engine::timer m_build_timer;
	static int m_max_build_time;
	static std::map<std::string, int> m_prices;
	static player* m_player_ptr;
	static glm::vec3 m_player_spawnpoint;
};

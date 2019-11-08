#include "pch.h"
#include "gameplay_manager.h"

int gameplay_manager::m_score = 0;
int gameplay_manager::m_health = 0;
int gameplay_manager::m_money = 0;
engine::timer gameplay_manager::m_build_timer{};
int gameplay_manager::m_max_build_time = 30;
std::map<std::string, int> gameplay_manager::m_prices{};
player* gameplay_manager::m_player_ptr;
glm::vec3  gameplay_manager::m_player_spawnpoint{};

void gameplay_manager::init(player* playr)
{
	m_player_ptr = playr;
	m_player_spawnpoint = m_player_ptr->object()->position();
}

void gameplay_manager::update(const engine::timestep& ts)
{

}

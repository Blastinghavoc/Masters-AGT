#include "enemy_manager.h"

//Static initializers
std::map<int,enemy> enemy_manager::s_minions;
int enemy_manager::s_next_id = 0;

void enemy_manager::init()
{
}

void enemy_manager::on_update(engine::timestep time_step)
{
	for (auto& pair : s_minions) {
		pair.second.on_update(time_step);
	}
}

enemy& enemy_manager::spawn_minion(glm::vec3 position)
{
	s_minions[s_next_id] = enemy(s_next_id, position);
	return s_minions[s_next_id++];
}

void enemy_manager::render(const engine::ref<engine::shader>& shader)
{
	for (auto& pair : s_minions) {
		engine::renderer::submit(shader, pair.second.object());
	}
}

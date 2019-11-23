#pragma once

#include "animated_enemy.h"
#include "static_flying_enemy.h"

class enemy_factory {
public:
	static engine::ref<abstract_enemy> create(int id, glm::vec3 position = { 0,0,0 }, enemy_type type = enemy_type::robot1);

private:
	static std::map<enemy_type, engine::game_object_properties> prefabs;
};

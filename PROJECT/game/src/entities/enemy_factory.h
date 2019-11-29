#pragma once

#include "animated_enemy.h"
#include "static_flying_enemy.h"

/*
Static class responsible for constructing enemy objects with their correct gameplay statistics.
Holds "prefabs" of each enemy variety so that new enemies can just be cloned from the prefab
rather than loading their assets each time.
*/
class enemy_factory {
public:
	static engine::ref<abstract_enemy> create(int id, glm::vec3 position = { 0,0,0 }, enemy_type type = enemy_type::robot1);

private:
	static std::map<enemy_type, engine::game_object_properties> prefabs;
};

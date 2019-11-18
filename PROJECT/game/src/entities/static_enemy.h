#pragma once

#include "abstract_enemy.h"

/*
Represents a non-animated enemy, i.e an enemy with a static model.
NOT an enemy that does not move.
*/
class static_enemy : public abstract_enemy {
public:	

	static_enemy();
	static_enemy(int id, glm::vec3 position = { 0,0,0 },enemy_type type = enemy_type::robot1);
	~static_enemy() {};

	void on_update(const engine::timestep& time_step) override;

	static engine::ref<static_enemy> create(int id, glm::vec3 position = { 0,0,0 }, enemy_type type = enemy_type::robot1) {
		return std::make_shared<static_enemy>(id, position,type);
	};

	/*
	Override the distance metric to account for the potential y offset
	*/
	float square_distance_to_next_waypoint() const override;

	glm::vec3 ground_position() const override;

private:
	static std::map<enemy_type,engine::game_object_properties> prefabs;
	float m_y_offset = 0.f;//Used to correctly position flying enemies

	void construct_gameobject(enemy_type type);
};

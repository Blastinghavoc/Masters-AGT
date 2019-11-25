#pragma once

#include "static_enemy.h"
#include "../ai/flyer_ai.h"

class static_flying_enemy :public static_enemy {
public:
	static_flying_enemy();
	static_flying_enemy(int id, engine::ref<engine::game_object> obj,float altitude, enemy_type type = enemy_type::flyer1);
	~static_flying_enemy() {};

	virtual void on_update(const engine::timestep& time_step) override;

	glm::vec3 ground_position() const override;

	/*
	Override the distance metric to account for the y offset
	*/
	float square_distance_to_next_waypoint() const override;

private:
	float m_y_offset = 0.f;//Used to correctly position flying enemies
	friend class flyer_ai;
	flyer_ai m_brain;

protected:
	void update_waypoints() override;
};


#pragma once

#include "static_enemy.h"
#include "../ai/flyer_ai.h"

/*
Represents a non-animated flying enemy.
As it happens this is the only type of flying enemy.
The behaviour of these enemies is controlled by flyer_ai objects.
*/
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

	//Extend reset to also reset AI
	void reset()override;

private:
	float m_y_offset = 0.f;//Used to correctly position flying enemies above the terrain

	//Allow flyer_ai access to all aspects of this class, and delegate control to an instance of flyer_ai
	friend class flyer_ai;
	flyer_ai m_brain;

protected:
	//override to account for y offset
	void update_waypoints() override;
};


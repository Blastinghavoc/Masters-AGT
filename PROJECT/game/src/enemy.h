#pragma once

#include "animated_actor.h"

class enemy: public animated_actor {
public:
	enemy();
	enemy(int id,glm::vec3 position = {0,0,0});
	~enemy();

	void on_update(const engine::timestep& time_step) override;

	void add_waypoint(glm::vec3 point) { m_waypoints.push_back(point); };
private:
	std::deque<glm::vec3> m_waypoints{};
	int m_id;//Unique enemy id
};

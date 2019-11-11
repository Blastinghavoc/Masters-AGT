#pragma once

#include "animated_actor.h"

class enemy: public animated_actor {
public:
	enemy();
	enemy(int id,glm::vec3 position = {0,0,0});
	~enemy();

	void on_update(const engine::timestep& time_step) override;

	void add_waypoint(glm::vec3 point) { m_waypoints.push_back(point); };

	void set_path(std::deque<glm::vec3> path) { m_waypoints = path; };

	const int& get_id() const { return m_id; };
private:
	static engine::game_object_properties prefab;
	static bool prefab_ready;
	std::deque<glm::vec3> m_waypoints{};
	int m_id;//Unique enemy id
};

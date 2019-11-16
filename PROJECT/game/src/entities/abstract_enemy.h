#pragma once

#include "abstract_actor.h"
#include "trigger_box.h"

//Represents the abstract capabilities of an enemy.
class abstract_enemy : public virtual abstract_actor{
public:
	abstract_enemy() : m_id{-1} {};
	abstract_enemy(int id) : m_id{id} {};
	virtual ~abstract_enemy() {};

	virtual void on_update(const engine::timestep& time_step) override;

	void add_waypoint(glm::vec3 point) { m_waypoints.push_back(point); };

	void set_path(std::deque<glm::vec3> path) { m_waypoints = path; };

	const int& id() const { return m_id; };

	const int waypoints_remaining() const { return (int)m_waypoints.size(); };

	glm::vec3 next_waypoint() const;

	float square_distance_to_next_waypoint() const;

	glm::vec3 position() const { return m_object->position(); };

	trigger_box& get_trigger_box() { return m_box; };

	void set_health(float amount) { m_health = amount; };

	const float& health() { return m_health; };

	const float& max_health() { return m_max_health; };

	void deal_damage(float amount) { m_health -= amount; };

	bool is_closer_to_goal_than(const abstract_enemy& e2) const;

	/*static bool is_closer_to_goal(const abstract_enemy& e1, const abstract_enemy& e2) {
		return e1.is_closer_to_goal_than(e2);
	}*/

	static bool is_closer_to_goal(const engine::ref<abstract_enemy>& e1, const engine::ref<abstract_enemy>& e2) {
		return e1->is_closer_to_goal_than(*e2);
	}

protected:	
	std::deque<glm::vec3> m_waypoints{};
	int m_id;//Unique enemy id
	trigger_box m_box{};
	float m_health{100.f};
	float m_max_health{ 100.f };
};

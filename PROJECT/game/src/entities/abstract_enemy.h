#pragma once

#include "abstract_actor.h"
#include "trigger_box.h"
#include "enemy_type.h"

/*
Abstract class representing the capabilities of an enemy.
Almost all of the functions are concretely implemented, an some of them can
be overriden in derived classes. However, this class does not implement an on_update
function because an abstract enemy has no behaviour.

Note virtual inheritance ensuring a single instance of abstract_actor for derived classes.
*/
class abstract_enemy : public virtual abstract_actor{
public:	

	abstract_enemy() : m_id{ -1 }, m_type{enemy_type::robot1} {};
	abstract_enemy(int id) : m_id{id}, m_type{ enemy_type::robot1 } {};
	virtual ~abstract_enemy() {};

	void add_waypoint(glm::vec3 point) { m_waypoints.push_back(point); };

	void set_path(std::deque<glm::vec3> path) { m_waypoints = path; };

	const int& id() const { return m_id; };

	const int waypoints_remaining() const { return (int)m_waypoints.size(); };

	glm::vec3 next_waypoint() const;

	virtual float square_distance_to_next_waypoint() const;

	glm::vec3 position() const { return m_object->position(); };

	virtual glm::vec3 ground_position() const { return position(); };

	trigger_box& get_trigger_box() { return m_box; };

	void set_health(float amount) { m_health = amount; };

	void set_max_health(float amount) { m_max_health = amount; };

	const float& health() { return m_health; };

	const float& max_health() { return m_max_health; };

	void set_movement_speed(float speed) { m_movement_speed = speed; };

	const float& movement_speed() { return m_movement_speed; };

	void deal_damage(float amount) { m_health -= amount; };

	bool is_closer_to_goal_than(const abstract_enemy& e2) const;

	static bool is_closer_to_goal(const engine::ref<abstract_enemy>& e1, const engine::ref<abstract_enemy>& e2) {
		return e1->is_closer_to_goal_than(*e2);
	}

	void set_frozen(bool val) { m_frozen = val; };
	bool is_frozen() { return m_frozen; };

	enemy_type type() { return m_type; };

	//Used to determine where on the enemy a turret should aim.
	glm::vec3 targetting_point() { return position() + m_targetting_offset; };
	void set_targetting_offset(glm::vec3 offset) { m_targetting_offset = offset; };

	//Restore gameplay statistics to default
	virtual void reset() {
		m_health = m_max_health;
		m_frozen = false;
	};

protected:	
	std::deque<glm::vec3> m_waypoints{};
	int m_id;//Unique enemy id
	trigger_box m_box{};
	float m_health{100.f};
	float m_max_health{ 100.f };
	bool m_frozen = false;
	float m_movement_speed = 1.f;
	enemy_type m_type;
	glm::vec3 m_targetting_offset{0};

	//Functions useful in derived classes implementing on_update behaviours
	virtual void update_waypoints();
	void update_trigger_box() { m_box.on_update(m_object->position()); };

	static bool close_enough(glm::vec3 v1, glm::vec3 v2);
};

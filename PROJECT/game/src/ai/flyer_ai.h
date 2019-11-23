#pragma once
#include "engine.h"
#include "engine/utils/timer.h"

enum class flyer_state {
	following_path,
	shooting,
	making_room,
	returning_to_path,
	pursuing
};

class static_flying_enemy;

/*
Objects of this class are responsible for controlling the behaviour of a
static_flying_enemy that has one as a member.
*/
class flyer_ai {
public:
	flyer_ai();

	void update_state(static_flying_enemy& body);
	void on_update(const engine::timestep& time_step,static_flying_enemy& body);

	static void update_shared_data();

private:
	flyer_state m_state{flyer_state::following_path};
	flyer_state m_previous_state{ flyer_state::following_path };

	//Behaviours
	void follow_path(const engine::timestep& time_step, static_flying_enemy& body);
	void shoot(static_flying_enemy& body);
	void return_to_path(static_flying_enemy& body);
	void make_room(const engine::timestep& time_step,static_flying_enemy& body);
	void pursue(const engine::timestep& time_step, static_flying_enemy& body);

	float m_agro_range2 = 9.f;//Square of agro distance
	engine::timer m_shooting_cooldown_timer;
	float m_shooting_cooldown = 1.f;

	engine::timer m_decision_timer;//For use when deciding how long to stick with a decision
	float m_decision_wait = 0.5f;

	glm::vec3 m_flock_force{0};
	float m_flock_sepparation2 = 1.f;//Square distance to maintain between flyers
	glm::vec3 compute_flock_forces(static_flying_enemy& body);

	bool ready_to_change_state() {
		return m_decision_timer.total() >= m_decision_wait;
	};

	void change_state(flyer_state s,float wait_time = .5f) {
		if (s == m_state)
		{
			return;//Do nothing if asked to change to same state
		}
		m_previous_state = m_state;
		m_state = s;
		m_decision_timer.reset();
		m_decision_wait = .5f;
	}

	glm::vec3 lateral_vector_to_player(static_flying_enemy& body);

	static std::vector<engine::ref<static_flying_enemy>> m_active_flyers_this_tick;

};

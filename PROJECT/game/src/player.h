#pragma once

#include <engine.h>
#include "glm/gtx/rotate_vector.hpp"
class player
{
public:
	player(glm::vec3 position = { 0.f,0.f,0.f });
	~player();
	void initialise(engine::ref<engine::game_object> object);
	void on_update(const engine::timestep& time_step);
	engine::ref<engine::game_object> object() const { return m_object; }
	void turn(float angle);
	void update_camera(engine::perspective_camera& camera);
	void jump();
	void on_event(engine::event& e);
private:	
	void switch_animation(uint32_t anim);


	float m_walk_speed{ 1.f };
	float m_run_speed{ 2.f };
	float m_jump_timer{0.f};
	float m_camera_backoff_distance;
	static const float camera_backoff_distance_default;

	engine::ref< engine::game_object> m_object;

	const uint32_t m_anim_walk;
	const uint32_t m_anim_run;
	const uint32_t m_anim_idle;
	const uint32_t m_anim_jump;

	uint32_t m_current_animation;
};

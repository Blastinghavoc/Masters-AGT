#pragma once

#include <engine.h>
#include "glm/gtx/rotate_vector.hpp"

/*
Class to represnt the player character and the controls it has.
Heavily modified, but originally based on the one from Lab 4
*/
class player
{
public:
	player(glm::vec3 position = { 0.f,0.f,0.f });
	~player();

	//Update the player based on user input and delta-time
	void on_update(const engine::timestep& time_step);

	//Update the camera to provide a 3rd person perspective of the player.
	void update_camera(engine::perspective_camera& camera);

	//Plays the jump animation, but currently does not change the trajectory of the player.
	void jump();

	//Respond to events, specifically the mouse scroll event
	void on_event(engine::event& e);

	engine::ref<engine::game_object> object() const { return m_object; }

private:
	//Change the animation to play
	void switch_animation(uint32_t anim);

	const float m_walk_speed{ 1.f };
	const float m_run_speed{ 2.f };

	float m_jump_timer{0.f};

	//camera details (distance behind the player to put the camera)
	const float camera_backoff_distance_default{1.f};
	float m_camera_backoff_distance;

	//The gameobject associated with the player
	engine::ref< engine::game_object> m_object;

	//Constants defining the indices of particular animations
	const uint32_t m_anim_walk;
	const uint32_t m_anim_run;
	const uint32_t m_anim_idle;
	const uint32_t m_anim_jump;

	//Index of the current animation
	uint32_t m_current_animation;
};

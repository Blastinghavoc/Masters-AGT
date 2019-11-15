#pragma once

#include "animated_actor.h"

/*
Class to represnt the player character and the controls it has.
Heavily modified, but originally based on the one from Lab 4
*/
class player: public animated_actor
{
public:
	player(glm::vec3 position = { 0.f,0.f,0.f });
	~player();

	//Update the player based on user input and delta-time
	void on_update(const engine::timestep& time_step) override;

	void move_physics(const glm::vec3& direction, const float& speed);

	//Update the camera to provide a 3rd person perspective of the player.
	void update_camera(engine::perspective_camera& camera);

	//Plays the jump animation, but currently does not change the trajectory of the player.
	void jump();

	//Respond to events, specifically the mouse scroll event
	void on_event(engine::event& e);

private:
	const float m_walk_speed{ 1.f };
	const float m_run_speed{ 2.f };
	glm::vec3 m_rotation_axis{ 0,1,0 };
	glm::vec3 m_camera_forward{0,0,1};

	float m_jump_timer{0.f};

	//camera details (distance behind the player to put the camera)
	const float camera_backoff_distance_default{1.f};
	float m_camera_backoff_distance;	
};

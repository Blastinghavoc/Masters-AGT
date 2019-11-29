#pragma once

#include "animated_actor.h"
#include "trigger_box.h"

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

	void on_render(const engine::ref<engine::shader>& shader);

	//Update the camera to provide a 3rd person perspective of the player.
	void update_camera(engine::perspective_camera& camera);

	void jump(const engine::timestep& ts);

	//Respond to events, specifically the mouse scroll event
	void on_event(engine::event& e);

	trigger_box& get_trigger_box() { return m_box; };

	void set_health(float amount) { m_health = amount; };

	const float& health() { return m_health; };

	const float& max_health() { return m_max_health; };

	void deal_damage(float amount) { m_health -= amount; };

	const glm::vec3& spawnpoint() { return m_spawn_point; };

private:
	const float m_walk_speed{ 1.f };
	const float m_run_speed{ 2.f };
	glm::vec3 m_rotation_axis{ 0,1,0 };
	glm::vec3 m_camera_forward{0,0,1};

	//Counts down how much time is left for the jump animation
	float m_jump_timer{0.f};

	//As above, for dancing. Just for fun!
	float m_dance_timer{ 0.f };

	//camera details (distance behind the player to put the camera)
	const float camera_backoff_distance_default{1.f};
	float m_camera_backoff_distance;
	const float m_camera_elevation{ .5f };

	//Gameplay stats
	trigger_box m_box{};
	float m_health{ 100.f };
	float m_max_health{ 100.f };
	glm::vec3 m_spawn_point{ 0.f };
};

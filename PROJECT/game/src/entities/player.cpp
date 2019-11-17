#include "player.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "engine/events/mouse_event.h"

player::player(glm::vec3 position):
	m_camera_backoff_distance{camera_backoff_distance_default}
{
	//load and intialise the player gameobject/mesh
	engine::ref<engine::skinned_mesh> skinned_mesh = engine::skinned_mesh::create("assets/models/animated/mannequin/free3Dmodel.dae");
	skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/walking.dae");
	skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/idle.dae");
	skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/jump.dae");
	skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/standard_run.dae");
	skinned_mesh->switch_root_movement(false);
	engine::game_object_properties props;
	props.animated_mesh = skinned_mesh;
	props.scale = glm::vec3(1.f / glm::max(skinned_mesh->size().x, glm::max(skinned_mesh->size().y, skinned_mesh->size().z)));
	props.type = 0;
	props.is_static = false;
	props.position = position;
	props.friction = 0.8f;
	props.mass = 55;
	props.type = 0;
	props.bounding_shape = glm::vec3(skinned_mesh->size().x / 4.f,
		skinned_mesh->size().y / 2.f, skinned_mesh->size().x / 4.f);

	m_animations["walk"] = 1;
	m_animations["idle"] = 2;
	m_animations["jump"] = 3;
	m_animations["run"] = 4;

	m_object = engine::game_object::create(props);

	m_object->set_offset(skinned_mesh->offset());
	m_object->set_angular_factor({ 0,0,0 });

	m_object->set_forward(glm::vec3(0.f, 0.f, 1.f));
	m_object->animated_mesh()->set_default_animation(1);
	m_object->animated_mesh()->switch_animation(m_object->animated_mesh()->default_animation());
	m_object->set_rotation_axis(m_rotation_axis);

	m_box.set_box(props);
	m_spawn_point = position;
}

player::~player()
{
}

void player::on_update(const engine::timestep& time_step)
{
	m_box.on_update(m_object->position());

	bool jumping = false;

	//Update the jump timer
	if (m_jump_timer > 0.f)
	{
		m_jump_timer -= (float)time_step;		
		jumping = true;		
	}

	glm::vec3 forward_direction = m_camera_forward;
	glm::vec3 movement_direction{ 0.f,0.f,0.f };
	float speed = m_walk_speed;
	auto movement_animation = m_animations["walk"];

	//Directional movement
	if (engine::input::key_pressed(engine::key_codes::KEY_W))
	{		
		movement_direction += forward_direction;
	}
	if (engine::input::key_pressed(engine::key_codes::KEY_A))
	{
		movement_direction += glm::rotate(forward_direction,(float)M_PI_2, m_rotation_axis);
	}
	if (engine::input::key_pressed(engine::key_codes::KEY_D))
	{
		movement_direction += glm::rotate(forward_direction, (float)-M_PI_2, m_rotation_axis);
	}
	if (engine::input::key_pressed(engine::key_codes::KEY_S))
	{
		movement_direction += glm::rotate(forward_direction, (float)M_PI, m_rotation_axis);
	}

	movement_direction = glm::normalize(movement_direction);	

	//Sprint
	if (engine::input::key_pressed(engine::key_codes::KEY_LEFT_SHIFT))
	{
		speed = m_run_speed;
		movement_animation = m_animations["run"];
	}	

	//Possibly jump if not already jumping
	if (!jumping && engine::input::key_pressed(engine::key_codes::KEY_SPACE))
	{
		jump(time_step);
		jumping = true;
	}

	//If player has a direction, move
	if (glm::length(movement_direction) > 0.0f)
	{
		//move(movement_direction, speed, time_step);
		move_physics(movement_direction,speed);

		//Play the movement animation if we aren't already (and we're not jumping)
		if (!jumping && m_current_animation != movement_animation)
		{
			switch_animation(movement_animation);
		}		
	}
	else {//Otherwise, idle if not jumping
		if (!jumping && m_current_animation != m_animations["idle"])
		{
			switch_animation(m_animations["idle"]);
		}
	}

	//Increment animation
	m_object->animated_mesh()->on_update(time_step);
}

void player::move_physics(const glm::vec3& direction, const float& speed)
{
	auto speed_in_direction = glm::dot(m_object->velocity(), direction) / glm::length(direction);
	auto adjustment = direction * (speed - speed_in_direction);
	//Set the speed in the desired direction without affecting speed in other directions.
	m_object->set_velocity(m_object->velocity() + adjustment);

	m_object->set_rotation_axis(m_rotation_axis);
	m_object->set_rotation_amount(atan2(direction.x, direction.z));
	m_object->animated_mesh()->switch_root_movement(false);
}


//handle the 3rd person camera
void player::update_camera(engine::perspective_camera& camera)
{
	glm::vec3 upward_unit{ 0,1,0 };

	//Makes the camera face where the player "looks" with the mouse
	camera.update_facing_from_mouse();

	auto forward_unit = glm::normalize(camera.front_vector());

	//Adjusts the position of the camera to be 3rd person.
	//A particular distance backwards and upwards from the player
	camera.position(m_object->position() + (-m_camera_backoff_distance * forward_unit) + (m_camera_elevation * upward_unit));

	//Set the forward direction of the gameobject to be where we're looking, so the player can run in a direction by just looking there.
	m_camera_forward = glm::vec3(forward_unit.x, 0.f, forward_unit.z);

	//m_object->set_forward(glm::normalize(glm::vec3(forward_unit.x,0.f,forward_unit.z)));
	/*auto cam_angle = atan2(forward_unit.x, forward_unit.z);
	auto current_angle = atan2(m_object->forward().x, m_object->forward().z);
	auto correction = cam_angle - current_angle;*/
	/*m_object->set_rotation_axis(m_rotation_axis);
	m_object->set_rotation_amount(cam_angle);*/
	//m_object->set_angular_velocity(glm::vec3(0.f, correction, 0.f));
}

//Play the jump animation.
void player::jump(const engine::timestep& ts)
{
	auto time_compensation = 1 / ts;
	switch_animation(m_animations["jump"]);
	m_jump_timer = (float)(m_object->animated_mesh()->animations().at(m_animations["jump"])->mDuration);
	m_object->set_acceleration(m_object->acceleration() + glm::vec3(0, time_compensation * m_object->mass()*8, 0));
}

//Handle events sent to the player
void player::on_event(engine::event& event) {

	//zoom
	if (event.event_type() == engine::event_type_e::mouse_scrolled) {
		auto& e = dynamic_cast<engine::mouse_scrolled_event&>(event);

		//each scroll increment slightly moves the camera closer/further up to a max/min
		//Min is practically first person.
		float amnt = (e.y_offset() > 0)? -.1f:.1f;		
		if (engine::input::key_pressed(engine::key_codes::KEY_LEFT_CONTROL))
		{
			m_camera_backoff_distance = std::clamp(m_camera_backoff_distance + amnt,0.f,3.f*camera_backoff_distance_default);
		}

		e.handled = true;
	}
}

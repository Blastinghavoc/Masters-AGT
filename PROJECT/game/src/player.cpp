#include "player.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "engine/events/mouse_event.h"

player::player(glm::vec3 position):
	m_anim_walk{ 1 },
	m_anim_idle{ 2 },
	m_anim_jump{ 3 },
	m_anim_run{ 4 },
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
	props.position = position;
	props.bounding_shape = skinned_mesh->size() / 2.f * props.scale.x;	

	m_object = engine::game_object::create(props);
	m_object->set_forward(glm::vec3(0.f, 0.f, 1.f));
	m_object->animated_mesh()->set_default_animation(1);
	m_object->animated_mesh()->switch_animation(m_object->animated_mesh()->default_animation());
	m_object->set_rotation_axis({ 0,1,0 });
}

player::~player()
{
}

void player::on_update(const engine::timestep& time_step)
{
	bool jumping = false;

	//Update the jump timer
	if (m_jump_timer > 0.f)
	{
		m_jump_timer -= (float)time_step;		
		jumping = true;		
	}

	glm::vec3 forward_direction = m_object->forward();
	glm::vec3 movement_direction{ 0.f,0.f,0.f };
	float speed = m_walk_speed;
	auto movement_animation = m_anim_walk;

	//Directional movement
	if (engine::input::key_pressed(engine::key_codes::KEY_W))
	{		
		movement_direction += forward_direction;
	}
	if (engine::input::key_pressed(engine::key_codes::KEY_A))
	{
		movement_direction += glm::rotate(forward_direction,(float)M_PI_2,m_object->rotation_axis());		
	}
	if (engine::input::key_pressed(engine::key_codes::KEY_D))
	{
		movement_direction += glm::rotate(forward_direction, (float)-M_PI_2, m_object->rotation_axis());
	}
	if (engine::input::key_pressed(engine::key_codes::KEY_S))
	{
		movement_direction += glm::rotate(forward_direction, (float)M_PI, m_object->rotation_axis());
	}

	movement_direction = glm::normalize(movement_direction);	

	//Sprint
	if (engine::input::key_pressed(engine::key_codes::KEY_LEFT_SHIFT))
	{
		speed = m_run_speed;
		movement_animation = m_anim_run;
	}	

	//Possibly jump if not already jumping
	if (!jumping && engine::input::key_pressed(engine::key_codes::KEY_SPACE))
	{
		jump();
		jumping = true;
	}

	//If player has a direction, move
	if (glm::length(movement_direction) > 0.0f)
	{
		//Increment position
		m_object->set_position(m_object->position() += movement_direction * speed* (float)time_step);

		//orient in direction of travel
		m_object->set_rotation_amount(atan2(movement_direction.x, movement_direction.z));
		m_object->animated_mesh()->switch_root_movement(false);

		//Play the movement animation if we aren't already (and wer're not jumping)
		if (!jumping && m_current_animation != movement_animation)
		{
			switch_animation(movement_animation);
		}		
	}
	else {//Otherwise, idle if not jumping
		if (!jumping && m_current_animation != m_anim_idle)
		{
			switch_animation(m_anim_idle);
		}
	}

	//Increment animation
	m_object->animated_mesh()->on_update(time_step);
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
	camera.position(m_object->position() + (-m_camera_backoff_distance * forward_unit) + (1.f * upward_unit));

	//Set the forward direction of the gameobject to be where we're looking, so the player can run in a direction by just looking there.
	m_object->set_forward(glm::normalize(glm::vec3(forward_unit.x,0.f,forward_unit.z)));
}

//Play the jump animation. TODO actually move the player when physics is added
void player::jump()
{
	switch_animation(m_anim_jump);
	m_jump_timer = (float)(m_object->animated_mesh()->animations().at(m_anim_jump)->mDuration);
}

//Utility function to change the currently playing animation
void player::switch_animation(uint32_t anim) {
	m_object->animated_mesh()->switch_animation(anim);
	m_current_animation = anim;
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

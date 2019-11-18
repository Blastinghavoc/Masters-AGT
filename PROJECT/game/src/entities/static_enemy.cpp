#include "pch.h"
#include "static_enemy.h"
#include <glm/gtx/norm.hpp>

std::map<enemy_type, engine::game_object_properties> static_enemy::prefabs;

static_enemy::static_enemy() :abstract_enemy{ -1 }
{
	m_type = enemy_type::robot1;
}

static_enemy::static_enemy(int id, glm::vec3 position, enemy_type type) : abstract_enemy{ id }
{
	m_type = type;
	construct_gameobject(type);
	m_object->set_position(position + glm::vec3(0,m_y_offset,0));
	m_box.set_box(m_object);
}

void static_enemy::on_update(const engine::timestep& time_step)
{
	//Modified from abstract_enemy::on_update
	{
		while (!m_waypoints.empty() && close_enough(ground_position(),m_waypoints.front()))
		{
			m_waypoints.pop_front();
		}

		m_box.on_update(m_object->position());
	}

	if (m_frozen)
	{
		return;//can't move if frozen
	}

	if (!m_waypoints.empty())
	{
		//Try to go towards the first waypoint that we aren't already at
		auto target = m_waypoints.front();
		target.y += m_y_offset;//Account for the fact that this object may be flying

		auto line = target - m_object->position();
		auto direction = glm::normalize(line);

		move(direction, m_movement_speed, time_step, glm::length(line));

	}
	
}

float static_enemy::square_distance_to_next_waypoint() const
{	
	return glm::distance2(ground_position(), next_waypoint());;
}

glm::vec3 static_enemy::ground_position() const
{
	return m_object->position() - glm::vec3(0, m_y_offset, 0);
}

/*
Constructs the required game object for any enemy type.
*/
void static_enemy::construct_gameobject(enemy_type type)
{
	engine::game_object_properties props;
	std::string model_path;
	std::string texture_path;
	float health;
	float scale_modifier = 1.f;
	float altitude = 0.f;
	float speed = 1.f;
	switch (type)
	{
	case enemy_type::flyer1:
		{		
		model_path = "assets/models/static/rts_models/Flyer1.obj";
		texture_path = "assets/models/static/rts_models/textures/Flyer1.png";
		health = 50.f;
		altitude = 3.f;
		speed = 2.5f;
		}
		break;
	case enemy_type::flyer2:
		{
		model_path = "assets/models/static/rts_models/Flyer2.obj";
		texture_path = "assets/models/static/rts_models/textures/Flyer2.png";
		health = 100.f;
		altitude = 4.f;
		speed = 2.f;
		}
		break;
	case enemy_type::robot1:
		{
		model_path = "assets/models/static/rts_models/Robot.obj";
		texture_path = "assets/models/static/rts_models/textures/Robot1.png";//Both robots have the same model, but different textures
		health = 100.f;
		}
		break;
	case enemy_type::robot2:
		{
		model_path = "assets/models/static/rts_models/Robot.obj";
		texture_path = "assets/models/static/rts_models/textures/Robot2.png";
		health = 250.f;
		scale_modifier = 1.25f;
		}
		break;
	default:
		throw std::runtime_error("Static enemy cannot create enemy of requested type");
		break;
	}


	if (prefabs.count(type) > 0)
	{
		props = prefabs[type];
	}
	else {
		engine::ref <engine::model> model = engine::model::create(model_path);
		props.is_static = false;
		props.type = 0;
		props.meshes = model->meshes();
		auto tex = engine::texture_2d::create(texture_path, false);
		props.textures = { tex };
		float scale = 1.f / glm::max(model->size().x, glm::max(model->size().y, model->size().z));
		props.scale = scale_modifier * glm::vec3(scale);
		props.bounding_shape = model->size() / 2.f;
		//props.offset = model->offset();

		prefabs[type] = props;
	}

	m_object = engine::game_object::create(props);
	m_health = health;
	m_max_health = health;
	m_y_offset = altitude;
	m_movement_speed = speed;
}

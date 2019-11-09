#include "enemy.h"

bool enemy::prefab_ready = false;//TODO remove/rework
engine::game_object_properties enemy::prefab{};

enemy::enemy() :m_id{-1}
{
}

enemy::enemy(int id, glm::vec3 position) :m_id{id}
{
	engine::game_object_properties props;
	/*if (prefab_ready)
	{
		props = prefab;
		props.position = position;
		props.animated_mesh = std::make_shared<engine::skinned_mesh>(*props.animated_mesh);
	}
	else {*/
		//Currently using same model as the player
		engine::ref<engine::skinned_mesh> skinned_mesh = engine::skinned_mesh::create("assets/models/animated/mannequin/free3Dmodel.dae");
		skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/walking.dae");
		skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/idle.dae");
		skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/jump.dae");
		skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/standard_run.dae");
		skinned_mesh->switch_root_movement(false);
		props;
		props.animated_mesh = skinned_mesh;
		props.scale = glm::vec3(1.f / glm::max(skinned_mesh->size().x, glm::max(skinned_mesh->size().y, skinned_mesh->size().z)));
		props.type = 0;
		props.position = position;
		props.bounding_shape = skinned_mesh->size() / 2.f * props.scale.x;

		prefab = props;
		prefab_ready = true;
	//}

	m_animations["walk"] = 1;
	m_animations["idle"] = 2;
	m_animations["jump"] = 3;
	m_animations["run"] = 4;

	m_object = engine::game_object::create(props);
	m_object->set_forward(glm::vec3(0.f, 0.f, 1.f));
	m_object->animated_mesh()->set_default_animation(1);
	m_object->animated_mesh()->switch_animation(m_object->animated_mesh()->default_animation());
	m_object->set_rotation_axis({ 0,1,0 });
}

enemy::~enemy()
{
}

void enemy::on_update(const engine::timestep& time_step)
{
	//Remove any waypoints from the front of the queue if we're already there.
	while (!m_waypoints.empty() && m_object->position() == m_waypoints.front())
	{
		m_waypoints.pop_front();
	}
	

	if (!m_waypoints.empty())
	{
		//Try to go towards the first waypoint that we aren't already at
		auto target = m_waypoints.front();

		auto run = m_animations["run"];
		if (m_current_animation != run)
		{
			switch_animation(run);
		}

		auto line = target - m_object->position();
		auto direction = glm::normalize( line );
		float speed = 2.f;

		move(direction,speed,time_step,glm::length(line));

	}
	else {
		//Become idle
		auto idle = m_animations["idle"];
		if (m_current_animation != idle)
		{
			switch_animation(idle);
		}
	}

	//Increment animation
	m_object->animated_mesh()->on_update(time_step);
}

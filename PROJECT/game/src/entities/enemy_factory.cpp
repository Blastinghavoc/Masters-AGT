#include "pch.h"
#include "enemy_factory.h"

std::map<enemy_type, engine::game_object_properties> enemy_factory::prefabs;

/*
Constructs the required object for any enemy type.
*/
engine::ref<abstract_enemy> enemy_factory::create(int id, glm::vec3 position, enemy_type type)
{
	engine::ref<abstract_enemy> new_enemy;
	engine::game_object_properties props;
	std::string model_path;
	std::string texture_path;
	float health;
	float scale_modifier = 1.f;
	float altitude = 0.f;
	float speed = 1.f;
	glm::vec3 targetting_offset = { 0,0,0 };

	bool is_flyer = false;
	switch (type)
	{
	case enemy_type::flyer1:
	{
		model_path = "assets/models/static/rts_models/Flyer1.obj";
		texture_path = "assets/models/static/rts_models/textures/Flyer1.png";
		health = 50.f;
		altitude = 3.f;
		speed = 2.5f;
		is_flyer = true;
	}
	break;
	case enemy_type::flyer2:
	{
		model_path = "assets/models/static/rts_models/Flyer2.obj";
		texture_path = "assets/models/static/rts_models/textures/Flyer2.png";
		health = 100.f;
		altitude = 4.f;
		speed = 2.f;
		is_flyer = true;
	}
	break;
	case enemy_type::robot1:
	{
		model_path = "assets/models/static/rts_models/Robot.obj";
		texture_path = "assets/models/static/rts_models/textures/Robot1.png";//Both robots have the same model, but different textures
		health = 100.f;
		targetting_offset.y = .7f;
	}
	break;
	case enemy_type::robot2:
	{
		model_path = "assets/models/static/rts_models/Robot.obj";
		texture_path = "assets/models/static/rts_models/textures/Robot2.png";
		health = 250.f;
		scale_modifier = 1.25f;
		targetting_offset.y = scale_modifier* .7f;
	}
	break;
	case enemy_type::animated_humanoid:
		new_enemy = animated_enemy::create(id);
		health = 100.f;
		speed = 2.f;
		targetting_offset.y = .7f;
		break;
	default:
		throw std::runtime_error("Cannot create static enemy of requested type");
		break;
	}


	if (type != enemy_type::animated_humanoid)//Animated humanoids handle themselves (for now)
	{
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

		if (is_flyer)
		{
			new_enemy = std::make_shared<static_flying_enemy>(id, engine::game_object::create(props), altitude, type);
			position.y = altitude;//Ensure flyer is created at correct height.
		}
		else {
			new_enemy = std::make_shared<static_enemy>(id, engine::game_object::create(props), type);
		}
	}

	new_enemy->set_health(health);
	new_enemy->set_max_health(health);
	new_enemy->set_movement_speed(speed);
	new_enemy->object()->set_position(position);
	new_enemy->set_targetting_offset(targetting_offset);

	return new_enemy;
}

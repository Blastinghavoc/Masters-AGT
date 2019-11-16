#include "pch.h"
#include "grenade.h"

bool grenade::prefab_ready = false;
engine::game_object_properties grenade::prefab{};

grenade::grenade(float scale,float lifetime)
{
	engine::game_object_properties props;
	if (!prefab_ready)
	{
		engine::ref<engine::texture_2d> rhombi_texture_sqr = engine::texture_2d::create("assets/textures/rhombi_bomb_face_sqr.png", true);//These textures made by me
		engine::ref<engine::texture_2d> rhombi_texture_tri = engine::texture_2d::create("assets/textures/rhombi_bomb_face_tri.png", true);
		engine::ref<engine::rhombicuboctahedron> rhombi_shape = engine::rhombicuboctahedron::create(1.f);
		auto radius = 1.f + (float)sqrt(2) / 2.f;//radius of rhombi with edge length 1 is 1+ root(2)/2
		props.position = { 0, 0, 0 };
		props.meshes = rhombi_shape->meshes();
		props.textures = { rhombi_texture_sqr ,rhombi_texture_tri };
		props.bounding_shape = glm::vec3(radius);
		props.offset = glm::vec3(0,-radius,0);
		props.mass = 10;
		props.type = 1;
		props.restitution = 0.85f;
		props.rolling_friction = 0.2f;
		props.scale = glm::vec3(scale);

		prefab = props;
		prefab_ready = true;
	}
	else {
		props = prefab;
		props.scale = glm::vec3(scale);
	}
	m_object = engine::game_object::create(props);
	m_lifetime = lifetime;
	m_box.set_box(props);
}

void grenade::on_update(const engine::timestep& ts)
{
	if (!m_active)
	{
		return;
	}

	m_box.on_update(m_object->position());

	if (m_life_timer.total() > m_lifetime)
	{
		m_active = false;
	}
}

void grenade::launch(glm::vec3 start, glm::vec3 direction, float force)
{
	m_life_timer.start();
	m_object->set_position(start);
	m_object->set_acceleration(m_object->acceleration() + direction * force * m_object->mass());
	m_active = true;
}

void grenade::render(const engine::ref<engine::shader>& shader)
{
	if (m_active)
	{
		engine::renderer::submit(shader, m_object);
	}
}

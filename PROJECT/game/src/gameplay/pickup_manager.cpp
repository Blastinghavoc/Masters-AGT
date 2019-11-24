#include "pickup_manager.h"
#include "../ai/enemy_manager.h"
#include "../gameplay/gameplay_manager.h"

bool pickup_manager::m_powerup_active;

pickup pickup_manager::m_freeze_time_pickup;
engine::timer pickup_manager::m_freeze_timer{};
bool pickup_manager::m_freeze_active = false;

pickup pickup_manager::m_invincibility_pickup;
engine::timer pickup_manager::m_invincibility_timer{};
bool pickup_manager::m_invincibility_active = false;
engine::ref<hud_element> pickup_manager::m_invincibility_overlay;

engine::ref<engine::SpotLight> pickup_manager::m_spot_light;

void pickup_manager::init()
{
	engine::game_object_properties props;
	engine::ref<engine::texture_2d> tex = engine::texture_2d::create("assets/textures/freeze_powerup.png", false);
	float size = 0.3f;
	engine::ref<engine::hollow_cuboid> shape = engine::hollow_cuboid::create({ size,size,size }, size / 2, 2 * size);
	props.meshes = shape->meshes();
	props.textures = { tex };
	props.bounding_shape = glm::vec3(size);
	props.offset = { 0,-size,0 };
	props.position = glm::vec3(0);
	props.rotation_axis = { 0,1,0 };
	m_freeze_time_pickup = pickup(engine::game_object::create(props),.4f);

	tex = engine::texture_2d::create("assets/textures/invincibility_powerup.png", false);
	props.textures = { tex };
	m_invincibility_pickup = pickup(engine::game_object::create(props), .4f);

	auto overlay = hud_element::create({ .5f,.5f }, { 1,1 }, engine::texture_2d::create("assets/textures/invincibility_overlay.png", true));
	hud_manager::add_element(overlay);	
	overlay->set_z_order(0.11f);//Just a little bit ahead of the rest of the HUD so that it appears over all of it.
	overlay->hide();
	m_invincibility_overlay = overlay;

	srand(time(nullptr));//Initialise random number generator.

	m_spot_light = std::make_shared<engine::SpotLight>();
	m_spot_light->Color = glm::vec3(.1f, .1f, .75f);
	m_spot_light->AmbientIntensity = 0.4f;
	m_spot_light->DiffuseIntensity = 0.6f;
	m_spot_light->Position = glm::vec3{ 0,0,0 };
	m_spot_light->Attenuation.Constant = .1f;
	m_spot_light->Attenuation.Linear = .1f;
	m_spot_light->Attenuation.Exp = .01f;
	m_spot_light->Direction = glm::vec3(0, -1, 0);
	m_spot_light->Cutoff = .9f;
	m_spot_light->On = false;

	light_manager::spot_lights.push_back(m_spot_light);
}

void pickup_manager::on_update(const engine::timestep& ts, trigger_box& player_trigger_box)
{
	if (m_freeze_active && m_freeze_timer.total() > m_freeze_duration)
	{
		unfreeze_enemies();
	}

	if (m_invincibility_active && m_invincibility_timer.total() > m_invincibility_duration)
	{
		set_invincibility(false);
	}

	if (m_freeze_time_pickup.is_active())
	{
		m_freeze_time_pickup.on_update(ts);
		if (m_freeze_time_pickup.get_trigger_box().collision(player_trigger_box))
		{
			m_freeze_time_pickup.set_active(false);
			m_powerup_active = false;
			m_spot_light->On = false;			
			freeze_enemies();
		}
	}
	else if (m_invincibility_pickup.is_active()) {
		m_invincibility_pickup.on_update(ts);
		if (m_invincibility_pickup.get_trigger_box().collision(player_trigger_box))
		{
			m_invincibility_pickup.set_active(false);
			m_powerup_active = false;
			m_spot_light->On = false;
			set_invincibility(true);
		}
	}
}

void pickup_manager::render(const engine::ref<engine::shader>& shader)
{
	if (m_freeze_time_pickup.is_active())
	{
		engine::renderer::submit(shader, m_freeze_time_pickup.object());
	}

	if (m_invincibility_pickup.is_active())
	{
		engine::renderer::submit(shader, m_invincibility_pickup.object());
	}
}

/*
Create a powerup at the given position with some probability.
The probability is zero if a powerup is already active.
*/
void pickup_manager::roll_for_powerup(glm::vec3 position)
{
	if (m_powerup_active)
	{
		return;
	}

	auto value = rand()% 100;
	if (value <= m_powerup_probability)
	{
		make_powerup_at(position);
	}
}

/*
Makes a random powerup at the given position
*/
void pickup_manager::make_powerup_at(glm::vec3 position)
{
	if (m_powerup_active)
	{
		return;//only one powerup may be present at a time
	}

	auto value = rand() % 100;
	if (value < 50)//Equal chance
	{//Freeze
		m_freeze_time_pickup.set_active(true);
		m_freeze_time_pickup.object()->set_position(position);
		m_powerup_active = true;

		m_spot_light->On = true;
		m_spot_light->Color = glm::vec3(.1f, .1f, .75f);
		m_spot_light->Position = m_freeze_time_pickup.object()->position();

	}
	else {
		m_invincibility_pickup.set_active(true);
		m_invincibility_pickup.object()->set_position(position);
		m_powerup_active = true;

		m_spot_light->On = true;
		m_spot_light->Color = glm::vec3(1.f, .84f, .18f);
		m_spot_light->Position = m_invincibility_pickup.object()->position();
	}

	m_spot_light->Position.y += 4.f;
}

void pickup_manager::freeze_enemies()
{	
	auto& enemies = enemy_manager::get_active_enemies();
	for (auto& enem : enemies) {
		enem->set_frozen(true);
	}
	m_freeze_timer.start();
	m_freeze_active = true;
}

void pickup_manager::unfreeze_enemies()
{
	auto& enemies = enemy_manager::get_active_enemies();
	for (auto& enem : enemies) {
		enem->set_frozen(false);
	}
	m_freeze_active = false;
}

void pickup_manager::set_invincibility(bool flag)
{
	if (flag)
	{
		m_invincibility_timer.start();
		m_invincibility_active = true;
		m_invincibility_overlay->show();
	}
	else {
		m_invincibility_active = false;
		m_invincibility_overlay->hide();
	}

	gameplay_manager::set_player_invincible(flag);
}

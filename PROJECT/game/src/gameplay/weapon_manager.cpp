#include "pch.h"
#include "weapon_manager.h"
#include "../physics/physics_manager.h"
#include "../sfx/sfx_manager.h"
#include "../ai/enemy_manager.h"
#include "gameplay_manager.h"

std::vector<engine::ref<grenade>> weapon_manager::m_grenades{};
std::vector<engine::ref<grenade>> weapon_manager::m_active_grenades{};
engine::timer weapon_manager::m_grenade_cooldown_timer;
engine::timer weapon_manager::m_charge_timer;
bool weapon_manager::m_is_charging = false;

void weapon_manager::init()
{
	//Maximum of 5 grenades
	for (size_t i = 0; i < 5; ++i)
	{
		auto nade = grenade::create(0.125f, 3.f);
		nade->object()->set_active(false);
		physics_manager::add(nade->object());
		m_grenades.push_back(nade);
	}
	m_grenade_cooldown_timer.start();
}


void weapon_manager::update(const engine::timestep& ts)
{
	auto& active_enemies = enemy_manager::get_active_enemies();
	for (auto gren_it = begin(m_active_grenades); gren_it != end(m_active_grenades);) {
		auto& gren = (*gren_it);
		gren->on_update(ts);

		//Collision with enemy deactivates the grenade, causing it to explode
		for (auto& enemy_ptr:active_enemies)
		{
			if (enemy_ptr->get_trigger_box().collision(gren->get_trigger_box()))
			{
				enemy_ptr->deal_damage(m_grenade_damage);
				gren->set_active(false);
			}
		}


		if (gren->is_active())
		{
			++gren_it;
		}
		else {
			//No longer active, so move back into pool

			//Explosion time!
			auto grenade_position = gren->object()->position();
			sfx_manager::explode_at(grenade_position);

			//m_audio_manager->play("grenade_explosion");
			gameplay_manager::audio_manager()->play_spatialised_sound("grenade_explosion", grenade_position,5.f);

			//Deactivate the object			
			gren->object()->set_active(false);
			m_grenades.push_back(gren);
			gren_it = m_active_grenades.erase(gren_it);
		}
	}
}

void weapon_manager::on_render(const engine::ref<engine::shader>& shader)
{
	for (auto& nade : m_active_grenades) {
		nade->render(shader);
	}
}

void weapon_manager::render_trigger_boxes(const engine::ref<engine::shader>& shader)
{
	for (auto& nade : m_active_grenades) {
		nade->get_trigger_box().on_render(shader);
	}
}

void weapon_manager::start_charging()
{
	m_is_charging = true;
	m_charge_timer.start();
}

/*
Launches a grenade, with extra force depending on the amount it has been charged up
if applicable
*/
void weapon_manager::launch_charged(glm::vec3 start, glm::vec3 direction, const engine::timestep& ts, float force)
{
	if (m_is_charging)
	{
		float charged_force = force * charge_percentage() * m_max_charge_multiplier;
		m_is_charging = false;
		launch_grenade(start, direction, ts, charged_force);
	}
	else {
		launch_grenade(start, direction,ts, force);
	}
}

/*
Needs timestep so that acceleration can be adjusted to be framerate independent
*/
void weapon_manager::launch_grenade(glm::vec3 start, glm::vec3 direction, const engine::timestep& ts,float force)
{
	if (!m_grenades.empty() && can_fire_grenade())
	{
		auto nade = m_grenades.back();
		m_active_grenades.push_back(nade);
		m_grenades.pop_back();
		nade->object()->set_active(true);
		nade->launch(start, direction, (1/ts.seconds())*force);//Scale with timestep
		gameplay_manager::audio_manager()->play("grenade_launch");
		m_grenade_cooldown_timer.reset();
	}
}

float weapon_manager::charge_percentage()
{
	if (m_is_charging)
	{
		return std::min(std::max(m_base_charge_time, (float)m_charge_timer.total()) / m_max_charge_time,m_max_charge_time);
	}
	return 0.0f;
}

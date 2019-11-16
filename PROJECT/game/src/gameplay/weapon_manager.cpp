#include "pch.h"
#include "weapon_manager.h"
#include "../physics/physics_manager.h"
#include "../sfx/sfx_manager.h"
#include "../ai/enemy_manager.h"

std::vector<engine::ref<grenade>> weapon_manager::m_grenades{};
std::vector<engine::ref<grenade>> weapon_manager::m_active_grenades{};
engine::ref<engine::audio_manager> weapon_manager::m_audio_manager;

void weapon_manager::init(engine::ref<engine::audio_manager> audio_manager)
{
	m_audio_manager = audio_manager;
	//Maximum of 5 grenades
	for (size_t i = 0; i < 5; ++i)
	{
		auto nade = grenade::create(0.125f, 3.f);
		nade->object()->set_active(false);
		physics_manager::add(nade->object());
		m_grenades.push_back(nade);
	}
}

void weapon_manager::launch_grenade(glm::vec3 start, glm::vec3 direction, float force)
{
	if (!m_grenades.empty())
	{
		auto nade = m_grenades.back();
		m_active_grenades.push_back(nade);
		m_grenades.pop_back();
		nade->object()->set_active(true);
		nade->launch(start, direction, force);
		m_audio_manager->play("grenade_launch");
	}
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
			sfx_manager::explode_at(gren->object()->position());
			m_audio_manager->play("grenade_explosion");

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

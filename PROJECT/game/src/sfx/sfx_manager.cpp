#include "pch.h"
#include "sfx_manager.h"
#include "../gui/hud_manager.h"

camera_jitter sfx_manager::jitter_effect(nullptr);
cross_fade sfx_manager::cross_fade_effect(nullptr);
std::vector<engine::ref<billboard>> sfx_manager::m_active_explosions{};
std::vector < engine::ref<billboard>> sfx_manager::m_explosion_pool{};
engine::ref<billboard> sfx_manager::m_explosion_prefab{};

void sfx_manager::init(engine::perspective_camera* camera)
{	
	jitter_effect = camera_jitter(camera);

	auto panel = hud_element::create({ .5f,.5f }, { 1,1 }, engine::texture_2d::create("assets/textures/cross_fade.png", true));
	hud_manager::add_element(panel);
	cross_fade_effect = cross_fade(panel);
	panel->set_z_order(0.11f);//Just a little bit ahead of the rest of the HUD so that it appears over all of it.
	panel->hide();

	//Explosion sprite sheet from https://opengameart.org/content/pixel-explosion-12-frames
	m_explosion_prefab = billboard::create("assets/textures/Explosion.png", 12, 1, 12);
	m_explosion_pool.push_back(std::make_shared<billboard>(*m_explosion_prefab));//Copy an explosion into the pool.
}

//Activate an explosion at a position, reusing from pool if possible.
void sfx_manager::explode_at(glm::vec3 position,float width,float height)
{
	if (m_explosion_pool.empty())
	{
		m_active_explosions.push_back(std::make_shared<billboard>(*m_explosion_prefab));//Copy from prefab
		auto current = m_active_explosions.back();
		current->activate(position,width,height);
	}
	else {
		auto current = m_explosion_pool.back();
		m_explosion_pool.pop_back();
		current->activate(position, width, height);
		m_active_explosions.push_back(current);
	}
}

void sfx_manager::on_update(const engine::timestep& ts)
{
	jitter_effect.on_update(ts);
	cross_fade_effect.on_update(ts);

	for (auto expl_it = begin(m_active_explosions); expl_it != end(m_active_explosions);) {
		auto& expl = (*expl_it);
		expl->on_update(ts);
		if (expl->is_active())
		{
			++expl_it;
		}
		else {
			//No longer active, so move back into pool
			m_explosion_pool.push_back(expl);
			expl_it = m_active_explosions.erase(expl_it);
		}
	}
}

void sfx_manager::on_render(engine::ref<engine::shader> shader, const engine::perspective_camera& camera)
{
	//Jitter and cross fade rendering is handled elsewhere.

	for (auto& expl:m_active_explosions) {
		expl->on_render(camera,shader);
	}
}

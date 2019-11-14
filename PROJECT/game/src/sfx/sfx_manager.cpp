#include "pch.h"
#include "sfx_manager.h"
#include "../gui/hud_manager.h"

camera_jitter sfx_manager::jitter_effect(nullptr);
cross_fade sfx_manager::cross_fade_effect(nullptr);

void sfx_manager::init(engine::perspective_camera* camera)
{	
	jitter_effect = camera_jitter(camera);

	auto panel = hud_element::create({ .5f,.5f }, { 1,1 }, engine::texture_2d::create("assets/textures/cross_fade.png", true));
	hud_manager::add_element(panel);
	cross_fade_effect = cross_fade(panel);
	panel->set_z_order(0.11f);//Just a little bit ahead of the rest of the HUD so that it appears over all of it.
	panel->hide();
}

void sfx_manager::on_update(const engine::timestep& ts)
{
	jitter_effect.on_update(ts);
	cross_fade_effect.on_update(ts);
}

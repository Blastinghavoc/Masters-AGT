#pragma once

#include "engine.h"
#include "camera_jitter.h"
#include "cross_fade.h"
#include "billboard.h"
#include "beam.h"

/*
Class to manage the SFX.
Only a single instance of a cross_fade and a camera_jitter is needed,
as there is never a reason to have multiple of those effects active at once.

Beams and billboards (for explosions) often require multiple instances,
so this class manages the creation and pooling of these.
*/
class sfx_manager {
public:

	static camera_jitter jitter_effect;
	static cross_fade cross_fade_effect;

	static void init(engine::perspective_camera* camera);

	static void explode_at(glm::vec3 position,float width = 1.f, float height = 1.f);

	static void on_update(const engine::timestep& ts);
	static void on_render_textured(engine::ref<engine::shader> shader,const engine::perspective_camera& camera);
	static void on_render_material(engine::ref<engine::shader> shader);

	static void make_beam(glm::vec3 start, glm::vec3 end, glm::vec3 rgb, float duration, float width, int nesting = 2);

private:
	static std::vector<engine::ref<billboard>> m_active_explosions;
	static std::vector <engine::ref<billboard>> m_explosion_pool;
	static engine::ref<billboard> m_explosion_prefab;

	static std::vector<engine::ref<beam>> m_active_beams;
	static std::vector <engine::ref<beam>> m_beam_pool;
	
};

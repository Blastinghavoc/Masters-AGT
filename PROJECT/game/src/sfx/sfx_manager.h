#pragma once

#include "engine.h"
#include "camera_jitter.h"
#include "cross_fade.h"
#include "billboard.h"

class sfx_manager {
public:

	static camera_jitter jitter_effect;
	static cross_fade cross_fade_effect;

	static void init(engine::perspective_camera* camera);

	static void explode_at(glm::vec3 position,float width = 1.f, float height = 1.f);

	static void on_update(const engine::timestep& ts);
	static void on_render(engine::ref<engine::shader> shader,const engine::perspective_camera& camera);

private:
	static std::vector<engine::ref<billboard>> m_active_explosions;
	static std::vector < engine::ref<billboard>> m_explosion_pool;
	static engine::ref<billboard> m_explosion_prefab;
	
};

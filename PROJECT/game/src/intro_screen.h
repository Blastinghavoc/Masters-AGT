#pragma once
#include <engine.h>

class intro_screen {
	static bool s_active;
	static glm::vec3 center;
public:
	static bool active();
	static void active(bool b);

	static void render(engine::ref<engine::text_manager>& text_manager);

	static void update(engine::perspective_camera& cam, const engine::timestep& ts);

	static void init(engine::perspective_camera& cam,glm::vec3& center_point);
};

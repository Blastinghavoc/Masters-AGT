#pragma once

#include "engine.h"
#include "../sfx/camera_jitter.h"

class sfx_manager {
public:

	static camera_jitter jitter_effect;

	static void init(engine::perspective_camera* camera);

	static void on_update(const engine::timestep& ts);

private:


};

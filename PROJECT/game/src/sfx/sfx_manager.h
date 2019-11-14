#pragma once

#include "engine.h"
#include "camera_jitter.h"
#include "cross_fade.h"

class sfx_manager {
public:

	static camera_jitter jitter_effect;
	static cross_fade cross_fade_effect;

	static void init(engine::perspective_camera* camera);

	static void on_update(const engine::timestep& ts);

private:


};

#include "pch.h"
#include "sfx_manager.h"

camera_jitter sfx_manager::jitter_effect(nullptr);

void sfx_manager::init(engine::perspective_camera* camera)
{	
	jitter_effect = camera_jitter(camera);	
}

void sfx_manager::on_update(const engine::timestep& ts)
{
	jitter_effect.on_update(ts);
}

#pragma once

#include "engine.h"

/*Static class for lighting
See also modifications to engine/renderer/light.cpp
*/
class light_manager {
public:

	static void init(engine::DirectionalLight* sun);

	static void on_update(const engine::timestep& ts);
	static void submit(engine::ref<engine::shader> shader);

	static float day_duration;
	static std::vector<engine::ref<engine::PointLight>> point_lights;
	static std::vector<engine::ref<engine::SpotLight>> spot_lights;

	static void set_point_light_colour(const glm::vec3& colour);

	static const glm::vec3 default_light_colour;
private:
	static engine::DirectionalLight* m_sun_ptr;
	static float m_sun_angle;
	static bool m_sun_underground;

};

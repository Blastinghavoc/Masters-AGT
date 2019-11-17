#pragma once

#include "engine.h"
#include <random>

/*
Creates camera jitter by adding a uniform random vector to the front vector of the camera.

A more advanced random number generator than the typical "rand()" is used to guarantee
a uniform jitter. To be honest, the difference is likely unnoticeable, but I just wanted
to use a "proper" random number generator for once.
*/
class camera_jitter {
public:
	camera_jitter(engine::perspective_camera* camera) :m_camera{camera} {
		std::random_device rd;
		m_generator = std::mt19937(rd());//Seed the random number generator.
	};
	~camera_jitter() {};

	void on_update(const engine::timestep& ts);

	void activate(float intensity, float duration);

	bool is_active() { return m_active; };

private:
	engine::perspective_camera* m_camera;
	bool m_active = false;
	float m_remaining = 0;
	float m_intensity = 0;
	glm::vec3 m_previous{0,0,0};

	
	std::mt19937 m_generator;	
};

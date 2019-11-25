#pragma once
#include "../entities/turret.h"
#include "engine.h"
#include <tuple>

//Static class to manage turrets.
class turret_manager {
	/*
	Using this as glm::vec3 does not implement functionality required for map.
	I could have extended glm::vec3 to implement the comparisons I wanted, but since I only
	need it in one place a local solution was sufficient.
	*/
	typedef std::tuple<float, float, float> vec3;

	static glm::vec3 to_glm(vec3 v) { return glm::vec3(std::get<0>(v), std::get<1>(v), std::get<2>(v)); };
	static vec3 from_glm(glm::vec3 v) { return vec3(v.x, v.y, v.z); };
public:

	static void init();

	static void render(engine::ref<engine::shader> shader);

	static bool place_turret(glm::vec3 position);

	static bool remove_turret(glm::vec3 position);

	static void update(const engine::timestep& ts);

	static int count() { return (int)m_turrets.size(); };
private:
	//Map of world positions to turret references.
	static std::map<vec3,engine::ref<turret>> m_turrets;
	static std::vector<engine::ref<turret>> m_buffer;
};

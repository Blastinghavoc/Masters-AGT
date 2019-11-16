#pragma once
#include <engine.h>

/*Class representing a defensive turret*/
class turret {
public:
	turret(glm::vec3 position = {0,0,0});

	~turret();

	void render(const engine::ref<engine::shader>& shader);

	void face(glm::vec3 target);

	static engine::ref<turret> create(glm::vec3 position = { 0,0,0 }) {
		return std::make_shared<turret>(position);
	};

	void set_position(glm::vec3 position);

private:
	glm::vec3 m_barrel_position;

	//Component objects
	engine::ref<engine::game_object> m_base;
	engine::ref<engine::game_object> m_swivel;
	engine::ref<engine::game_object> m_barrel;

	glm::mat4 m_barrel_transform{};

	const float m_base_height = .3f;
	const float m_swivel_height = .7f;

};

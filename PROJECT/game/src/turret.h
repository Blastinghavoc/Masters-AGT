#pragma once
#include <engine.h>

/*Class representing a defensive turret*/
class turret {
public:
	turret(glm::vec3 position = {0,0,0});

	~turret();

	void render(const engine::ref<engine::shader>& shader);

	void face(glm::vec3 target);

private:
	glm::vec3 m_barrel_position;

	//Component objects
	engine::ref<engine::game_object> m_base;
	engine::ref<engine::game_object> m_swivel;
	engine::ref<engine::game_object> m_barrel;

};

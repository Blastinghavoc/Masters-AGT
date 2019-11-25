#pragma once
#include <engine.h>

/*
Class representing a defensive turret.
Does NOT extend abstract_actor, because it doesn't have a
single game_object (it has three). The abstract base class
could probably be adjusted to be even more general, but no benefit
is gained from that in this case.
*/
class turret {
public:
	turret(glm::vec3 position = {0,0,0});

	~turret();

	void render(const engine::ref<engine::shader>& shader);

	void face(glm::vec3 target);

	static engine::ref<turret> create(glm::vec3 position = { 0,0,0 }) {
		return std::make_shared<turret>(position);
	};

	void update(const engine::timestep& ts);

	float fire();

	void set_position(glm::vec3 position);

	const float& range_squared() const { return m_range_squared; };

	bool is_in_range(glm::vec3 target);

	const glm::vec3& barrel_position() { return m_barrel_position; };

private:
	glm::vec3 m_barrel_position;

	//Component objects
	engine::ref<engine::game_object> m_base;
	engine::ref<engine::game_object> m_swivel;
	engine::ref<engine::game_object> m_barrel;

	glm::mat4 m_barrel_transform{};

	const float m_base_height = .3f;
	const float m_swivel_height = .7f;

	const float m_range_squared = 49.f;//Range of 7 meters (3.5 blocks)

	bool m_can_fire{ true };
	float m_cooldown_remaining{ 0.f };
	const float m_cooldown_time = 1.f;

	const float m_damage = 20.f;

};

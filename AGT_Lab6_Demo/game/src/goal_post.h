#pragma once
#include <engine.h>

class football;

class goal_post
{
public:
	goal_post();
	~goal_post();
	//bool is_goal_scored(football& football);
	void initialise(float height, float width, float rotation, float x_pos, float z_pos, float thickness);
	bool goal_scored(const football& ball);
	void on_render(const engine::ref<engine::shader>& shader);

private:
	engine::ref<engine::cuboid> m_horizontal_bar;
	engine::ref<engine::cuboid> m_vertical_bar;

	engine::ref<engine::material> m_material;

	float m_height, m_width, m_rotation;
	glm::vec3 m_position;

	glm::mat4 left_transform, right_transform, top_transform;

	std::vector<glm::vec3> m_vertices;

	bool check_collision(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 r0, glm::vec3 r1, glm::vec3& intersection, double& t);
};

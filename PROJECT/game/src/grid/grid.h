#pragma once
#include <engine.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "../orientation.h"
#include "grid_tile.h"

//Two dimensional grid of game objects
class grid {
public:	

	grid(float cell_size = 1.f,float y= .5f);
	~grid();

	//Assumes begin scene has already been called
	void render(const engine::ref<engine::shader>& shader);

	void set_border(const int& x, const int& z, const orientation& relative_heading);
	void set_gateway(const int& x, const int& z, const orientation& relative_heading, const float& rotation_angle = 0.f);
	void set_corner(const int& x, const int& z, const orientation& relative_heading);
	void set_floor(const int& x, const int& z);

	void del_border(const int& x, const int& z, const orientation& relative_heading);

	grid_tile& operator[](const glm::vec2& vec);
	grid_tile& operator[](const std::pair<int,int>& loc);

	glm::vec3 grid_to_world_coords(int x, int z);
	std::pair<int,int> world_to_grid_coords(glm::vec3 vec);

	const engine::ref<engine::game_object>& get_wall(const orientation& facing) const;
private:
	//functions

	std::pair<int, int> get_corner_index_from_relative_heading(const int& x, const int& z, const orientation& heading);

	//data

	float m_cell_size;
	float m_y;

	std::map<std::pair<int,int>, grid_tile> m_tiles{};

	std::vector<engine::ref<engine::game_object>> m_walls_prefabs{4};
	std::vector < engine::ref<engine::game_object>> m_corner_prefabs{4};
	engine::ref<engine::game_object> m_floor_prefab{};
	engine::ref<engine::game_object> m_gateway_prefab{};
};

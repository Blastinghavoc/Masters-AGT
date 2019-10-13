#pragma once
#include <engine.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "../../game/orientation.h"
#include "grid_tile.h"

//Two dimensional grid of game objects
class grid {
public:	

	grid(float cell_size = 1.f,float y= .5f);
	~grid();

	//Assumes begin scene has already been called
	void render(const engine::ref<engine::shader>& shader);

	void set_border(const int& x, const int& z, const orientation& facing);

	void set_floor(const int& x, const int& z);

	grid_tile& operator[](const glm::vec2& vec);
	grid_tile& operator[](const std::pair<int,int>& loc);

	const engine::ref<engine::game_object>& get_wall(const orientation& facing) const;
private:
	float m_cell_size;
	float m_y;

	std::map<std::pair<int,int>, grid_tile> m_tiles{};

	std::vector<engine::ref<engine::game_object>> m_walls_prefabs{4};
	engine::ref<engine::game_object> m_corner_prefab{};
	engine::ref<engine::game_object> m_floor_prefab{};
};

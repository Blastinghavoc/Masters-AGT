#pragma once
#include <engine.h>
#include "../orientation.h"

//Represents a 1m x 1m tile on the world grid
class grid_tile {
public:
	grid_tile();
	~grid_tile();	

	void set_border(const engine::ref<engine::game_object>& obj, const orientation& facing);

	void set_corner(const engine::ref<engine::game_object>& obj);

	void set_floor(const engine::ref<engine::game_object>& obj);

	//Assumes begin scene has already been called
	void render(const engine::ref<engine::shader>& shader);

private:
	std::vector<engine::ref<engine::game_object>> m_borders{ 4 };

	//The grid tile owns a single corner, the one with the lowest x and z coordinates.
	engine::ref<engine::game_object> m_corner{};

	engine::ref<engine::game_object> m_floor{};
	engine::ref<engine::game_object> m_ceiling{};

};


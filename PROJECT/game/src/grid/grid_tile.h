#pragma once
#include <engine.h>


//Represents a 1m x 1m tile on the world grid
class grid_tile {
public:
	grid_tile();
	~grid_tile();

	enum cardinal_orientation
	{
		north = 0,
		east = 1,
		south = 2,
		west = 3
	};

	//Assumes begin scene already been called
	void render(const engine::ref<engine::shader>& shader);

private:
	std::vector<engine::ref<engine::game_object>> m_borders{ 4 };
	std::vector<engine::ref<engine::game_object>> m_corners{ 4 };

	engine::ref<engine::game_object> m_floor{};
	engine::ref<engine::game_object> m_ceiling{};

};


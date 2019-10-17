#pragma once
#include <engine.h>
#include "../orientation.h"

//Represents a 1m x 1m tile on the world grid
class grid_tile {
public:
	grid_tile();
	~grid_tile();

	enum class tile_type {
		maze,
		empty
	};

	tile_type type{tile_type::empty};

	void set_border(const engine::ref<engine::game_object>& obj, const orientation& facing);
	void set_corner(const engine::ref<engine::game_object>& obj);
	void set_floor(const engine::ref<engine::game_object>& obj);
	void set_ceiling(const engine::ref<engine::game_object>& obj);

	void del_border(const orientation& facing);
	void del_corner();
	

	bool has_border(const orientation& facing) const { return (m_borders[facing]) ? true : false; };
	bool has_corner() const { return (m_corner) ? true : false; };

	//Assumes begin scene has already been called
	void render(const engine::ref<engine::shader>& shader);

private:
	std::vector<engine::ref<engine::game_object>> m_borders{ 4 };

	//The grid tile owns a single corner, the one with the lowest x and z coordinates.
	engine::ref<engine::game_object> m_corner{};

	engine::ref<engine::game_object> m_floor{};
	engine::ref<engine::game_object> m_ceiling{};

};


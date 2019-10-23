#pragma once
#include <engine.h>
#include "../orientation.h"

//Represents a square tile on the world grid
class grid_tile {
public:
	grid_tile();
	~grid_tile();

	//The "type" of tile
	enum class tile_type {
		maze, //Indicates tile is part of the player-built maze
		empty //Indicates tile can be pathed through in some way
		//TODO Tower type, Start and End types? Consider subclassing instead?
	};

	tile_type type{tile_type::empty};

	//Set the borders/corners etc to the given game object
	void set_border(const engine::ref<engine::game_object>& obj, const orientation& facing);
	void set_corner(const engine::ref<engine::game_object>& obj);
	void set_floor(const engine::ref<engine::game_object>& obj);
	void set_ceiling(const engine::ref<engine::game_object>& obj);

	//Deletion of borders/corners etc
	void del_border(const orientation& facing);
	void del_corner();	

	bool has_border(const orientation& facing) const { return (m_borders.count(facing) > 0); };
	bool has_corner() const { return (m_corner) ? true : false; };

	//Assumes begin scene has already been called
	void render(const engine::ref<engine::shader>& shader);

private:
	std::map<orientation,engine::ref<engine::game_object>> m_borders{};

	//The grid tile owns a single corner, the one with the lowest x and z coordinates.
	engine::ref<engine::game_object> m_corner{};

	engine::ref<engine::game_object> m_floor{};
	engine::ref<engine::game_object> m_ceiling{};

};


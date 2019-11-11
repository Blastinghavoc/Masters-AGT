#pragma once
#include <engine.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "../orientation.h"
#include "grid_tile.h"

/*
Two dimensional grid of game objects, each grid cell is managed by a grid_tile object.

*/
class grid {
public:	

	grid(float cell_size = 1.f,float y= .5f);
	~grid();

	//Assumes begin scene has already been called
	void render(const engine::ref<engine::shader>& shader);

	//Methods to place a particular game object at a given grid location and orientation (if applicable)
	void set_border(const int& x, const int& z, const orientation& relative_heading);
	void set_gateway(const int& x, const int& z, const orientation& relative_heading, const float& rotation_angle = 0.f);
	void set_corner(const int& x, const int& z, const orientation& relative_heading = orientation::south_east);
	void set_floor(const int& x, const int& z);
	void set_ceiling(const int& x, const int& z);

	void set_state(const int& x, const int& z, grid_tile::tile_state state,bool force= false);
	void set_start(const int& x, const int& z) {
		set_state(x, z, grid_tile::tile_state::start, true);
		m_start_index = std::pair(x, z);
	};
	void set_end(const int& x, const int& z) {
		set_state(x, z, grid_tile::tile_state::end, true);
		m_end_index = std::pair(x, z);
	};
	std::pair<int,int> get_start() const { return m_start_index; };
	std::pair<int, int> get_end() const { return m_end_index; };
	bool is_walkable(std::pair<int, int> index) const {
		if (contains(index))
		{
			auto tile_state = m_tiles.at(index).state;
			return (tile_state == grid_tile::tile_state::empty) || (tile_state == grid_tile::tile_state::start) ||
				(tile_state == grid_tile::tile_state::end);
		}
		return false;		
	};

	//Methods to place and remove whole "blocks" of the grid. A block has 4 walls, 4 corners and a ceiling.
	//returns indicate success or failure of operation
	bool place_block(const int& x, const int& z, bool force = false);
	bool remove_block(const int& x, const int& z);

	//The opposite of the set_ methods. Removes a particular grid element.
	void del_border(const int& x, const int& z, const orientation& relative_heading);
	void del_corner(const int& x, const int& z, const orientation& relative_heading = orientation::south_east);

	//Saves a copy of the current state of all tiles that can be restored later. Used to define the "default" state of tiles.
	void bake_tiles();

	//helper method to determine whether a given index is present in the grid.
	bool contains(std::pair<int, int> index) const { return m_tiles.count(index) > 0; }
	bool contains(const int& x,const int& z) const { return m_tiles.count(std::pair(x,z)) > 0; }

	//Allows addressing single tiles in the grid
	grid_tile& operator[](const glm::vec2& vec);
	grid_tile& operator[](const std::pair<int,int>& loc);

	//Conversions between grid indices and world coordinates.
	glm::vec3 grid_to_world_coords(int x, int z) const;
	std::pair<int,int> world_to_grid_coords(glm::vec3 vec) const;

	//Simple getters
	const float& cell_size() const { return m_cell_size; };
	const float& floor_level() const { return m_y; };

private:
	//utility functions

	//returns the index adjacent to the given index in the given direction. E.g, adjacent_to({1,0},north) = (1,1)
	std::pair<int, int> adjacent_to(const std::pair<int, int>& index, const orientation& direction);
	//Checks whether given grid location has a corner in the given heading.
	bool has_corner(const int& x, const int& z, const orientation& relative_heading = orientation::south_east);
	//Obtains the grid index of the corner found at the given heading from the given grid coordinates.
	std::pair<int, int> get_corner_index_from_heading(const int& x, const int& z, const orientation& heading);

	//data

	float m_cell_size;
	float m_y;

	//The tiles owned by this grid object, indexed by an x-z pair
	std::map<std::pair<int,int>, grid_tile> m_tiles{};

	//A map of tiles that can be restored later.
	std::map<std::pair<int, int>, grid_tile> m_tiles_baked{};

	//"Prefab" objects to instantiate grid pieces from.
	std::map<orientation,engine::ref<engine::game_object>> m_walls_prefabs{};
	engine::ref<engine::game_object> m_corner_prefab{};
	engine::ref<engine::game_object> m_floor_prefab{};
	engine::ref<engine::game_object> m_ceiling_prefab{};
	engine::ref<engine::game_object> m_gateway_prefab{};

	//Indices of start and end of the maze defined by the grid.
	std::pair<int, int> m_start_index{};
	std::pair<int, int> m_end_index{};
};

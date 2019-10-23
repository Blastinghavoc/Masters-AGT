#include "grid.h"

grid::grid(float cell_size,float y):
	m_cell_size{cell_size},
	m_y{y}
{
	std::string path = "assets/models/static/dungeon/";
	std::string extn = ".obj";

	//Set up prefabs

	if (m_walls_prefabs.count(orientation::east) < 1)
	{
		m_walls_prefabs[orientation::east];
	}

	if (m_walls_prefabs.count(orientation::east) < 1)
	{
		throw std::exception();
	}

	//Models in the "modified" directory were put together by me from the original pieces.
	//Walls
	engine::ref <engine::model> model = engine::model::create(path + "modified/wall_straight" + extn);
	engine::game_object_properties props;
	props.meshes = model->meshes();
	props.textures = model->textures();
	float scale = (m_cell_size * 1.f) / glm::max(model->size().x, glm::max(model->size().y, model->size().z));
	props.scale = glm::vec3(scale);
	props.bounding_shape = model->size() / 2.f * scale;
	props.rotation_axis = { 0,1, 0 };
	props.position = { 0,0,m_cell_size * .5f };
	props.rotation_amount = (float)(M_PI / 2);
	m_walls_prefabs[orientation::east]=engine::game_object::create(props);
	props.position = { m_cell_size ,0, m_cell_size * .5f };
	props.rotation_amount = (float)(M_PI *1.5);
	m_walls_prefabs[orientation::west] = engine::game_object::create(props);
	props.position = { m_cell_size * .5f,0, 0 };
	props.rotation_amount = 0;
	m_walls_prefabs[orientation::south] = engine::game_object::create(props);
	props.position = { m_cell_size * .5f,0, m_cell_size };
	props.rotation_amount = (float)M_PI;
	m_walls_prefabs[orientation::north] = engine::game_object::create(props);

	//floor
	model = engine::model::create(path + "Floor_Square" + extn);
	props = engine::game_object_properties();
	props.meshes = model->meshes();
	props.textures = model->textures();
	scale = (m_cell_size * 1.f) / glm::max(model->size().x, glm::max(model->size().y, model->size().z));
	props.scale = glm::vec3(scale);
	props.bounding_shape = model->size() / 2.f * scale;
	props.rotation_axis = { 0,1, 0 };
	props.position = { m_cell_size * .5f,0,m_cell_size * .5f };
	m_floor_prefab = engine::game_object::create(props);
	//ceiling
	props.position += glm::vec3{0, m_cell_size * .8, 0};
	m_ceiling_prefab = engine::game_object::create(props);

	//pillar/corner
	model = engine::model::create(path + "modified/pillar" + extn);
	props = engine::game_object_properties();
	props.meshes = model->meshes();
	props.textures = model->textures();
	scale = (m_cell_size * 1.f) / glm::max(model->size().x, glm::max(model->size().y, model->size().z));
	props.scale = glm::vec3(scale);
	props.bounding_shape = model->size() / 2.f * scale;
	props.rotation_axis = { 0,1, 0 };	
	props.position = { m_cell_size * 0,0,m_cell_size * 0 };
	m_corner_prefab = engine::game_object::create(props);	

	//Gateway
	model = engine::model::create(path + "modified/gateway_large" + extn);
	props = engine::game_object_properties();
	props.meshes = model->meshes();
	props.textures = model->textures();	
	scale = (m_cell_size * 1.f) / model->size().x;
	auto max_scale = (m_cell_size * 1.f) / glm::max(model->size().x, glm::max(model->size().y, model->size().z));
	props.scale = glm::vec3(scale);
	props.bounding_shape = model->size() / 2.f * max_scale;
	props.rotation_axis = { 0,1, 0 };
	props.position = { 0,0,0 };
	m_gateway_prefab = engine::game_object::create(props);
}

grid::~grid()
{
}

//Renders all tiles owned by this grid, assuming begin_scene has already been called.
void grid::render(const engine::ref<engine::shader>& shader)
{
	for (auto pair : m_tiles)
	{
		pair.second.render(shader);
	}
}

//Set the border wall at a particular position and orientation. Care should be taken to ensure these do not overlap.
void grid::set_border(const int& x, const int& z, const orientation& relative_heading)
{
	//If facing is not a cardinal direction, exception
	if (!relative_heading.is_cardinal())
	{
		throw std::exception();
	}

	//copy from the prefabs
	engine::ref<engine::game_object> wall_obj = std::make_shared<engine::game_object>(*m_walls_prefabs[relative_heading]);
	wall_obj->set_position(wall_obj->position() + grid_to_world_coords(x, z));
	m_tiles[{x, z}].set_border(wall_obj, relative_heading);
}

//Sets a gateway piece (half) at the given position, orientation and angle. Angle is required because the gateway piece is not rotationally symmetrical.
void grid::set_gateway(const int& x, const int& z, const orientation& relative_heading,const float& rotation_angle) {
	engine::ref<engine::game_object> obj = std::make_shared < engine::game_object>(*m_gateway_prefab);

	//Determine which tile will own the gateway.
	std::pair<int, int> index = get_corner_index_from_heading(x, z, relative_heading);

	obj->set_position(obj->position() + grid_to_world_coords(index.first, index.second));
	//Rotate the gateway.
	obj->set_rotation_amount(rotation_angle);

	m_tiles[index].set_corner(obj);
}

//Sets a corner piece at the given grid square and facing.
void grid::set_corner(const int& x, const int& z, const orientation& relative_heading)
{	
	/*NOTE: any corner can be expressed as the south_east corner of some tile, since the corner models are rotationally
	symmetrical. This prevents two tiles trying to render models for the same corner.
	*/
	
	engine::ref<engine::game_object> corner_obj = std::make_shared < engine::game_object>(*m_corner_prefab);

	//Obtain the grid index that will own the corner.
	std::pair<int, int> index = get_corner_index_from_heading(x, z, relative_heading);

	corner_obj->set_position(corner_obj->position() + grid_to_world_coords(index.first, index.second));
	m_tiles[index].set_corner(corner_obj);
}

//Returns true if a corner exists at the given location and orientation (a gateway counts as a corner)
bool grid::has_corner(const int& x, const int& z, const orientation& relative_heading)
{
	std::pair<int, int> index = get_corner_index_from_heading(x, z, relative_heading);
	return m_tiles[index].has_corner();
}

/*Determine the index of the grid_tile that should own the south_east corner such that it is also the
"relative_heading" corner of the given index. E.g, the north_east corner of (1,2) is actually the south_east
corner of (1,3).
*/
std::pair<int, int> grid::get_corner_index_from_heading(const int& x, const int& z,const orientation& relative_heading) {
	//If facing is not a corner direction, exception
	if (relative_heading.is_cardinal())
	{
		throw std::exception();
	}

	int effective_x = x, effective_z = z;
	if (relative_heading == orientation::north_east)
	{
		effective_z += 1;
	}
	if (relative_heading == orientation::south_west)
	{
		effective_x += 1;
	}
	if (relative_heading == orientation::north_west)
	{
		effective_x += 1;
		effective_z += 1;
	}
	
	return std::make_pair(effective_x, effective_z);
}

//Sets the floor at the given grid location.
void grid::set_floor(const int& x, const int& z)
{
	//copy from the prefabs
	engine::ref < engine::game_object> obj = std::make_shared<engine::game_object>(*m_floor_prefab);
	obj->set_position(obj->position() + grid_to_world_coords(x, z));
	m_tiles[{x, z}].set_floor(obj);
}

//Places a ceiling at the target location
void grid::set_ceiling(const int& x, const int& z)
{
	engine::ref < engine::game_object> obj = std::make_shared<engine::game_object>(*m_ceiling_prefab);
	obj->set_position(obj->position() + grid_to_world_coords(x, z));
	m_tiles[{x, z}].set_ceiling(obj);
}

/*Place a whole "block" at the target location.
This has four walls, a ceiling and four corners.
Care is taken not place additional walls if they already exist (perhaps owned by adjacent tiles).
Adjacent "blocks" should have no walls between them, to give the illusion that they merge together.
Blocks are also know as "maze" tiles, because they are used to construct a maze for enemies.
*/
void grid::place_block(const int& x, const int& z)
{
	set_ceiling(x, z);
	std::vector<orientation> wall_facings= orientation::get_all_cardinal();

	auto index = std::make_pair(x, z);
	auto& tile = m_tiles[index];

	tile.type = grid_tile::tile_type::maze;

	std::map<orientation,bool> adjacent_is_maze;

	//Must be careful about placing walls so as not to have two adjacent tiles both rendering walls.
	for (const auto& facing : wall_facings)
	{
		auto offset = facing.to_vec();
		std::pair<int, int> adjacent_index{ index.first + (int)offset.x,index.second + (int)offset.z };
		orientation opposite_facing = facing.invert();
		auto& adjacent_tile = m_tiles[adjacent_index];
		adjacent_is_maze[facing] = adjacent_tile.type == grid_tile::tile_type::maze;

		//Nothing required if this tile already has a border in this direction.
		if (!(tile.has_border(facing)))
		{
			bool need_to_place_border = true;

			if (adjacent_is_maze[facing])
			{
				//Delete the adjacent border (if it exists)
				adjacent_tile.del_border(opposite_facing);
				//Don't need a border, as it won't be visible next to the other solid block.
				need_to_place_border = false;
			}
			else {
				//If the adjacent tile is not another block, then we do not delete its wall and do not place a new one.
				if (adjacent_tile.has_border(opposite_facing))
				{
					need_to_place_border = false;
				}
			}

			if (need_to_place_border)
			{
				set_border(x, z, facing);
			}
		}
	}

	//Set the south_east corner if necessary.
	if (!tile.has_corner())
	{
		set_corner(x, z);
	}	

	//Add the other corners if they don't already exist.
	orientation other_corner_owners[]{ orientation::north,orientation::west,orientation::north_west};
	for (const auto& facing : other_corner_owners)
	{
		auto offset = facing.to_vec();
		std::pair<int, int> other_index{ index.first + (int)offset.x,index.second + (int)offset.z };
		auto& other_tile = m_tiles[other_index];		

		if (!other_tile.has_corner())
		{
			set_corner(other_index.first, other_index.second);
		}

	}

	//Check if diagonally adjacent tiles are maze tiles
	for (const auto & corner_facing : orientation::get_all_composite())
	{
		auto offset = corner_facing.to_vec();
		std::pair<int, int> corner_index{ index.first + (int)offset.x,index.second + (int)offset.z };
		auto& corner_tile = m_tiles[corner_index];
		bool corner_is_maze = corner_tile.type == grid_tile::tile_type::maze;

		//Get the cardinal components of the composite orientation
		auto components = corner_facing.cardinal_components();		
		//Delete corner if shared on all sides by maze blocks.
		if (corner_is_maze && adjacent_is_maze[components[0]] && adjacent_is_maze[components[1]])
		{
			del_corner(x,z, corner_facing);
		}
	}


}

//The opposite of place_block. Removes a whole block, reseting the tile state to the baked state if possible.
void grid::remove_block(const int& x, const int& z)
{
	auto index = std::make_pair(x, z);

	if (m_tiles.count(index) > 0)
	{
		auto wall_facings = orientation::get_all_cardinal();
		auto& old_tile = m_tiles[index];

		old_tile.type = grid_tile::tile_type::empty;

		if (m_tiles_baked.count(index) > 0)
		{
			//Restore saved version if possible.
			m_tiles[index] = m_tiles_baked[index];			
		}
		else {
			//This branch should not really be executed in normal gameplay, because the whole map should have been baked before play begins.
			throw std::exception();

			//What the code would do if this was allowed.
			for each (auto facing in wall_facings)
			{
				old_tile.del_border(facing);				
			}
			del_corner(x, z, orientation::north_east);
			del_corner(x, z, orientation::south_east);
			del_corner(x, z, orientation::south_west);
			del_corner(x, z, orientation::north_west);
		}

		//Remove any extraneous corners.
		//These can occur in the any position except the south-east, as that corner is owned by the block being deleted.
		for each (const auto& corner_facing in std::vector<orientation>{orientation::north_east, orientation::north_west, orientation::south_west})
		{
			auto corner_index = get_corner_index_from_heading(x, z, corner_facing);
			auto& corner_tile = m_tiles[corner_index];
			//Maze type tiles should keep their corners. Any others get them reset if applicable, or removed
			if (corner_tile.type != grid_tile::tile_type::maze && corner_tile.has_corner())
			{
				bool should_remove = true;
				//If the baked version of the tile exists and has a corner, keep it.
				if (m_tiles_baked.count(corner_index) > 0 && m_tiles_baked[corner_index].has_corner())
				{
					should_remove = false;
				}

				//Otherwise, remove the corner.
				if (should_remove)
				{
					corner_tile.del_corner();
				}
			}
		}

		//Similar to placing a tile, must smartly update adjacent walls.
		//Any adjacent maze tiles should have a border in this direction, and a corner.
		//Corners may be missing, not from the previous step, but if the removed tile was in the middle of other maze tiles (and thus had no corners)
		for each (const auto& facing in wall_facings)
		{
			auto offset = facing.to_vec();
			std::pair<int, int> adjacent_index{ index.first + (int)offset.x,index.second + (int)offset.z };
			orientation opposite_facing = facing.invert();
			auto& adjacent_tile = m_tiles[adjacent_index];

			if (adjacent_tile.type == grid_tile::tile_type::maze)
			{
				//Set any missing corners
				for each (auto corner_facing in opposite_facing.composite_components())
				{
					if (!has_corner(adjacent_index.first, adjacent_index.second, corner_facing))
					{
						set_corner(adjacent_index.first, adjacent_index.second, corner_facing);
					}
				}

				//Set any missing borders.
				if (!adjacent_tile.has_border(opposite_facing))
				{
					set_border(adjacent_index.first, adjacent_index.second, opposite_facing);
				}
			}			
		}
	}
}

//Removes the border (wall) associated with a particular grid location and orientation.
void grid::del_border(const int& x, const int& z, const orientation& relative_heading)
{
	//If facing is not a cardinal direction, exception
	if (!relative_heading.is_cardinal())
	{
		throw std::exception();
	}
	
	m_tiles[{x, z}].del_border(relative_heading);
}

//Removes the corner associated with a particular grid location and orientation.
void grid::del_corner(const int& x, const int& z, const orientation& relative_heading)
{
	//If facing is not a composite direction, exception
	if (relative_heading.is_cardinal())
	{
		throw std::exception();
	}

	std::pair<int, int> index = get_corner_index_from_heading(x, z, relative_heading);

	m_tiles[index].del_corner();
}

//Save a copy of all tiles so that they can be restored later.
void grid::bake_tiles()
{
	m_tiles_baked.clear();
	m_tiles_baked.insert(m_tiles.begin(), m_tiles.end());
}

//Allows addressing single tiles by a 2D vector
grid_tile& grid::operator[](const glm::vec2& vec)
{
	std::pair<int, int> index = std::make_pair((int)floor(vec.x), (int)floor(vec.y));
	return m_tiles[index];
}

//Allows addressing single tiles by a pair of ints
grid_tile& grid::operator[](const std::pair<int, int>& loc)
{
	return m_tiles[loc];
}

//Convert grid indices to world coordinates
inline glm::vec3 grid::grid_to_world_coords(int x, int z)
{
	return glm::vec3(m_cell_size*x,m_y,m_cell_size*z);
}

//Convert world coordinates to grid indices
std::pair<int, int> grid::world_to_grid_coords(glm::vec3 vec)
{
	return std::pair<int, int>((int)floor(vec.x/m_cell_size),(int)floor(vec.z/m_cell_size));
}



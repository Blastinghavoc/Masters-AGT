#include "grid.h"

grid::grid(float cell_size,float y):
	m_cell_size{cell_size},
	m_y{y}
{
	std::string path = "assets/models/static/dungeon/";
	std::string extn = ".obj";

	//Set up prefabs

	//Models in the "modified" directory were put together by me from the original pieces.
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

	//pillar
	model = engine::model::create(path + "modified/pillar" + extn);
	props = engine::game_object_properties();
	props.meshes = model->meshes();
	props.textures = model->textures();
	scale = (m_cell_size * 1.f) / glm::max(model->size().x, glm::max(model->size().y, model->size().z));
	props.scale = glm::vec3(scale);
	props.bounding_shape = model->size() / 2.f * scale;
	props.rotation_axis = { 0,1, 0 };
	props.position = { m_cell_size * 0,0,m_cell_size * 1 };
	m_corner_prefabs[orientation::north_east-4] = engine::game_object::create(props);
	props.position = { m_cell_size * 0,0,m_cell_size * 0 };
	m_corner_prefabs[orientation::south_east - 4] = engine::game_object::create(props);
	props.position = { m_cell_size * 1,0,m_cell_size * 0 };
	m_corner_prefabs[orientation::south_west - 4] = engine::game_object::create(props);
	props.position = { m_cell_size * 1,0,m_cell_size * 1 };
	m_corner_prefabs[orientation::north_west - 4] = engine::game_object::create(props);

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

void grid::render(const engine::ref<engine::shader>& shader)
{
	for each (auto pair in m_tiles)
	{
		pair.second.render(shader);
	}
}

//Set the border wall at a particular position and orientation. Care must be taken to ensure these do not overlap.
void grid::set_border(const int& x, const int& z, const orientation& relative_heading)
{
	//If facing is not a cardinal direction, exception
	if (relative_heading > orientation::west)
	{
		throw std::exception();
	}

	//copy from the prefabs
	engine::ref<engine::game_object> wall_obj = std::make_shared<engine::game_object>(*m_walls_prefabs[relative_heading]);
	wall_obj->set_position(wall_obj->position() + grid_to_world_coords(x, z));
	m_tiles[{x, z}].set_border(wall_obj, relative_heading);
}

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

	//TODO remove use of vector of corner prefabs, since I only need one
	
	engine::ref<engine::game_object> corner_obj = std::make_shared < engine::game_object>(*m_corner_prefabs[1]);

	std::pair<int, int> index = get_corner_index_from_heading(x, z, relative_heading);

	corner_obj->set_position(corner_obj->position() + grid_to_world_coords(index.first, index.second));
	m_tiles[index].set_corner(corner_obj);
}

bool grid::has_corner(const int& x, const int& z, const orientation& relative_heading)
{
	std::pair<int, int> index = get_corner_index_from_heading(x, z, relative_heading);
	return m_tiles[index].has_corner();
}

std::pair<int, int> grid::get_corner_index_from_heading(const int& x, const int& z,const orientation& relative_heading) {
	//If facing is not a corner, exception
	if (relative_heading <= orientation::west)
	{
		throw std::exception();
	}

	int effective_x = x, effective_z = z;
	switch (relative_heading)
	{
	case north_east:
		effective_z += 1;
		break;
	case south_west:
		effective_x += 1;
		break;
	case north_west:
		effective_x += 1;
		effective_z += 1;
		break;
	default:
		break;
	}
	return std::make_pair(effective_x, effective_z);
}

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

void grid::place_block(const int& x, const int& z)
{
	set_ceiling(x, z);
	orientation wall_facings[]{north,south,east,west};

	auto index = std::make_pair(x, z);
	auto& tile = m_tiles[index];

	tile.type = grid_tile::tile_type::maze;

	std::vector<bool> adjacent_is_maze(4);

	//Must be careful about placing walls so as not to have two adjacent tiles both rendering walls.
	for each (const auto& facing in wall_facings)
	{
		auto offset = to_vec(facing);
		std::pair<int, int> adjacent_index{ index.first + offset.x,index.second + offset.z };
		orientation opposite_facing = invert(facing);
		auto& adjacent_tile = m_tiles[adjacent_index];
		adjacent_is_maze[facing] = adjacent_tile.type == grid_tile::tile_type::maze;

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

	//Set the SE corner if necessary.
	if (!tile.has_corner())
	{
		set_corner(x, z);
	}	

	//Ensure adjacent tiles don't render extra corners.
	orientation other_corner_owners[]{ north,west,north_west};
	for each (const auto& facing in other_corner_owners)
	{
		auto offset = to_vec(facing);
		std::pair<int, int> other_index{ index.first + offset.x,index.second + offset.z };
		auto& other_tile = m_tiles[other_index];		

		if (!other_tile.has_corner())
		{
			set_corner(other_index.first, other_index.second);
		}

	}

	//Check if diagonally adjacent tiles are maze tiles
	for each (const auto & corner_facing in std::vector<orientation>{north_east, north_west, south_west,south_east})
	{
		auto offset = to_vec(corner_facing);
		std::pair<int, int> corner_index{ index.first + offset.x,index.second + offset.z };
		auto& corner_tile = m_tiles[corner_index];
		bool corner_is_maze = corner_tile.type == grid_tile::tile_type::maze;

		auto components = composite_components(corner_facing);		
		//Delete corner if shared on all sides by maze blocks.
		if (corner_is_maze && adjacent_is_maze[components[0]] && adjacent_is_maze[components[1]])
		{
			del_corner(x,z, corner_facing);
		}
	}


}

void grid::remove_block(const int& x, const int& z)
{
	auto index = std::make_pair(x, z);

	if (m_tiles.count(index) > 0)
	{
		orientation wall_facings[]{ north,south,east,west };
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
			del_corner(x, z, north_east);
			del_corner(x, z, south_east);
			del_corner(x, z, south_west);
			del_corner(x, z, north_west);
		}

		//Remove any extraneous corners.
		//These can occur in the any position except the south-east, as that corner is owned by the block being deleted.
		for each (const auto& corner_facing in std::vector<orientation>{north_east,north_west,south_west})
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
		for each (const auto& facing in wall_facings)
		{
			auto offset = to_vec(facing);
			std::pair<int, int> adjacent_index{ index.first + offset.x,index.second + offset.z };
			orientation opposite_facing = invert(facing);
			auto& adjacent_tile = m_tiles[adjacent_index];

			if (adjacent_tile.type == grid_tile::tile_type::maze)
			{
				//Set any missing corners
				for each (auto corner_facing in cardinal_to_composite(opposite_facing))
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

void grid::del_border(const int& x, const int& z, const orientation& relative_heading)
{
	//If facing is not a cardinal direction, exception
	if (relative_heading > orientation::west)
	{
		throw std::exception();
	}
	
	m_tiles[{x, z}].del_border(relative_heading);
}

void grid::del_corner(const int& x, const int& z, const orientation& relative_heading)
{
	//If facing is not a composite direction, exception
	if (relative_heading <= orientation::west)
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

grid_tile& grid::operator[](const glm::vec2& vec)
{
	std::pair<int, int> index = std::make_pair((int)floor(vec.x), (int)floor(vec.y));
	return m_tiles[index];
}

grid_tile& grid::operator[](const std::pair<int, int>& loc)
{
	return m_tiles[loc];
}

inline glm::vec3 grid::grid_to_world_coords(int x, int z)
{
	return glm::vec3(m_cell_size*x,m_y,m_cell_size*z);
}

std::pair<int, int> grid::world_to_grid_coords(glm::vec3 vec)
{
	return std::pair<int, int>((int)floor(vec.x/m_cell_size),(int)floor(vec.z/m_cell_size));
}



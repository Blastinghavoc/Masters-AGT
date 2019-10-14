#include "grid.h"

grid::grid(float cell_size,float y):
	m_cell_size{cell_size},
	m_y{y}
{
	std::string path = "assets/models/static/dungeon/";
	std::string extn = ".obj";

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

void grid::set_border(const int& x, const int& z, const orientation& facing)
{
	//If facing is not a cardinal direction, exception
	if (facing > orientation::west)
	{
		throw std::exception();
	}

	//copy from the prefabs
	engine::ref<engine::game_object> wall_obj = std::make_shared<engine::game_object>(*m_walls_prefabs[facing]);
	wall_obj->set_position(wall_obj->position() + grid_to_world_coords(x, z));
	m_tiles[{x, z}].set_border(wall_obj, facing);
}

void grid::set_corner(const int& x, const int& z, const orientation& facing)
{
	//If facing is not a corner, exception
	if (facing <= orientation::west)
	{
		throw std::exception();
	}

	/*int index = facing - 4;
	engine::ref<engine::game_object> corner_obj = std::make_shared < engine::game_object>(*m_corner_prefabs[index]);*/

	/*NOTE: since the corner acts like a point, it may not necessarily be "owned" by the same tile as associated walls.
	e.g, tile 1,1 owns corner 1,1 but not corner 1,2 despite them being adjacent. Corners are visually
	 and literally shared by multiple tiles.
	
	Therefore, only using the south east corner, but adjusting which tile it belongs to.
	This is because any corner can be expressed as the south_east corner of some tile, since the corner models are rotationally
	symmetrical. This prevents two tiles trying to render models for the same corner.
	*/

	int effective_x = x, effective_z =z;
	engine::ref<engine::game_object> corner_obj = std::make_shared < engine::game_object>(*m_corner_prefabs[1]);	
	switch (facing)
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

	corner_obj->set_position(corner_obj->position() + grid_to_world_coords(effective_x, effective_z));
	m_tiles[{effective_x, effective_z}].set_corner(corner_obj, 1);
}

void grid::set_floor(const int& x, const int& z)
{
	//copy from the prefabs
	engine::ref < engine::game_object> obj = std::make_shared<engine::game_object>(*m_floor_prefab);
	obj->set_position(obj->position() + grid_to_world_coords(x, z));
	m_tiles[{x, z}].set_floor(obj);
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

const engine::ref<engine::game_object>& grid::get_wall(const orientation& facing) const
{
	return m_walls_prefabs[facing];
}


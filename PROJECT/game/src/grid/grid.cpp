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

	//Testing
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
	std::pair<int, int> index = get_corner_index_from_relative_heading(x, z, relative_heading);

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
	
	engine::ref<engine::game_object> corner_obj = std::make_shared < engine::game_object>(*m_corner_prefabs[1]);

	std::pair<int, int> index = get_corner_index_from_relative_heading(x, z, relative_heading);

	corner_obj->set_position(corner_obj->position() + grid_to_world_coords(index.first, index.second));
	m_tiles[index].set_corner(corner_obj);
}

std::pair<int, int> grid::get_corner_index_from_relative_heading(const int& x, const int& z,const orientation& relative_heading) {
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

void grid::del_border(const int& x, const int& z, const orientation& relative_heading)
{
	//If facing is not a cardinal direction, exception
	if (relative_heading > orientation::west)
	{
		throw std::exception();
	}
	
	m_tiles[{x, z}].del_border(relative_heading);
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


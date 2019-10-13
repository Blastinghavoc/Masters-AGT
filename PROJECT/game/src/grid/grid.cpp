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
	props.position = { 0,m_y,m_cell_size * .5f };
	props.rotation_amount = (float)(M_PI / 2);
	m_walls_prefabs[orientation::east]=engine::game_object::create(props);
	props.position = { m_cell_size ,m_y, m_cell_size * .5f };
	props.rotation_amount = (float)(M_PI *1.5);
	m_walls_prefabs[orientation::west] = engine::game_object::create(props);
	props.position = { m_cell_size * .5f,m_y, 0 };
	props.rotation_amount = 0;
	m_walls_prefabs[orientation::south] = engine::game_object::create(props);
	props.position = { m_cell_size * .5f,m_y, m_cell_size };
	props.rotation_amount = (float)M_PI;
	m_walls_prefabs[orientation::north] = engine::game_object::create(props);

	model = engine::model::create(path + "Floor_Square" + extn);
	props = engine::game_object_properties();
	props.meshes = model->meshes();
	props.textures = model->textures();
	scale = (m_cell_size * 1.f) / glm::max(model->size().x, glm::max(model->size().y, model->size().z));
	props.scale = glm::vec3(scale);
	props.bounding_shape = model->size() / 2.f * scale;
	props.rotation_axis = { 0,1, 0 };
	props.position = { m_cell_size * .5f,m_y,m_cell_size * .5f };
	m_floor_prefab = engine::game_object::create(props);
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
	//copy from the prefabs
	engine::ref < engine::game_object> wall_obj = std::make_shared<engine::game_object>(*m_walls_prefabs[facing]);
	wall_obj->set_position(wall_obj->position() + glm::vec3{x*m_cell_size,0,z*m_cell_size});
	m_tiles[{x, z}].set_border(wall_obj, facing);
}

void grid::set_floor(const int& x, const int& z)
{
	//copy from the prefabs
	engine::ref < engine::game_object> obj = std::make_shared<engine::game_object>(*m_floor_prefab);
	obj->set_position(obj->position() + glm::vec3{ x * m_cell_size,0,z * m_cell_size });
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

glm::vec3 grid::grid_to_world_coords(int x, int z)
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


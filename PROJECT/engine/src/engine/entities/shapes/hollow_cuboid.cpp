#include "pch.h"
#include "hollow_cuboid.h"
#define _USE_MATH_DEFINES
#include <math.h>

engine::hollow_cuboid::hollow_cuboid(glm::vec3 half_extents, float border_width, float tex_size)
{
	std::vector<engine::mesh::vertex> vertices;
	std::vector<uint32_t> indices;
	uint32_t index = 0;

	auto x = half_extents.x;
	auto y = half_extents.y;
	auto z = half_extents.z;

	auto x_prime = x - border_width;
	auto y_prime = y - border_width;
	auto z_prime = z - border_width;

	auto tex_x = (2 * half_extents.x) / tex_size;
	auto tex_y = (2 * half_extents.y) / tex_size;
	auto tex_z = (2 * half_extents.z) / tex_size;


	//Working variables for the current vertices being processed
	engine::mesh::vertex bl{};//bottom left
	engine::mesh::vertex br{};//bottom right
	engine::mesh::vertex tr{};//top right
	engine::mesh::vertex tl{};//top left
	std::vector<engine::mesh::vertex> tmp_vertices{};
	glm::vec3 rotation_axis{};
	float angle = 0;

	//face bottom-left...face inner bottom-left...face inner-inner bottom-left
	glm::vec3 fbl, fbr, ftr, ftl, fibl, fibr, fitr, fitl, fiibl, fiibr, fiitr, fiitl;
	//Face norm, face left norm.. face up norm, face down norm;
	glm::vec3 fnorm, flnorm, frnorm, funorm, fdnorm;

	//Texture coords for width, height and border for current face 
	float ftexw, ftexh, ftexb;
	ftexb = border_width / tex_size;//Border width constant for all faces

	auto make_axis_sides = [&]() {
		//Top strip
		bl = { fitl, fnorm, { ftexh-ftexb,ftexb } };
		br = { fitr, fnorm, { ftexh - ftexb,ftexw - ftexb } };
		tr = { ftr, fnorm, {ftexh,ftexw } };
		tl = { ftl, fnorm, { ftexh,0 } };
		tmp_vertices = { bl,br,tr,tl };
		shape_utils::add_quads(2, angle, tmp_vertices, vertices, indices, index, rotation_axis);

		//Bottom strip
		bl = { fbl, fnorm, { 0,0 } };
		br = { fbr, fnorm, { 0,ftexw } };
		tr = { fibr, fnorm, {ftexb,ftexw - ftexb } };
		tl = { fibl, fnorm, { ftexb,ftexb } };
		tmp_vertices = { bl,br,tr,tl };
		shape_utils::add_quads(2, angle, tmp_vertices, vertices, indices, index, rotation_axis);

		//Left strip
		bl = { fbl, fnorm, { 0,0 } };
		br = { fibl, fnorm, { ftexb,ftexb } };
		tr = { fitl, fnorm, { ftexh-ftexb,ftexb } };
		tl = { ftl, fnorm, { ftexh,0 } };
		tmp_vertices = { bl,br,tr,tl };
		shape_utils::add_quads(2, angle, tmp_vertices, vertices, indices, index, rotation_axis);

		//Right strip
		bl = { fibr, fnorm, { ftexb,ftexw-ftexb } };
		br = { fbr, fnorm, { 0,ftexw } };
		tr = { ftr, fnorm, { ftexh,ftexw } };
		tl = { fitr, fnorm, { ftexh-ftexb,ftexw-ftexb } };
		tmp_vertices = { bl,br,tr,tl };
		shape_utils::add_quads(2, angle, tmp_vertices, vertices, indices, index, rotation_axis);

		//Inners
		//Inner top		
		bl = { fiitl, fdnorm, { ftexh - 2*ftexb,ftexb } };
		br = { fiitr, fdnorm, { ftexh - 2*ftexb,ftexw - ftexb } };
		tr = { fitr, fdnorm, { ftexh - ftexb,ftexw - ftexb } };
		tl = { fitl, fdnorm, { ftexh - ftexb,ftexb } };
		tmp_vertices = { bl,br,tr,tl };
		shape_utils::add_quads(2, angle, tmp_vertices, vertices, indices, index, rotation_axis);

		//Inner Bottom
		bl = { fiibr, funorm, { 2*ftexb,ftexw - ftexb } };
		br = { fiibl, funorm, { 2*ftexb,ftexb } };
		tr = { fibl, funorm, { ftexb,ftexb } };
		tl = { fibr, funorm, { ftexb,ftexw - ftexb } };
		tmp_vertices = { bl,br,tr,tl };
		shape_utils::add_quads(2, angle, tmp_vertices, vertices, indices, index, rotation_axis);

		//Inner Left
		bl = { fiibl, frnorm, { ftexb,2*ftexb } };
		br = { fiitl, frnorm, {ftexh - ftexb,2*ftexb } };
		tr = { fitl, frnorm, { ftexh - ftexb,ftexb } };
		tl = { fibl, frnorm, { ftexb,ftexb } };
		tmp_vertices = { bl,br,tr,tl };
		shape_utils::add_quads(2, angle, tmp_vertices, vertices, indices, index, rotation_axis);

		//Inner Right
		bl = { fiitr, flnorm, { ftexh - ftexb,ftexw - 2*ftexb } };
		br = { fiibr, flnorm, { ftexb,ftexw - 2*ftexb } };
		tr = { fibr, flnorm, { ftexb,ftexw - ftexb } };
		tl = { fitr, flnorm, { ftexh - ftexb,ftexw - ftexb } };
		tmp_vertices = { bl,br,tr,tl };
		shape_utils::add_quads(2, angle, tmp_vertices, vertices, indices, index, rotation_axis);
	};

	//--Front and Back (Z axis)
	{
		fnorm = { 0,0,1 };
		flnorm = { -1,0,0 };
		frnorm = { 1,0,0 };
		funorm = { 0,1,0 };
		fdnorm = { 0,-1,0 };
		rotation_axis = { 0,1,0 };
		angle = M_PI;
		fbl = { -x,-y,z };
		fbr = { x,-y,z };
		ftr = { x, y, z };
		ftl = { -x, y, z };
		fibl = { -x_prime, -y_prime, z };
		fibr = { x_prime, -y_prime, z };
		fitr = { x_prime,y_prime,z };
		fitl = { -x_prime,y_prime,z };
		fiibl = { -x_prime,-y_prime,z_prime };
		fiibr = { x_prime,-y_prime,z_prime };
		fiitr = { x_prime,y_prime,z_prime };
		fiitl = { -x_prime,y_prime,z_prime };

		ftexw = tex_x;
		ftexh = tex_y;		

		make_axis_sides();
	}

	//--Right and left (X axis)
	{		
		fnorm = { 1,0,0 };
		flnorm = { 0,0,1 };
		frnorm = { 0,0,-1 };
		funorm = { 0,1,0 };
		fdnorm = { 0,-1,0 };
		angle = M_PI;
		fbl = { x,-y,z };
		fbr = { x,-y,-z };
		ftr = { x, y, -z };
		ftl = { x, y, z };
		fibl = { x, -y_prime, z_prime };
		fibr = { x, -y_prime, -z_prime };
		fitr = { x,y_prime,-z_prime };
		fitl = { x,y_prime,z_prime };
		fiibl = { x_prime,-y_prime,z_prime };
		fiibr = { x_prime,-y_prime,-z_prime };
		fiitr = { x_prime,y_prime,-z_prime };
		fiitl = { x_prime,y_prime,z_prime };

		ftexw = tex_z;
		ftexh = tex_y;

		make_axis_sides();
	}

	//--Top and bottom (Y axis)
	{
		fnorm = { 0,1,0 };
		flnorm = { -1,0,0 };
		frnorm = { 1,0,0 };
		funorm = { 0,0,-1 };
		fdnorm = { 0,0,1 };
		angle = M_PI;
		fbl = { -x,y,z };
		fbr = { x,y,z };
		ftr = { x, y, -z };
		ftl = { -x, y, -z };
		fibl = { -x_prime, y, z_prime };
		fibr = { x_prime, y, z_prime };
		fitr = { x_prime,y,-z_prime };
		fitl = { -x_prime,y,-z_prime };
		fiibl = { -x_prime,y_prime,z_prime };
		fiibr = { x_prime,y_prime,z_prime };
		fiitr = { x_prime,y_prime,-z_prime };
		fiitl = { -x_prime,y_prime,-z_prime };
		rotation_axis = { 0,0,1 };

		ftexw = tex_x;
		ftexh = tex_z;
		
		make_axis_sides();
	}

	//Flipping texture coords, because I specified them all the wrong way round...
	for (auto& vertex:vertices)
	{
		vertex.tex_coords = { vertex.tex_coords.y,vertex.tex_coords.x };
	}
	
	m_meshes.push_back(engine::mesh::create(vertices, indices));
}

engine::hollow_cuboid::~hollow_cuboid()
{
}

engine::ref<engine::hollow_cuboid> engine::hollow_cuboid::create(glm::vec3 half_extents, float border_width, float tex_size)
{
	return std::make_shared<engine::hollow_cuboid>(half_extents,border_width,tex_size);
}


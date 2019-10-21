#pragma once

namespace engine
{
	class mesh;

	///Class creating a rhombicuboctahedron object mesh of a specified size
	class rhombicuboctahedron
	{
	public:
		rhombicuboctahedron(float edge_length = 1.f);

		~rhombicuboctahedron();

		//This shape has two sets of meshes, square faces and triangular faces, that are textured sepparately.
		std::vector < ref<engine::mesh>> meshes() const { return m_meshes; }

		static ref<rhombicuboctahedron> create(float edge_length = 1.f);
	private:
		std::vector< ref<engine::mesh>> m_meshes{};
	};
}

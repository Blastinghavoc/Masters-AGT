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

		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<rhombicuboctahedron> create(float edge_length = 1.f);
	private:
		ref<engine::mesh> m_mesh;
	};
}

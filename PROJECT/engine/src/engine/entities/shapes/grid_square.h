#pragma once


namespace engine
{
	class mesh;

	class grid_square {
	public:
		grid_square(float width = 0.1f);

		~grid_square();

		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<grid_square> create(float width = 0.1f);

	private:
		ref<engine::mesh> m_mesh;
	};
}

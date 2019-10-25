#pragma once

/*
Class representing a single hollow square shape, currently only used for debugging purposes.
May eventually be used as part of the UI to highlight locations that the player can place blocks.
*/
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

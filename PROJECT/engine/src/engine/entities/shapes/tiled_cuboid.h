#pragma once

namespace engine
{
	class mesh;

	/// \brief Class creating a cuboid object with a mesh of a specified size, that has its face textures tiled.
	class tiled_cuboid
	{
	public:
		/// \brief Constructor
		tiled_cuboid(glm::vec3 half_extents, bool inwards, glm::vec2 tile_scale);

		/// \brief Destructor
		~tiled_cuboid();

		/// \brief Getter methods
		glm::vec3 half_extents() const { return m_half_extents; }
		bool inwards() const { return m_inwards; }
		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<tiled_cuboid> create(glm::vec3 half_extents, bool inwards, glm::vec2 tile_scale);
	private:

		glm::vec3 m_half_extents;

		bool m_inwards;

		ref<engine::mesh> m_mesh;
	};
}

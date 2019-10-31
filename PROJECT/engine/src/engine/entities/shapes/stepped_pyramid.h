#pragma once

namespace engine
{
	class mesh;

	///Class creating a stepped pyramid object mesh of a specified size
	class stepped_pyramid
	{
	public:
		stepped_pyramid(float height,float top_side_length,float bottom_side_length,int num_steps, float border_fraction = 0.f);

		~stepped_pyramid();

		//This shape has separate meshes for the faces and borders (if any) so that they can be independently textured.
		std::vector < ref<engine::mesh>> meshes() const { return m_meshes; }

		static ref<stepped_pyramid> create(float height, float top_side_length, float bottom_side_length, int num_steps,float border_fraction = 0.f);
	private:
		std::vector< ref<engine::mesh>> m_meshes{};

	};
}

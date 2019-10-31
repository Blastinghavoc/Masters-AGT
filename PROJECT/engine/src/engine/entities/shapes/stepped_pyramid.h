#pragma once

namespace engine
{
	class mesh;

	///Class creating a stepped pyramid object mesh of a specified size
	class stepped_pyramid
	{
	public:
		stepped_pyramid(float height,float top_side_length,float bottom_side_length,int num_steps);

		~stepped_pyramid();

		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<stepped_pyramid> create(float height, float top_side_length, float bottom_side_length, int num_steps);
	private:
		ref<engine::mesh> m_mesh{};
	};
}

#pragma once
#include <engine.h>

namespace engine
{

	/*
	This class represent a generalised stepped pyramid. That is to say, a pyramid made by stacking
	any polygon. For example, a square-based stepped pyramid is created when num_sides = 4,
	triangular based when num_sides = 3 and N-gon based when num_sides = N.
	*/
	class stepped_pyramid
	{
	public:
		stepped_pyramid(float height,float top_radius,float bottom_radius,int num_steps, float border_fraction = 0.f, int num_sides = 4);

		~stepped_pyramid();

		//This shape has separate meshes for the faces and borders (if any) so that they can be independently textured.
		std::vector < ref<engine::mesh>> meshes() const { return m_meshes; }

		static ref<stepped_pyramid> create(float height, float top_radius, float bottom_radius, int num_steps,float border_fraction = 0.f, int num_sides = 4);
	private:
		std::vector< ref<engine::mesh>> m_meshes{};


	};
}

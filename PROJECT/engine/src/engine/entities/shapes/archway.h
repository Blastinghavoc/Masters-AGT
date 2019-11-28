#pragma once

namespace engine
{
	class mesh;

	//Class representing an untextured archway shape.
	class archway {
	public:
		archway(float width,float squareHeight,float archHeight, float depth);
		~archway() {};

		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<archway> create(float width, float squareHeight, float archHeight, float depth) {
			return std::make_shared<archway>(width, squareHeight, archHeight,depth);
		};

	private:
		ref<engine::mesh> m_mesh;
	};
}

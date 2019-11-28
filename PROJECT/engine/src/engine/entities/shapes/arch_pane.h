#pragma once

namespace engine
{
	class mesh;

	/*Class representing an arch shaped forcefield pane.
	Only intended to serve one purpose, so is not very accomodating to other uses.
	*/
	class arch_pane {
	public:
		arch_pane(float width,float squareHeight,float archHeight);
		~arch_pane() {};

		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<arch_pane> create(float width, float squareHeight, float archHeight) {
			return std::make_shared<arch_pane>(width, squareHeight, archHeight);
		};

	private:
		ref<engine::mesh> m_mesh;
	};
}

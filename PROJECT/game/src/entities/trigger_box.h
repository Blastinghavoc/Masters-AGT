#pragma once
#include <engine.h>

/*Modified from lab 7 bounding box.
Used to detect non-physical collisions, or otherwise
make collision detection explicit rather than handled by Bullet
*/
class trigger_box
{
public:
	trigger_box();
	~trigger_box();
	void get(glm::vec3 &bottom, float &width, float &height, float &depth);

	void set_box(engine::game_object_properties props) {
		set_box(props.bounding_shape.x * 2.f * props.scale.x,
			props.bounding_shape.y * 2.f * props.scale.y,
			props.bounding_shape.z * 2.f * props.scale.z,
			props.position);
	};

	void set_box(engine::ref<engine::game_object> obj) {
		set_box(obj->bounding_shape().x * 2.f * obj->scale().x,
			obj->bounding_shape().y * 2.f * obj->scale().y,
			obj->bounding_shape().z * 2.f * obj->scale().z,
			obj->position());
	};

	void set_box(float width, float height, float depth, glm::vec3 position);
	void on_update(glm::vec3 position);
	void on_render(const engine::ref<engine::shader>& shader,float r = .5f, float g = .5f, float b = .5f);
	bool collision(trigger_box& other_box);
private:
	glm::vec3 m_bottom{0};
	float m_width{0};
	float m_height{0};
	float m_depth{0};
	engine::ref<engine::mesh> m_mesh{};

	float m_rotation_amount{0};
};

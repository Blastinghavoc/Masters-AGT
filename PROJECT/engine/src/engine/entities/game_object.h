#pragma once
#include "bounding_box_bullet.h"

namespace engine
{
	class mesh;
	class texture_2d;
	class shader;
	class skinned_mesh;

    /// \brief Struct used to initialise/create a game object
	struct game_object_properties final
	{
        std::vector<ref<mesh>> meshes{};
		ref<skinned_mesh> animated_mesh{};
		std::vector<ref<texture_2d>> textures{};
        glm::vec3 position{0};
        glm::vec3 velocity{0};
        glm::vec3 scale{1};
	    glm::vec3 bounding_shape{0};
	    glm::vec3 rotation_axis{0.f, 1.f, 0.f};
        int32_t type = 0;
        float mass = 1.f;
		float restitution = 0.1f;
		float friction = 0.9f;
        float rotation_amount = 0.f;
		float rolling_friction = 0.0f;
        bool is_static = false;
		glm::vec3 offset{ 0.f };
		float gravity_scale = 1.f;
	};


	/// \brief Base class for storing the physical information about the objects in the game
	class game_object
    {
    public:
        // TODO - use the following commented ctor only
        game_object(const game_object_properties& props);
        // TODO - lines shouldn't exceed 100 characters, for better readability.
        // TODO - comments in header files should start with "/// \brief ", and there should be one per member/method so that intellisense can show tooltips properly
		/*game_object(const glm::vec3 position, const glm::vec3 velocity, std::vector<ref<mesh>> meshes, const glm::vec3 scale, const bool is_static, float mass, int32_t type, glm::vec3 bounding_shape, glm::vec3 rotation_axis, float rotation_amount);
		game_object(const bool is_static, int32_t type, glm::vec3 bounding_shape);
		game_object(std::vector<ref<mesh>> meshes, int32_t type, glm::vec3 bounding_shape);*/
		/// \brief Destructor -TODO: comments like these are not useful. If the method does something other ppl need to be aware of that's what you should write.
		~game_object();

		/// \brief Getter methods
		glm::vec3 position() const { return m_position; }
		glm::vec3 velocity() const { return m_velocity; }
		glm::vec3 acceleration() const { return m_acceleration; }

		glm::vec3 right() const { return m_right; }
		glm::vec3 up() const { return m_up; }
		glm::vec3 forward() const { return m_forward; }

		glm::vec3 rotation_axis() const { return m_rotation_axis; }
		float     rotation_amount() const { return m_rotation_amount; }
		glm::vec3 angular_velocity() const { return m_angular_velocity; }
		glm::vec3 torque() const { return m_torque; }

		glm::vec3 scale() const { return m_scale; }
		bool      is_static() const { return s_static; }
		float     mass() const { return m_mass; }
		float	  restitution() const { return m_restitution; }
		float	  friction() const { return m_friction; }
		float	  rolling_friction() const { return m_rolling_friction; }
		int32_t   type() const { return m_type; }
		glm::vec3 bounding_shape() const { return m_bounding_shape; }
		const std::vector<ref<mesh>>& meshes() const { return m_meshes; }
		const std::vector<ref<texture_2d>>& textures() const { return m_textures; }
		ref<skinned_mesh> animated_mesh() const { return m_animated_mesh; }

		glm::vec3 offset() const { return m_offset; }

		/// \brief Setter methods
		void set_position(glm::vec3 position) { m_position = position; }
		void set_velocity(glm::vec3 velocity) { m_velocity = velocity; }
		void set_acceleration(glm::vec3 acceleration) { m_acceleration = acceleration; }

		void set_right(glm::vec3 right) { m_right = right; }
		void set_up(glm::vec3 up) { m_up = up; }
		void set_forward(glm::vec3 forward) { m_forward = forward; }

		void set_rotation_axis(glm::vec3 rotation_axis) { m_rotation_axis = rotation_axis; }
		void set_rotation_amount(float rotation_amount) { m_rotation_amount = rotation_amount; }
		void set_angular_velocity(glm::vec3 angular_velocity) { m_angular_velocity = angular_velocity; }
		void set_torque(glm::vec3 torque) { m_torque = torque; }

		void set_scale(glm::vec3 scale) { m_scale = scale; }
		void set_mass(float mass) { m_mass = mass; }
		void set_type(int32_t type) { m_type = type; }
		void set_bounding_shape(glm::vec3 bounding_shape) { m_bounding_shape = bounding_shape; }
		void set_mesh(ref<mesh> mesh) { m_meshes.push_back(mesh); }
		void set_mesh(std::vector<ref<mesh>> meshes) { m_meshes = meshes; }
		void set_textures(std::vector<ref<texture_2d>> textures) { m_textures = textures; }
		void set_animated_mesh(ref<skinned_mesh> animated_mesh) { m_animated_mesh = animated_mesh; }
		void set_offset(glm::vec3 offset) { m_offset = offset; }

		// rotate the object to face the specific direction
		void turn_towards(glm::vec3 direction);

		// create a transform matrix using the object's position, rotation and scale member variables
		const void transform(glm::mat4 &object_transform);

		// bind the object's textures if there are any
		void bind_textures();

		bool is_colliding() const { return s_colliding; }
		std::vector<engine::ref<engine::game_object>> collision_objects() const {
			return
				m_collision_objects;
		}
		void set_collision_state(bool col_state) { s_colliding = col_state; }
		void clear_collision_objects() { m_collision_objects.clear(); }
		void add_collision_object(engine::ref<engine::game_object> object) {
			m_collision_objects.push_back(object);
		}

		void set_angular_factor(glm::vec3 factor) { m_angular_factor = factor; };
		glm::vec3 get_angular_factor() { return m_angular_factor; };

		void update_obb() {
			m_obb.on_update(position() - glm::vec3(0.f,
				offset().y, 0.f) * scale(),
				rotation_amount(),rotation_axis());
		};

		void render_obb(glm::vec3 rgb,const engine::ref<engine::shader>& shader) {
			update_obb();
			m_obb.on_render(rgb.x, rgb.y, rgb.z, shader);
		}

		//Allow enabling/disabling physics
		void set_active(bool state) { m_physics_active = state;
			if (m_physics_active)
			{
				//Just been activated, reset
				reset_physics_properties();
			}
			else {
				//Just been deactivated, move to somewhere "off the map"
				m_position = glm::vec3(-100);
			}
		};

		bool is_active() { return m_physics_active; };

		void reset_physics_properties() {
			m_acceleration = glm::vec3(0);
			m_velocity = glm::vec3(0);
			m_angular_velocity = glm::vec3(0);
			m_torque = glm::vec3(0);
			m_position = glm::vec3(0);
		};

		float gravity_scale() { return m_gravity_scale; };
    public:
        static ref<game_object> create(const game_object_properties& props);

    private:
		/// \brief Fields
		// object's position vectors
		glm::vec3		m_position{ 0.f };
		// object's velocity vector 
		glm::vec3		m_velocity{ 0.f };
		// object's acceleration vector
		glm::vec3		m_acceleration{ 0.f };

		// object's right vector
		glm::vec3		m_right{ 1.0f, 0.0f, 0.0f };
		// object's up vector
		glm::vec3		m_up{ 0.0f, 1.0f, 0.0f };
		// object's forward vector
		glm::vec3		m_forward{ 0.0f, 0.0f, 1.0f };

		// object's rotation axis vector 
		glm::vec3		m_rotation_axis{ 0.0f, 1.0f, 0.0f };
		// object's rotation amount
		float			m_rotation_amount{ 0.f };

		// object's angular velocity vector
		glm::vec3		m_angular_velocity{ 0.0f };
		// object's torque vector
		glm::vec3		m_torque{ 0.0f };
		// object's inertia

		// object's meshes
		std::vector<ref<mesh>> m_meshes;
		// object's textures
		std::vector<ref<texture_2d>> m_textures;

		// object'sanimated mesh
		ref<skinned_mesh> m_animated_mesh;

		// object's scale vector
		glm::vec3		m_scale{ 1.0f };
		// object's mass
		float			m_mass{ 1.0f };
		// object's restitution
		float			m_restitution{ 0.1f };
		// object's friction
		float			m_friction{ 0.9f };

		// object's rolling friction
		float			m_rolling_friction{ 0.0f };

        // static/non-static object
		bool			s_static{ false };

		// object's bounding shape type
		int32_t			m_type;
		// object's bounding shape dimensions
		glm::vec3		m_bounding_shape{ 0.f };
		//object's offset from the local origin
		glm::vec3 m_offset{ 0.f };

		//---Physics stuff
		bool s_colliding = false;
		std::vector<engine::ref<engine::game_object>> m_collision_objects{};

		glm::vec3 m_angular_factor{ 1.f };

		bounding_box m_obb;

		bool m_physics_active = true;

		//Lifetime constant representing how much this object is affected by gravity
		float m_gravity_scale = 1.f;		
    };
}

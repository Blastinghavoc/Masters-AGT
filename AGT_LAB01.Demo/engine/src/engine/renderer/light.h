#pragma once
namespace engine
{
	class shader;

	class light
	{
	public:
		light(const glm::vec3 position, const glm::vec3 colour, const float ambient, const float diffuse, const float specular);
		~light();

		glm::vec3 position() const { return m_position; }
		glm::vec3 colour() const { return m_colour; }
		float ambient() const { return m_ambient; }
		float diffuse() const { return m_diffuse; }
		float specular() const { return m_specular; }

		void set_position(glm::vec3 position) { m_position = position; }
		void set_colour(glm::vec3 colour) { m_colour = colour; }
		void set_ambient(float ambient) { m_ambient = ambient; }
		void set_diffuse(float diffuse) { m_diffuse = diffuse; }
		void set_specular(float specular) { m_specular = specular; }

		static ref<light> create(const glm::vec3 position, const glm::vec3 colour,
			const float ambient, const float diffuse, const float specular);

		void submit(const ref<shader> shader);

	private:
		glm::vec3 m_position;
		glm::vec3 m_colour;
		float m_ambient;
		float m_diffuse;
		float m_specular;
	};
}

// Basic Texture Shader

#type vertex
#version 430  

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_tex_coord;  

uniform mat4 u_view_projection;  
uniform mat4 u_transform;

out vec3 v_position;
out vec3 v_normal;

void main()  
{  
	v_position = vec3(u_transform * vec4(a_position, 1.0));
    v_normal = mat3(transpose(inverse(u_transform))) * a_normal;
    gl_Position = u_view_projection * u_transform * vec4(a_position, 1.0);  
}  

#type fragment
#version 430  
  
layout(location = 0) out vec4 o_color;  

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;

struct Light {
    vec3 position;
	vec3 colour;

    float ambient;
    float diffuse;
    float specular;
};

in vec3 v_position;
in vec3 v_normal;

uniform Light light;

uniform vec3 view_pos;
  
void main()  
{  
	// ambient
	vec3 ambient = light.ambient * light.colour * material.ambient;
 	
	// diffuse 
	vec3 norm = normalize(v_normal);
	vec3 light_dir = normalize(light.position - v_position);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = light.diffuse * light.colour * diff * material.diffuse;
	// specular
	vec3 view_dir = normalize(view_pos - v_position);
	vec3 reflect_dir = reflect(-light_dir, norm);  
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular = light.specular * light.colour * spec *  material.specular;
       
	vec3 result = (ambient + diffuse + specular);

	
    o_color = vec4(result, 1.0f);
}

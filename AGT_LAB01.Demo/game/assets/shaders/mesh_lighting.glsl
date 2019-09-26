// Basic Texture Shader

#type vertex
#version 430  

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_tex_coord;  

uniform mat4 u_view_projection;  
uniform mat4 u_transform;  

out vec2 v_tex_coord;
out vec3 v_position;
out vec3 v_normal;

void main()  
{  
    v_tex_coord = a_tex_coord;
	v_position = vec3(u_transform * vec4(a_position, 1.0));
    v_normal = mat3(transpose(inverse(u_transform))) * a_normal;
    gl_Position = u_view_projection * u_transform * vec4(a_position, 1.0);  
}  

#type fragment
#version 430  
  
layout(location = 0) out vec4 o_color;  

struct Light {
    vec3 position;
	vec3 colour;

    float ambient;
    float diffuse;
    float specular;
};

in vec2 v_tex_coord;
in vec3 v_position;
in vec3 v_normal;
  
uniform sampler2D u_sampler;

uniform Light light;

uniform vec3 view_pos;

uniform float shininess;

uniform bool skybox_rendering = false;
  
void main()  
{  
    vec4 tex_color = texture(u_sampler, v_tex_coord);

	vec3 result;

	if(skybox_rendering)
	{
		result = tex_color.rgb;

		// uncomment the following to affect the skybox with the light colour
		//result = light.colour * tex_color.rgb;
	}
	else
	{
		// ambient
		vec3 ambient = light.ambient * light.colour;
  	
		// diffuse 
		vec3 norm = normalize(v_normal);
		vec3 light_dir = normalize(light.position - v_position);
		float diff = max(dot(norm, light_dir), 0.0);
		vec3 diffuse = light.diffuse * light.colour * diff;

		// specular
		vec3 view_dir = normalize(view_pos - v_position);
		vec3 reflect_dir = reflect(-light_dir, norm);  
		float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
		vec3 specular = light.specular * light.colour * spec;  
        
		result = (ambient + diffuse + specular) * tex_color.rgb;
	}

	
    o_color = vec4(result, tex_color.a);
}

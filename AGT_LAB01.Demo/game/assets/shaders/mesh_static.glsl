// Basic Texture Shader

#type vertex
#version 430  

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_tex_coord;  

uniform mat4 u_view_projection;  
uniform mat4 u_transform;  

out vec2 v_tex_coord;   

void main()  
{  
    v_tex_coord = a_tex_coord;  
    gl_Position = u_view_projection * u_transform * vec4(a_position, 1.0);  
}  

#type fragment
#version 430  
  
layout(location = 0) out vec4 o_color;  
  
in vec2 v_tex_coord;  
  
uniform sampler2D u_sampler;   
  
void main()  
{  
    o_color = texture(u_sampler, v_tex_coord); 
}  

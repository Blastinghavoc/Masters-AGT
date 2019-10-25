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
out vec4 v_pos;

void main()  
{  
	v_position = vec3(u_transform * vec4(a_position, 1.0));
    v_normal = mat3(transpose(inverse(u_transform))) * a_normal;
	v_pos = u_view_projection * u_transform * vec4(a_position, 1.0); 
    gl_Position = v_pos;
  
}  

#type fragment
#version 430   

layout(location = 0) out vec4 o_color;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
	float transparency;
}; 

const int MAX_POINT_LIGHTS = 2;
const int MAX_SPOT_LIGHTS = 2;

in vec3 v_position;
in vec3 v_normal;                                                              

struct VSOutput
{
    vec3 Normal;                                                                   
    vec3 WorldPos;                                                                 
};


struct BaseLight
{
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
};

struct DirectionalLight
{
    BaseLight Base;
    vec3 Direction;
};
                                                                                    
struct Attenuation                                                                  
{                                                                                   
    float Constant;                                                                 
    float Linear;                                                                   
    float Exp;                                                                      
};                                                                                  
                                                                                    
struct PointLight                                                                           
{                                                                                           
    BaseLight Base;                                                                  
    vec3 Position;                                                                          
    Attenuation Atten;                                                                      
};                                                                                          
                                                                                            
struct SpotLight                                                                            
{                                                                                           
    PointLight Base;                                                                 
    vec3 Direction;                                                                         
    float Cutoff;                                                                           
};                                                                                          
                                                                                            
uniform int gNumPointLights;                                                                
uniform int gNumSpotLights;                                                                 
uniform DirectionalLight gDirectionalLight;                                                 
uniform PointLight gPointLights[MAX_POINT_LIGHTS];                                          
uniform SpotLight gSpotLights[MAX_SPOT_LIGHTS];                                            
uniform vec3 gEyeWorldPos;                                                                  
uniform float gMatSpecularIntensity;                                                        
uniform float gSpecularPower;
uniform Material material;
uniform bool lighting_on = true;
uniform bool fog_on = false;
uniform vec3 fog_colour;
uniform int fog_factor_type;
in vec4 v_pos;
float rho = 0.15f;
float fog_start = 3.0f;
float fog_end = 15.0f;


vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, VSOutput In)            
{                                                                                           
    vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity * material.ambient, 1.0f);
    float DiffuseFactor = dot(In.Normal, -LightDirection);                                     
                                                                                            
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                                  
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                                  
                                                                                            
    if (DiffuseFactor > 0.0) {                                                                
        DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor * material.diffuse, 1.0f);
                                                                                            
        vec3 VertexToEye = normalize(gEyeWorldPos - In.WorldPos);                             
        vec3 LightReflect = normalize(reflect(LightDirection, In.Normal));                     
        float SpecularFactor = dot(VertexToEye, LightReflect);                                      
        if (SpecularFactor > 0.0) {                                                         
            SpecularFactor = pow(SpecularFactor, gSpecularPower);
            SpecularColor = vec4(Light.Color * gMatSpecularIntensity * SpecularFactor * material.specular * material.shininess, 1.0f);
        }                                                                                   
    }                                                                                       
                                                                                            
    return (AmbientColor + DiffuseColor + SpecularColor);                                   
}                                                                                           
                                                                                            
vec4 CalcDirectionalLight(VSOutput In)                                                      
{                                                                                           
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, In);  
}                                                                                           
                                                                                            
vec4 CalcPointLight(PointLight l, VSOutput In)                                       
{                                                                                           
    vec3 LightDirection = In.WorldPos - l.Position;                                           
    float Distance = length(LightDirection);                                                
    LightDirection = normalize(LightDirection);                                             
                                                                                            
    vec4 Color = CalcLightInternal(l.Base, LightDirection, In);                         
    float Attenuation =  l.Atten.Constant +                                                 
                         l.Atten.Linear * Distance +                                        
                         l.Atten.Exp * Distance * Distance;                                 
                                                                                            
    return Color / Attenuation;                                                             
}                                                                                           
                                                                                            
vec4 CalcSpotLight(SpotLight l, VSOutput In)                                         
{                                                                                           
    vec3 LightToPixel = normalize(In.WorldPos - l.Base.Position);                             
    float SpotFactor = dot(LightToPixel, l.Direction);                                      
                                                                                            
    if (SpotFactor > l.Cutoff) {                                                            
        vec4 Color = CalcPointLight(l.Base, In);                                        
        return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));                   
    }                                                                                       
    else {                                                                                  
        return vec4(0,0,0,0);                                                               
    }                                                                                       
}

void main()  
{
	VSOutput In;
    In.Normal   = normalize(v_normal);
    In.WorldPos = v_position;
	if(lighting_on) {
		vec4 TotalLight = CalcDirectionalLight(In);                                         
                                                                                            
		for (int i = 0 ; i < gNumPointLights ; i++) {                                           
			TotalLight += CalcPointLight(gPointLights[i], In);                              
		}                                                                                       
                                                                                            
		for (int i = 0 ; i < gNumSpotLights ; i++) {                                            
			TotalLight += CalcSpotLight(gSpotLights[i], In);                                
		}

		if(material.transparency<1.0)
			TotalLight.w = material.transparency;
	
	    o_color = TotalLight;
	}
	else {
		o_color = vec4(material.ambient, material.transparency);
	}

	if(fog_on)
	{
		float d = length(v_pos.xyz);
		float w;
		if(fog_factor_type == 0) {
			if (d < fog_end)
				w = (fog_end - d) / (fog_end-fog_start);
			else
				w = 0;
		} else if (fog_factor_type == 1) {
			w = exp(-(rho*d));
		} else {
			w = exp(-(rho*d)*(rho*d));
		}
		o_color.rgb = mix(fog_colour, o_color.rgb, w);
	}

	
}

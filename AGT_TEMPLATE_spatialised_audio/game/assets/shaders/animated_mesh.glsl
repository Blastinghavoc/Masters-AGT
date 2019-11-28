// -----------------------------
// -- Hazel Engine PBR shader --
// -----------------------------
// Note: this shader is still very much in progress. There are likely many bugs and future additions that will go in.
//       Currently heavily updated. 
//
// References upon which this is based:
// - Unreal Engine 4 PBR notes (https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf)
// - Frostbite's SIGGRAPH 2014 paper (https://seblagarde.wordpress.com/2015/07/14/siggraph-2014-moving-frostbite-to-physically-based-rendering/)
// - Michał Siejak's PBR project (https://github.com/Nadrin)
// - My implementation from years ago in the Sparky engine (https://github.com/TheCherno/Sparky)
#type vertex
#version 430 core                                                               
                                                                                    
layout (location = 0) in vec3 Position;                                             
layout (location = 1) in vec2 TexCoord;                                             
layout (location = 2) in vec3 Normal;                                               
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

out vec2 TexCoord0;
out vec3 Normal0;                                                                   
out vec3 WorldPos0;  
out vec4 v_pos;                                                               

const int MAX_BONES = 100;

uniform mat4 u_view_projection;
uniform mat4 u_ModelMatrix;
uniform mat4 gWorld;
uniform mat4 gBones[MAX_BONES];
uniform int num_bones;

void main()
{
	vec4 PosL;
	vec4 NormalL;
	if(num_bones>0){

		mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
		BoneTransform     += gBones[BoneIDs[1]] * Weights[1];
		BoneTransform     += gBones[BoneIDs[2]] * Weights[2];
		BoneTransform     += gBones[BoneIDs[3]] * Weights[3];
		PosL = BoneTransform * vec4(Position, 1.0);
		NormalL = BoneTransform * vec4(Normal, 0.0);

		gl_Position  = u_view_projection * u_ModelMatrix * PosL;
		v_pos = gl_Position;
		TexCoord0    = TexCoord;
		Normal0      = (u_ModelMatrix* NormalL).xyz;
		WorldPos0    = (u_ModelMatrix* PosL).xyz;
	} else {
		gl_Position  = u_view_projection * u_ModelMatrix * vec4(Position, 1.0);
		v_pos = gl_Position;
		TexCoord0    = TexCoord;
		Normal0		 = mat3(transpose(inverse(u_ModelMatrix))) * Normal;
		WorldPos0    = vec3(u_ModelMatrix * vec4(Position, 1.0));
		
	}

    


}


#type fragment
#version 430 core

const int MAX_POINT_LIGHTS = 2;
const int MAX_SPOT_LIGHTS = 2;

in vec2 TexCoord0;
in vec3 Normal0;                                                                   
in vec3 WorldPos0;                                                                 

struct VSOutput
{
    vec2 TexCoord;
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
uniform sampler2D gColorMap;                                                                
uniform vec3 gEyeWorldPos;                                                                  
uniform float gMatSpecularIntensity;                                                        
uniform float gSpecularPower;
uniform float transparency;
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
    vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0);
    float DiffuseFactor = dot(In.Normal, -LightDirection);                                     
                                                                                            
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                                  
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                                  
                                                                                            
    if (DiffuseFactor > 0.0) {                                                                
        DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0);
                                                                                            
        vec3 VertexToEye = normalize(gEyeWorldPos - In.WorldPos);                             
        vec3 LightReflect = normalize(reflect(LightDirection, In.Normal));                     
        float SpecularFactor = dot(VertexToEye, LightReflect);                                      
        if (SpecularFactor > 0.0) {                                                         
            SpecularFactor = pow(SpecularFactor, gSpecularPower);
            SpecularColor = vec4(Light.Color * gMatSpecularIntensity * SpecularFactor, 1.0);
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
                            
out vec4 FragColor;
                                                                
void main()
{                                    
    VSOutput In;
    In.TexCoord = TexCoord0;
    In.Normal   = normalize(Normal0);
    In.WorldPos = WorldPos0;

	if(lighting_on){
		vec4 TotalLight = CalcDirectionalLight(In);                                         
                                                                                            
		for (int i = 0 ; i < gNumPointLights ; i++) {                                           
			TotalLight += CalcPointLight(gPointLights[i], In);                              
		}                                                                                       
                                                                                            
		for (int i = 0 ; i < gNumSpotLights ; i++) {                                            
			TotalLight += CalcSpotLight(gSpotLights[i], In);                                
		}

		TotalLight.w = transparency;
                                                                                            
		FragColor = texture(gColorMap, In.TexCoord.xy) * TotalLight;  
	} else {
		FragColor = texture(gColorMap, In.TexCoord.xy);  
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
		FragColor.rgb = mix(fog_colour, FragColor.rgb, w);
	}

       
}

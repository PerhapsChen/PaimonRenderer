#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

uniform float outlineScale;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    
    vec4 pos_afterMV = view * model * vec4(aPos, 1.0);
    vec3 normal_afterMV = mat3(view) * mat3(model) * aNormal;
    normal_afterMV.z = -0.5,
    pos_afterMV = pos_afterMV + vec4(normalize(normal_afterMV), 0) * outlineScale;  //_Outline;


    gl_Position = projection * pos_afterMV;
}
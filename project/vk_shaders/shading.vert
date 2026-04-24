#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normalIn;
layout(location = 2) in vec2 texCoordIn;

layout(location = 0) out vec2 texCoord;
layout(location = 1) out vec3 viewSpaceNormal;
layout(location = 2) out vec3 viewSpacePosition;

layout(set = 0, binding = 10) uniform ShadingVertexUBO
{
    mat4 normalMatrix;
    mat4 modelViewMatrix;
    mat4 modelViewProjectionMatrix;
};

void main()
{
    gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
    texCoord = texCoordIn;
    viewSpaceNormal = (normalMatrix * vec4(normalIn, 0.0)).xyz;
    viewSpacePosition = (modelViewMatrix * vec4(position, 1.0)).xyz;
}
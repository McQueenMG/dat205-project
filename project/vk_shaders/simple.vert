#version 450

layout(location = 0) in vec3 position;

layout(set = 0, binding = 0) uniform SimpleVertexUBO
{
    mat4 modelViewProjectionMatrix;
};

void main()
{
    gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
}
#version 450

layout(location = 0) out vec4 fragmentColor;

layout(set = 0, binding = 1) uniform SimpleFragmentUBO
{
    vec3 material_color;
};

void main()
{
    fragmentColor = vec4(material_color, 1.0);
}
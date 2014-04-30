#version 150

in vec4 vPosition;
in vec3 vTangent;
in vec3 vNormal;
in vec2 vTexCoord;

void main()
{
    gl_Position = vPosition;
}
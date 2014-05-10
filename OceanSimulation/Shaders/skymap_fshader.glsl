#version 150 

uniform samplerCube skymap;

in vec3 R;

out vec4 fragment;

void main()
{
    fragment = texture(skymap, R);
}
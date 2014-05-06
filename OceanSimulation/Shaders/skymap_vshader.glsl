#version 150 

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

in vec3 vPosition;

out vec3 R;

void main()
{
    vec4 pos = (View * Model * vec4(vPosition, 1.0));

    R = pos.xyz;

    gl_Position = Projection * pos;
}
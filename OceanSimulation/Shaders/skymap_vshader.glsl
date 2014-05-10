#version 150 

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

in vec3 vPosition;

out vec3 R;

void main()
{
    R = vPosition;

    gl_Position = Projection * (View * Model * vec4(vPosition, 1.0));
}
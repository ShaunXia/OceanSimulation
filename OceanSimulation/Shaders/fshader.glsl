#version 150

uniform vec4 Color;

in vec3 LightDirection;
in vec4 vColor;

out vec4 fColor;

void main()
{
    vec3 L = normalize(LightDirection);

    //draw lines
    //fColor = Color;

    fColor = vColor;

    //check light source:
    //fColor = vec4((L + 1.0) / 2.0, 1.0);
}
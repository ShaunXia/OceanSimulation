#version 150

uniform vec4 LightPosition;
uniform vec3 cameraPosition;
uniform mat4 View;
uniform mat4 Model;
uniform mat4 Projection;

in vec3 vPosition;
in vec3 vNormal;

out vec3 Normal;
out vec3 Incident;


void main()
{
    Incident = -normalize((Model*vec4(vPosition, 1.0)).xyz - cameraPosition);
    Normal = normalize(vNormal);

    gl_Position = Projection * (View * Model * vec4(vPosition, 1.0));
}
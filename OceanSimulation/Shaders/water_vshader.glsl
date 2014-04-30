#version 150

uniform vec4 LightPosition;
uniform mat4 View;
uniform mat4 Model;
uniform mat4 Projection;

in vec3 vPosition;
in vec3 vNormal;

out vec3 N;
out vec3 E;
out vec3 H;
out vec3 LightDirection;

void main()
{
    float nSnell = 1.34;
     
    vec3 vVertex = (View * Model * vec4(vPosition, 1.0)).xyz;

    LightDirection = normalize((View * Model * LightPosition).xyz - vVertex);
    E = normalize(-vVertex);
    H = normalize (LightDirection + E);

    N = normalize(View * Model * vec4(vNormal, 0.0)).xyz;

    vec4 AmbientProduct = vec4(0.53,0.81,0.98,1.0)* vec4(0.18,0.34,0.55,0.0);

    vec4 DiffuseProduct = vec4(1.0,1.0,1.0,1.0) * vec4(0.18,0.34,0.55,0.0);

    gl_Position = Projection * (View * Model * vec4(vPosition, 1.0));
    //gl_Position = Projection * Model * vec4(vPosition, 1.0);
}
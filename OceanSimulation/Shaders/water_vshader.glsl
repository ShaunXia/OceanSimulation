#version 150

uniform vec4 LightPosition;
uniform mat4 View;
uniform mat4 Model;
uniform mat4 Projection;

in vec3 vPosition;
in vec3 vNormal;

out vec3 LightDirection;
out vec4 vColor;


void main()
{
    vec3 vVertex = (View * Model * vec4(vPosition, 1.0)).xyz;

    LightDirection = normalize((View * Model * LightPosition).xyz - vVertex);

    //vec3 N = normalize(View * Model * vec4(vNormal, 0.0)).xyz;
    vec3 N = normalize(vNormal);

    vec4 AmbientProduct = vec4(0.53,0.81,0.98,1.0)* vec4(0.18,0.34,0.55,0.0);
    vec4 ambient = AmbientProduct;

    float kd = max(dot(LightDirection, N), 0.0);
    vec4 DiffuseProduct = vec4(1.0,1.0,1.0,1.0) * vec4(0.18,0.34,0.55,0.0);
    vec4 diffuse = kd * DiffuseProduct;

    //vColor = vec4((vNormal + 1)/2, 1.0);
    vec4 rgba = ambient + diffuse;
    vColor = rgba;

    gl_Position = Projection * (View * Model * vec4(vPosition, 1.0));
    //gl_Position = Projection * Model * vec4(vPosition, 1.0);
}
#version 150

uniform samplerCube texMap;

in vec3 Normal;
in vec3 Incident;

out vec4 fColor;

void main()
{
    vec3 N = normalize(Normal);
    vec3 I = normalize(Incident);

    vec4 Ko = vec4(1.0, 0.0, 1.0, 1.0);
    float f = clamp(pow(1.0 - dot(I, N), 3.0), 0.0, 1.0);


    vec3 R = reflect(I, N);
    fColor = mix(Ko, textureCube(texMap, R), f);
}
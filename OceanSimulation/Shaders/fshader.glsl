#version 150

uniform samplerCube texMap;

in vec3 Normal;
in vec3 Incident;
in mat4 ViewMatrix;

out vec4 fColor;

void main()
{
    vec3 N = normalize(Normal);
    vec3 I = normalize(Incident);

    vec4 Ko = vec4(0.03, 0.30, 0.30, 1.0);

    vec3 reflection = reflect(I, N);

    vec4 reflectionColor = texture(texMap, reflection);

    float f = clamp(pow(1.0 - dot(I, N), 3.0), 0.0, 1.0);

    fColor = mix(Ko, reflectionColor, f);
    //fColor = reflectionColor;
    //fColor = vec4((N + 1)/2, 1.0);
}
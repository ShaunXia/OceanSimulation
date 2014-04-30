#version 150

uniform vec4 Color;

in vec3 N;
in vec3 E;
in vec3 H;
in vec3 LightDirection;

out vec4 fColor;

void main()
{
    vec3 L = normalize(LightDirection);

    float facing    = max(0.0, dot(H, -E));
    float fresnel   = pow(1.0 - facing, 5.0); // Fresnel approximation
    float diffuse   = max(0.0, dot(N, LightDirection));
    
    vec4 waterColor = Color;
    vec4 sky = vec4(0.53,0.81,0.98,1.0);

    fColor = waterColor*diffuse + Color*fresnel;

    //check light source:
    //fColor = vec4((L + 1.0) / 2.0, 1.0);
}
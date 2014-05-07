#version 150 

uniform samplerCube skymap;
uniform sampler2D diffuse;
//uniform vec4 AmbientProduct;

//in vec3 fn;
in vec3 fv;
//in vec3 fl;
in vec2 ft;

out vec4 fragment;

void main()
{
    //vec3 n = normalize(fn);
    vec3 v = normalize(fv);
    //vec3 l = normalize(fl);

    //float kd = max(dot(l, n), 0.0);
    vec4 d = texture(diffuse, ft); //texture2D in version 120

    //vec4 AmbientProduct = vec4(0.53,0.81,0.98,1.0)* vec4(0.18,0.34,0.55,0.0);

    if (d.a < 0.5)
        discard;
    else
    {
       //fragment = (AmbientProduct + kd) * d;
       fragment = d;
    }

    //fragment = vec4(0.0, 1.0, 0.0, 0.0);
}
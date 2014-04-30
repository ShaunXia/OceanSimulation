#version 150 

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
//uniform vec4 LightPosition;

in vec3 vPosition;
//in vec3 vNormal;
in vec2 vTexCoord;

//out vec3 fn;
out vec3 fv;
//out vec3 fl;
out vec2 ft;

void main()
{
    vec3 pos = (View * Model * vec4(vPosition, 1.0)).xyz;

    //fn = (ModelView * vec4(vNormal, 0.0)).xyz;
    fv = -pos;
    //fl = (ModelView * LightPosition).xyz - pos;
    ft = vTexCoord;

    gl_Position = Projection * (View * Model * vec4(vPosition, 1.0));
}
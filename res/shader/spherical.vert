#version 420 core

layout (location=0) in vec3 P;
layout (location=1) in float T;

uniform mat4 MVP;

smooth out float segment;

void main()
{
    segment = T;
    gl_Position = MVP * vec4(P.x*cos(P.y)*cos(P.z), P.x*cos(P.y)*sin(P.z), P.x*sin(P.y), 1.0);
}

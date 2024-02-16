#version 460 core

layout (location=0) in vec3 P;
layout (location=1) in float T;

uniform mat4 MVP;

smooth out float segment;

void main()
{
    segment = T;
    gl_Position = MVP * vec4(P, 1.0);
}

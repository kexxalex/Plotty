#version 420 core

layout (location=0) in vec3 P;

uniform mat4 MVP;

out vec3 pos;

void main()
{
    pos = P;
    gl_Position = MVP * vec4(P, 1.0);
}

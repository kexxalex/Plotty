#version 420 core

layout (location=0) out vec4 fragColor;

in vec3 pos;

void main()
{
    float d = 1.0 / (1.0 + length(pos));
    if (pos.x == 0.0 && pos.y == 0.0)
        fragColor = vec4(0.5, 0.5, 0.5, d)*d;
    else if (pos.x == 0.0 && pos.z == 0.0)
        fragColor = vec4(0.5, 0.5, 0.5, d)*d;
    else if (pos.y == 0.0 && pos.z == 0.0)
        fragColor = vec4(0.5, 0.5, 0.5, d)*d;
    else if (pos.x == 0.0)
        fragColor = vec4(0.7, 0.0, 0.0, d)*d;
    else if (pos.y == 0.0)
        fragColor = vec4(0.0, 0.7, 0.0, d)*d;
    else if (pos.z == 0.0)
        fragColor = vec4(0.0, 0.5, 0.8, d)*d;
}

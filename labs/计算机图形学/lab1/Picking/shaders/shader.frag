#version 400 core

uniform bool picked;
out vec4 outColor;

void main()
{
   outColor = picked ? vec4(0.0f, 0.5f, 0.0f, 1.0f) : vec4(0.5f, 0.0f, 0.0f, 1.0f);
}

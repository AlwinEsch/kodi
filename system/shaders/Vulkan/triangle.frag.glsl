#version 450

layout(location = 0) in vec2 frag_cord0;
layout(location = 1) in vec2 frag_cord1;
layout(location = 2) in vec4 frag_colour;

layout (location = 0) out vec4 outFragColor;

void main()
{
  outFragColor = frag_colour;
}
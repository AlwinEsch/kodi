#version 450

layout (set = 1, binding = 0) uniform sampler2D samplerColor;

layout(location = 0) in vec2 frag_cord0;
layout(location = 1) in vec2 frag_cord1;
//layout(location = 2) in vec4 frag_colour;

layout(push_constant) uniform PushConsts {
  vec4 colour;
} pushConsts;

layout (location = 0) out vec4 outFragColor;

void main()
{
  outFragColor = texture(samplerColor, frag_cord0) * pushConsts.colour;
  //outFragColor = texture(samplerColor, frag_cord0);
  //outFragColor = pushConsts.colour;
}

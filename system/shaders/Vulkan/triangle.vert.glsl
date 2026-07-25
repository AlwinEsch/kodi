#version 450

layout(location = 0) in vec3 in_attrpos;
layout(location = 1) in vec4 in_attrcol;
layout(location = 2) in vec2 in_attrcord0;
layout(location = 3) in vec2 in_attrcord1;

layout (binding = 0) uniform UBO
{
	mat4 projectionMatrix;
	mat4 modelMatrix;
	mat4 viewMatrix;
} ubo;

layout(location = 0) out vec2 frag_cord0;
layout(location = 1) out vec2 frag_cord1;
layout(location = 2) out vec4 frag_colour;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
	gl_Position = ubo.projectionMatrix * ubo.modelMatrix * vec4(in_attrpos.xyz, 1.0);
  frag_colour = in_attrcol;
  frag_cord0 = in_attrcord0;
  frag_cord1 = in_attrcord1;
}

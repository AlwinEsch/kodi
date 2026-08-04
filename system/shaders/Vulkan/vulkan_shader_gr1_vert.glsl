/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#version 450

layout(location = 0) in vec3 in_attrpos;
layout(location = 1) in vec2 in_attrcord0;
layout(location = 2) in vec2 in_attrcord1;

layout (set = 0, binding = 0) uniform UBO
{
  mat4 projectionMatrix;
  mat4 modelMatrix;
  float depth;
} ubo;

layout(location = 0) out vec2 frag_cord0;
layout(location = 1) out vec2 frag_cord1;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
	gl_Position = ubo.projectionMatrix * ubo.modelMatrix * vec4(in_attrpos.xyz, 1.0);
  gl_Position.z = ubo.depth * gl_Position.w;
  frag_cord0 = in_attrcord0;
  frag_cord1 = in_attrcord1;
}

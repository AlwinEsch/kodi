/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#version 450

layout(location = 0) in vec3 in_attrpos;
layout(location = 1) in vec4 in_attrcol;
layout(location = 2) in vec2 in_attrcord0;

layout(set = 0, binding = 0) uniform UBO
{
  mat4 projModelMatrix;
  float depth;
} u_ubo;

layout(push_constant) uniform PushConsts {
  mat4 viewMatrix;
} pushConsts;

layout(location = 0) out vec2 frag_cord0;
layout(location = 1) out vec4 frag_color;

void main ()
{
  gl_Position = u_ubo.projModelMatrix * pushConsts.viewMatrix * vec4(in_attrpos, 1.);
  gl_Position.z = u_ubo.depth * gl_Position.w;
  frag_color = in_attrcol;
  frag_cord0 = in_attrcord0;
}

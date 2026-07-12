/*
 *  Copyright (C) 2010-2024 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#version 450

layout(location = 0) in vec2 in_attrpos;
layout(location = 1) in vec4 in_attrcol;
layout(location = 2) in vec2 in_attrcord0;
layout(location = 3) in vec2 in_attrcord1;

layout(set = 0, binding = 0) uniform UBO
{
  mat4 proj;
  mat4 model;
  float depth;
} u_ubo;

layout(location = 0) out vec2 frag_cord0;
layout(location = 1) out vec2 frag_cord1;
layout(location = 2) out vec4 frag_colour;

void main()
{
  mat4 mvp = u_ubo.proj * u_ubo.model;
  gl_Position = mvp * vec4(in_attrpos, 0., 1.);
  gl_Position.z = u_ubo.depth * gl_Position.w;
  frag_colour = in_attrcol;
  frag_cord0 = in_attrcord0;
  frag_cord1 = in_attrcord1;
}

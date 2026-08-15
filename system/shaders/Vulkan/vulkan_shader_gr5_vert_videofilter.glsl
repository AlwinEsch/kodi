/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#version 450

layout(location = 0) in vec4 in_attrpos;
layout(location = 1) in vec2 in_attrcord;

layout(binding = 0) uniform UBO
{
  mat4 projection;
  mat4 model;
} u_ubo;

layout(location = 0) out vec2 frag_cord;

void main ()
{
  mat4 mvp  = u_ubo.projection * u_ubo.model;
  gl_Position = mvp * in_attrpos;
  frag_cord = in_attrcord;
}

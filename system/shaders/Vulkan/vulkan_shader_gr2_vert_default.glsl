/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#version 450

layout(location = 0) in vec2 in_position;

layout(set = 0, binding = 0) uniform UBO
{
  mat4 projection;
  mat4 model;
} u_ubo;

void main()
{
  mat4 mvp = u_ubo.projection * u_ubo.model;
  gl_Position = mvp * vec4(in_position, 0., 1.);
  gl_Position.z = -1. * gl_Position.w;
}

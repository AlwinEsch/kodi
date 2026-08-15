/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#version 450

layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_tex;

layout(binding = 0) uniform UBO
{
  mat4 u_proj;
} ubo;

layout(location = 0) out vec2 frag_tex;

void main()
{
  gl_Position = ubo.u_proj * vec4(in_pos, 0.0, 1.0);
  frag_tex = in_tex;
}

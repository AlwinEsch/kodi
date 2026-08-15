/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#version 450

layout(location = 0) in vec2 frag_cord;

layout(binding = 0) uniform sampler2D u_samp;
layout(binding = 1) uniform UBO
{
  float alpha;
} u_ubo;

layout(location = 0) out vec4 out_frag_color;

void main()
{
  out_frag_color = texture(u_samp, frag_cord);
  out_frag_color.a = u_ubo.alpha;
}

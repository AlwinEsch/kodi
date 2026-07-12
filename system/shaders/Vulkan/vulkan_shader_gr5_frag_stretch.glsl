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
  float stretch;
  float alpha;
} u_ubo;

layout(location = 0) out vec4 out_frag_color;

vec2 stretch(vec2 pos)
{
  // our transform should map [0..1] to itself, with f(0) = 0, f(1) = 1, f(0.5) = 0.5, and f'(0.5) = b.
  // a simple curve to do this is g(x) = b(x-0.5) + (1-b)2^(n-1)(x-0.5)^n + 0.5
  // where the power preserves sign. n = 2 is the simplest non-linear case (required when b != 1)
  float x = pos.x - 0.5;
  return vec2(mix(x * abs(x) * 2.0, x, u_ubo.stretch) + 0.5, pos.y);
}

void main()
{
  out_frag_color.rgb = texture(u_samp, stretch(frag_cord)).rgb;
  out_frag_color.a = u_ubo.alpha;
}

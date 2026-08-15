/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#version 450

layout(constant_id = 0) const int useLimitedColor = 0;

layout(location = 0) in vec2 frag_cord0;
layout(location = 1) in vec2 frag_cord1;

layout (set = 1, binding = 0) uniform sampler2D u_samp0;

layout (location = 0) out vec4 out_color;

void main()
{
  out_color = texture(u_samp0, frag_cord0);
  if (useLimitedColor == 1)
  {
    out_color.rgb *= (235.0-16.0) / 255.0;
    out_color.rgb += 16.0 / 255.0;
  }
}

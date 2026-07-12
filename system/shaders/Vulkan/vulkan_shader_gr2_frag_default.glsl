/*
 *  Copyright (C) 2010-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#version 450

layout(set = 0, binding = 0) uniform sampler2D u_samp0;
layout(set = 0, binding = 1) uniform UBO
{
  vec4 unicol;
  bool limitedRange;
} u_ubo;

layout(location = 0) out vec4 out_color;

void main()
{
  out_color = u_ubo.unicol;
  if (u_ubo.limitedRange)
  {
    // Convert from full range to limited range
    out_color.rgb *= (235.0-16.0) / 255.0;
    out_color.rgb += 16.0 / 255.0;
  }
}

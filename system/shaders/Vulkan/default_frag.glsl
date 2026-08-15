/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#version 450

layout(constant_id = 0) const int useLimitedColor = 0;

layout(push_constant) uniform PushConsts {
  vec4 color;
} pushConsts;

layout (location = 0) out vec4 out_color;

void main()
{
  out_color = pushConsts.color;
  if (useLimitedColor == 1)
  {
    out_color.rgb *= (235.0-16.0) / 255.0;
    out_color.rgb += 16.0 / 255.0;
  }
}

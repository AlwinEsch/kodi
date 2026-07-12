/*
 *  Copyright (C) 2010-2024 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#version 450

layout(location = 0) in vec2 frag_cord0;

layout(binding = 0) uniform sampler2D u_samp0;
layout(binding = 1) uniform UBO
{
  bool limitedRange;
} u_ubo;

layout(location = 0) out vec4 out_color;

// SM_TEXTURE_NOBLEND shader
void main()
{
  out_color = texture(u_samp0, frag_cord0);
  if (u_ubo.limitedRange)
  {
    out_color.rgb *= (235.0-16.0) / 255.0;
    out_color.rgb += 16.0 / 255.0;
  }
}

/*
 *  Copyright (C) 2010-2024 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#version 450

layout(constant_id = 0) const bool KODI_LIMITED_RANGE = false;

layout (set = 1, binding = 0) uniform sampler2D samplerColor;

layout(location = 0) in vec2 frag_cord0;
layout(location = 1) in vec2 frag_cord1;

layout(push_constant) uniform PushConsts {
  vec4 colour;
} pushConsts;

layout (location = 0) out vec4 outFragColor;

void main()
{
  outFragColor = texture(samplerColor, frag_cord0) * pushConsts.colour;
  if (KODI_LIMITED_RANGE)
  {
    outFragColor.rgb *= (235.0-16.0) / 255.0;
    outFragColor.rgb += 16.0 / 255.0;
  }
}

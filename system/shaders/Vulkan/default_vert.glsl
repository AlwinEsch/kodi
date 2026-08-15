/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#version 450

layout(location = 0) in vec3 in_attrpos;

layout (set = 0, binding = 0) uniform UBO
{
  mat4 projectionMatrix;
  mat4 modelMatrix;
} ubo;

out gl_PerVertex
{
  vec4 gl_Position;
};

void main()
{
  gl_Position = ubo.projectionMatrix * ubo.modelMatrix * vec4(in_attrpos.xyz, 1.0);
  gl_Position.z = gl_Position.w;
}

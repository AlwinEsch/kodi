/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#version 450

layout(location = 0) in vec3 in_attrpos;
layout(location = 1) in vec4 in_attrcol;
layout(location = 2) in vec2 in_attrcord0;

layout(set = 0, binding = 0) uniform UBO
{
  mat4 projModelMatrix;
  float depth;
} u_ubo;

layout(push_constant) uniform PushConsts {
  mat4 viewMatrix;
  vec4 shaderClip;
  vec4 cordStep;
} pushConsts;

layout(location = 0) out vec2 frag_cord0;
layout(location = 1) out vec4 frag_color;

// this shader can be used in cases where clipping via vkCmdSetScissor() is not
// possible (e.g. when rotating). it can't discard triangles, but it may
// degenerate them.

void main()
{
  // limit the vertices to the clipping area
  vec4 position = vec4(0., 0., 0., 1.);
  position.xy = clamp(in_attrpos.xy, pushConsts.shaderClip.xy, pushConsts.shaderClip.zw);
  gl_Position = u_ubo.projModelMatrix * pushConsts.viewMatrix * position;

  // set rendering depth
  gl_Position.z = u_ubo.depth * gl_Position.w;

  // correct texture coordinates for clipped vertices
  vec2 clipDist = in_attrpos.xy - position.xy;
  frag_cord0 = in_attrcord0 - clipDist * pushConsts.cordStep.xy;

  frag_color = in_attrcol;
}

/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <stack>
#include <glm/glm.hpp>

#include "system_vulkan.h"

//union
//{
//  T x, r, s;
//};
//union
//{
//  T y, g, t;
//};
//union
//{
//  T z, b, p;
//};
//union
//{
//  T w, a, q;
//};

class TransformMatrix;

class CVulkanMatrix
{
public:
  CVulkanMatrix() = default;

  constexpr CVulkanMatrix(float x0, float x1, float x2, float x3,
                      float x4, float x5, float x6, float x7,
                      float x8, float x9, float x10, float x11,
                      float x12, float x13, float x14, float x15)
    :m_pMatrix{x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15} {}

  CVulkanMatrix(const TransformMatrix &src) noexcept;

  operator const float*() const                { return m_pMatrix; }

  void LoadIdentity();
  void Ortho(float l, float r, float b, float t, float n, float f);
  void Ortho2D(float l, float r, float b, float t);
  void Frustum(float l, float r, float b, float t, float n, float f);
  void Translatef(float x, float y, float z);
  void Scalef(float x, float y, float z);
  void Rotatef(float angle, float x, float y, float z);
  void MultMatrixf(const CVulkanMatrix &matrix) noexcept;
  void LookAt(float eyex, float eyey, float eyez, float centerx, float centery, float centerz, float upx, float upy, float upz);

  static bool Project(float objx,
                      float objy,
                      float objz,
                      const float modelMatrix[16],
                      const float projMatrix[16],
                      const int viewport[4],
                      float* winx,
                      float* winy,
                      float* winz);

private:
  /* alignas(16) allows better SIMD optimizations (e.g. SSE2 benefits
     a lot from this) */
  alignas(16) float m_pMatrix[16];
};

class CVulkanMatrixStack
{
public:
  void Push()
  {
    m_stack.push(m_current);
  }

  void Clear()
  {
    m_stack = std::stack<CVulkanMatrix>();
  }

  void Pop()
  {
    if(!m_stack.empty())
    {
      m_current = m_stack.top();
      m_stack.pop();
    }
  }

  void Load();
  void PopLoad() { Pop(); Load(); }

  CVulkanMatrix& Get()        { return m_current; }
  CVulkanMatrix* operator->() { return &m_current; }

private:
  std::stack<CVulkanMatrix> m_stack;
  CVulkanMatrix             m_current;
};

extern CVulkanMatrixStack vulkanMatrixModview;
extern CVulkanMatrixStack vulkanMatrixProject;
extern CVulkanMatrixStack vulkanMatrixTexture;

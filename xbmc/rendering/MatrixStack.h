/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <stack>

#include <glm/glm.hpp>

namespace KODI::RENDERING
{

/**
 * @brief A stack of transformation matrices.
 */
class CMatrixStack : public glm::mat4
{
public:
  /**
   * @brief Push the current matrix onto the stack.
   */
  void Push() { m_stack.push(*this); }

  /**
   * @brief Pop the top matrix off the stack.
   */
  void Pop()
  {
    if (!m_stack.empty())
    {
      dynamic_cast<glm::mat4&>(*this) = m_stack.top();
      m_stack.pop();
    }
  }

  /**
   * @brief Clear the stack.
   */
  void Clear() { m_stack = std::stack<glm::mat4>(); }

  /**
   * @brief Assign a new matrix to the current matrix.
   *
   * @param mat The new matrix to assign.
   * @return A reference to the current matrix.
   */
  glm::mat4& operator=(const glm::mat4& mat) { return static_cast<glm::mat4&>(*this) = mat; }

  /**
   * @brief Get the current matrix.
   * @return The current matrix.
   */
  glm::mat4& Get() { return *this; }

  /**
   * @brief Get a pointer to the current matrix.
   * @return A pointer to the current matrix.
   */
  glm::mat4* operator->() { return this; }

private:
  std::stack<glm::mat4> m_stack;
};

extern CMatrixStack globalMatrixModview;
extern CMatrixStack globalMatrixProject;
extern CMatrixStack globalMatrixTexture;

} // namespace KODI::RENDERING

/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <memory>

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

struct IRenderHelper
{
  virtual ~IRenderHelper() = default;
  virtual bool Init() = 0;
  virtual void Deinit() = 0;
  virtual void Begin() = 0;
  virtual void End() = 0;
}; /* class IRenderHelper */

struct CRenderHelperStub : public IRenderHelper
{
  CRenderHelperStub(const std::string& clientIdentifier) {}
  bool Init() override { return true; }
  void Deinit() override {}
  void Begin() override {}
  void End() override {}
}; /* class CRenderHelperStub */

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */

#if defined(TARGET_LINUX)
#include "GLViewRenderer.h"
#endif

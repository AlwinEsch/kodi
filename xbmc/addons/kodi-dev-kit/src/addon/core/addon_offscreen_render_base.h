/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

namespace KODI_ADDON
{
namespace INTERNAL
{

class IOffsceenRenderBase
{
public:
  IOffsceenRenderBase() = default;
  virtual ~IOffsceenRenderBase() = default;

  virtual bool Create() = 0;
  virtual void Destroy() = 0;
};

} /* namespace INTERNAL */
} /* namespace KODI_ADDON */

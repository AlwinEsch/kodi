/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "utils/Variant.h"

#include <array>
#include <string>

namespace KODI
{
namespace MESSAGING
{
namespace HELPERS
{

struct DialogCrashReportMessage
{
  std::string addon;
  std::string uuid;
  std::string stacktrace;
};

void ShowReportCrashDialog(std::string addon,
                           std::string uuid,
                           std::string stacktrace);

} // namespace HELPERS
} // namespace MESSAGING
} // namespace KODI


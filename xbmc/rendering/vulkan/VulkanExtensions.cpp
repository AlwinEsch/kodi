/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanExtensions.h"

#include "ServiceBroker.h"
#include "rendering/RenderSystem.h"

#include <ranges>
#include <unordered_set>

bool CVulkanExtensions::IsExtensionSupported(CVulkanExtensions::Extension extension)
{
  static const std::unordered_set<CVulkanExtensions::Extension> supportedExtensions = []()
  {
    auto isExtSupported = [](const auto& tmp)
    { return CServiceBroker::GetRenderSystem()->IsExtSupported(std::string(tmp.second).c_str()); };
    // clang-format off
    auto supportedExtensions = CVulkanExtensions::stringMap | std::views::filter(isExtSupported)
                                                        | std::views::keys;
    // clang-format on
    return std::unordered_set<CVulkanExtensions::Extension>(supportedExtensions.begin(),
                                                            supportedExtensions.end());
  }();

  return supportedExtensions.contains(extension);
}

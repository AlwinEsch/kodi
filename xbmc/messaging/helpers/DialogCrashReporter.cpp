/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DialogCrashReporter.h"

#include "ServiceBroker.h"
#include "messaging/ApplicationMessenger.h"

namespace KODI
{
namespace MESSAGING
{
namespace HELPERS
{

void ShowReportCrashDialog(std::string addon,
                           std::string uuid,
                           std::string stacktrace)
{
  DialogCrashReportMessage* options = new DialogCrashReportMessage;
  options->addon = std::move(addon);
  options->uuid = std::move(uuid);
  options->stacktrace = std::move(stacktrace);

  CServiceBroker::GetAppMessenger()->PostMsg(TMSG_GUI_DIALOG_ADDON_CRASH_REPORT, -1, -1, static_cast<void*>(options));
}

} // namespace HELPERS
} // namespace MESSAGING
} // namespace KODI

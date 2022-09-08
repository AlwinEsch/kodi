/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "addons/IAddon.h"
#include "dialogs/GUIDialogBoxBase.h"
#include "messaging/helpers/DialogCrashReporter.h"

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

class CGUIDialogCrashReporter : public CGUIDialogBoxBase
{
public:
  CGUIDialogCrashReporter(void);
  ~CGUIDialogCrashReporter(void) override = default;

  bool OnMessage(CGUIMessage& message) override;
  bool OnAction(const CAction& action) override;
  bool OnBack(int actionID) override;

  static void ReportCrash(const std::string& addon, const std::string& uuid, const std::string& stacktrace);

  /*!
  \brief Open a add-on crash report dialog and wait for input

  \param[in] options  a struct of type DialogCrashReportMessage containing
  the options to set for this dialog.

  \sa KODI::MESSAGING::HELPERS::DialogCrashReportMessage
  */
  void ReportCrash(const KODI::MESSAGING::HELPERS::DialogCrashReportMessage* options);

private:
  void OnInitWindow() override;

  enum class RESULT
  {
    Canceled = -1,
    IgnoreCrash,
    DisableAddon,
    DisableAddonAndReport,
  };
  RESULT GetResult() const;
  static void SendCrashReport(CGUIComponent* const gui,
                              const ADDON::AddonInfoPtr& addonInfo,
                              const std::string& uuid,
                              const std::string& stacktrace);

  bool m_canceled;
  bool m_disableAddon;
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */

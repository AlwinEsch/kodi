/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "GUIDialogCrashReporter.h"

#include "ServiceBroker.h"
#include "addons/AddonManager.h"
#include "addons/interface/Controller.h"
#include "dialogs/GUIDialogTextViewer.h"
#include "filesystem/File.h"
#include "guilib/GUIComponent.h"
#include "guilib/GUIWindowManager.h"
#include "guilib/LocalizeStrings.h"
#include "input/Key.h"
#include "utils/Variant.h"

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

#define CONTROL_DISABLE_ADDON_BUTTON CONTROL_CUSTOM_BUTTON

CGUIDialogCrashReporter::CGUIDialogCrashReporter()
  : CGUIDialogBoxBase(WINDOW_DIALOG_ADDON_CRASH_REPORTER, "DialogConfirm.xml")
{
}

void CGUIDialogCrashReporter::OnInitWindow()
{
  SET_CONTROL_VISIBLE(CONTROL_DISABLE_ADDON_BUTTON);
  SET_CONTROL_HIDDEN(CONTROL_PROGRESS_BAR);
  SET_CONTROL_FOCUS(CONTROL_YES_BUTTON, 0);

  CGUIDialogBoxBase::OnInitWindow();
}

bool CGUIDialogCrashReporter::OnAction(const CAction& action)
{

  return CGUIDialogBoxBase::OnAction(action);
}

bool CGUIDialogCrashReporter::OnMessage(CGUIMessage& message)
{
  switch (message.GetMessage())
  {
    case GUI_MSG_CLICKED:
    {
      int iControl = message.GetSenderId();
      int iAction = message.GetParam1();
      if (true || ACTION_SELECT_ITEM == iAction)
      {
        if (iControl == CONTROL_NO_BUTTON)
        {
          m_bConfirmed = false;
          Close();
          return true;
        }
        if (iControl == CONTROL_YES_BUTTON)
        {
          m_bConfirmed = true;
          Close();
          return true;
        }
        if (iControl == CONTROL_DISABLE_ADDON_BUTTON)
        {
          m_bConfirmed = false;
          m_disableAddon = true;
          Close();
          return true;
        }
      }
    }
    break;
  }
  return CGUIDialogBoxBase::OnMessage(message);
}

bool CGUIDialogCrashReporter::OnBack(int actionID)
{
  m_canceled = true;
  m_bConfirmed = false;
  m_disableAddon = false;
  return CGUIDialogBoxBase::OnBack(actionID);
}

CGUIDialogCrashReporter::RESULT CGUIDialogCrashReporter::GetResult() const
{
  if (m_canceled)
    return RESULT::Canceled;
  else if (m_disableAddon)
    return RESULT::DisableAddon;
  else if (m_bConfirmed)
    return RESULT::DisableAddonAndReport;
  else
    return RESULT::IgnoreCrash;
}

void CGUIDialogCrashReporter::ReportCrash(const KODI::MESSAGING::HELPERS::DialogCrashReportMessage* options)
{
  ReportCrash(options->addon, options->uuid, options->stacktrace);
  delete options;
}

void CGUIDialogCrashReporter::ReportCrash(const std::string& addon,
                                          const std::string& uuid,
                                          const std::string& stacktrace)
{
  using namespace ADDON;

  AddonInfoPtr addonInfo = CServiceBroker::GetAddonMgr().GetAddonInfo(addon);
  if (!addonInfo)
    return;

  const auto gui = CServiceBroker::GetGUI();
  if (!gui)
    return;

  CGUIDialogCrashReporter* dialog = gui->GetWindowManager().GetWindow<CGUIDialogCrashReporter>(
      WINDOW_DIALOG_ADDON_CRASH_REPORTER);
  if (!dialog)
    return;

  const std::string text = StringUtils::Format(g_localizeStrings.Get(2296), addonInfo->Name());
  dialog->SetHeading(2295);
  dialog->SetText(text);
  dialog->m_canceled = false;
  dialog->m_bConfirmed = false;
  dialog->m_disableAddon = false;
  dialog->SetChoice(0, 2297);
  dialog->SetChoice(1, 2299);
  dialog->SetChoice(2, 2298);
  dialog->Open();
  const RESULT result = dialog->GetResult();
  switch (result)
  {
    case RESULT::Canceled:
    case RESULT::IgnoreCrash:
      break;
    case RESULT::DisableAddonAndReport:
      SendCrashReport(gui, addonInfo, uuid, stacktrace);
      // fallthru is intended
      [[fallthrough]];
    case RESULT::DisableAddon:
      CServiceBroker::GetAddonMgr().DisableAddon(addonInfo->ID(),
                                                 AddonDisabledReason::PERMANENT_FAILURE);
      break;

    default:
      break;
  }
}

void CGUIDialogCrashReporter::SendCrashReport(CGUIComponent* const gui,
                                              const ADDON::AddonInfoPtr& addonInfo,
                                              const std::string& uuid,
                                              const std::string& stacktrace)
{
  using namespace ADDON;
  using namespace XFILE;

  CGUIDialogTextViewer* dialog =
      gui->GetWindowManager().GetWindow<CGUIDialogTextViewer>(WINDOW_DIALOG_TEXT_VIEWER);
  if (!dialog)
    return;

  dialog->SetHeading(g_localizeStrings.Get(2294));
  dialog->UseMonoFont(true);

  if (addonInfo->Type(addonInfo->MainType())->Language() == AddonLanguage::Java)
  {

    const std::string path =
        CServiceBroker::GetAddonIfcCtrl().GetDevkitReportPath() + "/hs_err_pid" + uuid + ".log";
    if (CFile::Exists(path))
    {
      CFile file;
      std::vector<uint8_t> buf;
      if (file.LoadFile(path, buf) > 0)
      {
        const std::string text(reinterpret_cast<char*>(buf.data()), buf.size());
        dialog->SetText(text);
      }
    }
  }
  else if (!stacktrace.empty())
    dialog->SetText(stacktrace);
  else
  {
    dialog->SetText("NEED TODO");
  }

  dialog->Open();
}

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */

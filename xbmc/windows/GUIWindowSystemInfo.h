/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "guilib/GUIWindow.h"
#include "view/GUIViewControl.h"

#include <string>
#include <vector>

class CFileItemList;

class CGUIWindowSystemInfo : public CGUIWindow
{
public:
  CGUIWindowSystemInfo(void);
  ~CGUIWindowSystemInfo(void) override;
  bool OnMessage(CGUIMessage& message) override;
  void FrameMove() override;

protected:
  void OnInitWindow() override;
  void OnWindowLoaded() override;
  void OnDeinitWindow(int nextWindowID) override;
  void OnWindowUnload() override;

private:
  int  m_section;
  void ResetLabels();
  void SetControlLabel(int id, const char *format, int label, int info);
  void UpdateAddonProcessInfos();
  CFileItemPtr FindAddonProcess(const std::string& addonId);

  std::vector<std::string> m_diskUsage;
  CFileItemList* m_addonProcessInfo;
  CGUIViewControl m_viewControl;
};


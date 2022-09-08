/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "../../AddonBase.h"
#include "../../c-api/gui/dialogs/progress.h"

#ifdef __cplusplus

namespace kodi
{
namespace gui
{
namespace dialogs
{

//==============================================================================
/// @defgroup cpp_kodi_gui_dialogs_CProgress Dialog Progress
/// @ingroup cpp_kodi_gui_dialogs
/// @brief @cpp_class{ kodi::gui::dialogs::CProgress }
/// **Progress dialog shown in center**\n
/// The with @ref Progress.h "#include <kodi/gui/dialogs/Progress.h>"
/// given class are basically used to create Kodi's progress dialog with named
/// text fields.
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/gui/dialogs/Progress.h>
///
/// kodi::gui::dialogs::CProgress *progress = new kodi::gui::dialogs::CProgress;
/// progress->SetHeading("Test progress");
/// progress->SetLine(1, "line 1");
/// progress->SetLine(2, "line 2");
/// progress->SetLine(3, "line 3");
/// progress->SetCanCancel(true);
/// progress->ShowProgressBar(true);
/// progress->Open();
/// for (unsigned int i = 0; i < 100; i += 10)
/// {
///   progress->SetPercentage(i);
///   sleep(1);
/// }
/// delete progress;
/// ~~~~~~~~~~~~~
///
class ATTR_DLL_LOCAL CProgress
{
public:
  //============================================================================
  /// @ingroup cpp_kodi_gui_dialogs_CProgress
  /// @brief Construct a new dialog
  ///
  CProgress()
  {
    m_DialogHandle = kodi::dl::api.kodi_gui_dialogs_progress_new_dialog();
    if (!m_DialogHandle)
      kodi::Log(ADDON_LOG_FATAL,
                "kodi::gui::dialogs::CProgress can't create window class from Kodi !!!");
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_dialogs_CProgress
  /// @brief Destructor
  ///
  ~CProgress()
  {
    if (m_DialogHandle)
      kodi::dl::api.kodi_gui_dialogs_progress_delete_dialog(m_DialogHandle);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_dialogs_CProgress
  /// @brief To open the dialog
  ///
  void Open() { kodi::dl::api.kodi_gui_dialogs_progress_open(m_DialogHandle); }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_dialogs_CProgress
  /// @brief Set the heading title of dialog
  ///
  /// @param[in] heading Title string to use
  ///
  void SetHeading(const std::string& heading)
  {
    kodi::dl::api.kodi_gui_dialogs_progress_set_heading(m_DialogHandle, heading.c_str());
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_dialogs_CProgress
  /// @brief To set the line text field on dialog from 0 - 2
  ///
  /// @param[in] iLine Line number
  /// @param[in] line Text string
  ///
  void SetLine(unsigned int iLine, const std::string& line)
  {
    kodi::dl::api.kodi_gui_dialogs_progress_set_line(m_DialogHandle, iLine, line.c_str());
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_dialogs_CProgress
  /// @brief To enable and show cancel button on dialog
  ///
  /// @param[in] canCancel if true becomes it shown
  ///
  void SetCanCancel(bool canCancel)
  {
    kodi::dl::api.kodi_gui_dialogs_progress_set_can_cancel(m_DialogHandle, canCancel);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_dialogs_CProgress
  /// @brief To check dialog for clicked cancel button
  ///
  /// @return True if canceled
  ///
  bool IsCanceled() const
  {
    return kodi::dl::api.kodi_gui_dialogs_progress_is_canceled(m_DialogHandle);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_dialogs_CProgress
  /// @brief Get the current progress position as percent
  ///
  /// @param[in] percentage Position to use from 0 to 100
  ///
  void SetPercentage(int percentage)
  {
    kodi::dl::api.kodi_gui_dialogs_progress_set_percentage(m_DialogHandle, percentage);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_dialogs_CProgress
  /// @brief To set the current progress position as percent
  ///
  /// @return Current Position used from 0 to 100
  ///
  int GetPercentage() const
  {
    return kodi::dl::api.kodi_gui_dialogs_progress_get_percentage(m_DialogHandle);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_dialogs_CProgress
  /// @brief To show or hide progress bar dialog
  ///
  /// @param[in] onOff If true becomes it shown
  ///
  void ShowProgressBar(bool onOff)
  {
    kodi::dl::api.kodi_gui_dialogs_progress_show_progress_bar(m_DialogHandle, onOff);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_dialogs_CProgress
  /// @brief Set the maximum position of progress, needed if `SetProgressAdvance(...)` is used
  ///
  /// @param[in] max Biggest usable position to use
  ///
  void SetProgressMax(int max)
  {
    kodi::dl::api.kodi_gui_dialogs_progress_set_progress_max(m_DialogHandle, max);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_dialogs_CProgress
  /// @brief To increase progress bar by defined step size until reach of maximum position
  ///
  /// @param[in] steps Step size to increase, default is 1
  ///
  void SetProgressAdvance(int steps = 1)
  {
    kodi::dl::api.kodi_gui_dialogs_progress_set_progress_advance(m_DialogHandle, steps);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_dialogs_CProgress
  /// @brief To check progress was canceled on work
  ///
  /// @return True if aborted
  ///
  bool Abort() { return kodi::dl::api.kodi_gui_dialogs_progress_abort(m_DialogHandle); }
  //----------------------------------------------------------------------------

private:
  KODI_GUI_HANDLE m_DialogHandle;
};

} /* namespace dialogs */
} /* namespace gui */
} /* namespace kodi */

#endif /* __cplusplus */

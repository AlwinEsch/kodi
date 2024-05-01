/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "../../c-api/gui/input/action.h"
#include "ActionIDs.h"

#ifdef __cplusplus

#include <string>

namespace kodi
{
namespace gui
{

class CWindow;

namespace input
{

//==============================================================================
/// @addtogroup cpp_kodi_gui_input_action
/// @brief @cpp_class{ kodi::gui::CAction }
/// **Action signal class**\n
/// Class e to manage given actions calls from Kodi.
///
/// This used by @ref kodi::gui::CWindow::OnAction() and partly on
/// other parts.
///
///
/// --------------------------------------------------------------------------
///
///
class ATTR_DLL_LOCAL CAction
{
public:
  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Constructor about joystick analog signals.
  ///
  /// @param[in] actionID ID to use, see @ref ADDON_ACTION about possible values
  /// @param[in] amount1 [optional]
  /// @param[in] amount2 [optional]
  /// @param[in] name [optional] Action name to set
  /// @param[in] holdTime [optional] Time how long the key was pressed
  /// @param[in] buttonCode [optional]
  ///
  CAction(ADDON_ACTION actionID,
          float amount1 = 1.0f,
          float amount2 = 0.0f,
          const std::string& name = "",
          unsigned int holdTime = 0,
          unsigned int buttonCode = 0)
    : m_id(actionID), m_name(name), m_holdTime(holdTime), m_buttonCode(buttonCode)
  {
    m_amount[0] = amount1;
    m_amount[1] = amount2;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Constructor about joystick analog signals.
  ///
  /// @param[in] actionID ID to use, see @ref ADDON_ACTION about possible values
  /// @param[in] state
  /// @param[in] posX
  /// @param[in] posY
  /// @param[in] offsetX
  /// @param[in] offsetY
  /// @param[in] velocityX [optional]
  /// @param[in] velocityY [optional]
  /// @param[in] name [optional] Action name to set
  ///
  CAction(ADDON_ACTION actionID,
          unsigned int state,
          float posX,
          float posY,
          float offsetX,
          float offsetY,
          float velocityX = 0.0f,
          float velocityY = 0.0f,
          const std::string& name = "")
    : m_id(actionID), m_name(name), m_holdTime(state)
  {
    m_amount[0] = posX;
    m_amount[1] = posY;
    m_amount[2] = offsetX;
    m_amount[3] = offsetY;
    m_amount[4] = velocityX;
    m_amount[5] = velocityY;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Constructor for unicode date e.g. from Keyboard.
  ///
  /// @param[in] actionID ID to use, see @ref ADDON_ACTION about possible values
  /// @param[in] unicode Unicode character to give on construction
  ///
  CAction(ADDON_ACTION actionID, wchar_t unicode) : m_id(actionID), m_unicode(unicode) {}
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Constructor with set also used name.
  ///
  /// @param[in] actionID ID to use, see @ref ADDON_ACTION about possible values
  /// @param[in] name Action name to set
  ///
  CAction(ADDON_ACTION actionID, const std::string& name) : m_id(actionID), m_name(name) {}
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Clone constructor.
  ///
  CAction(const CAction& other) { *this = other; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Copy class to another.
  ///
  /// @param[in] rhs
  ///
  CAction& operator=(const CAction& rhs)
  {
    if (this != &rhs)
    {
      m_id = rhs.m_id;
      for (unsigned int i = 0; i < KODI_GUI_ACTION_MAX_AMOUNTS; i++)
        m_amount[i] = rhs.m_amount[i];
      m_name = rhs.m_name;
      m_repeat = rhs.m_repeat;
      m_buttonCode = rhs.m_buttonCode;
      m_unicode = rhs.m_unicode;
      m_holdTime = rhs.m_holdTime;
      m_text = rhs.m_text;
    }
    return *this;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Identifier of the action
  ///
  /// @return id of the action
  ///
  int GetID() const { return m_id; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Is this an action from the mouse
  ///
  /// @return true if this is a mouse action, false otherwise
  ///
  bool IsMouse() const
  {
    return (m_id >= ADDON_ACTION_MOUSE_START && m_id <= ADDON_ACTION_MOUSE_END);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Is this an action from the gesture
  ///
  /// @return True if this is a gesture action, false otherwise
  ///
  bool IsGesture() const
  {
    return (m_id >= ADDON_ACTION_GESTURE_NOTIFY && m_id <= ADDON_ACTION_GESTURE_END);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Human-readable name of the action
  ///
  /// @return Name of the action
  ///
  const std::string& GetName() const { return m_name; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Text of the action if any
  ///
  /// @return Text payload of this action.
  ///
  const std::string& GetText() const { return m_text; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Set the text payload of the action
  ///
  /// @param[in] text To be set
  ///
  void SetText(const std::string& text) { m_text = text; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Get an amount associated with this action
  ///
  /// @param[in] zero-based index of amount to retrieve, defaults to 0
  /// @return An amount associated with this action
  ///
  float GetAmount(unsigned int index = 0) const
  {
    return (index < KODI_GUI_ACTION_MAX_AMOUNTS) ? m_amount[index] : 0;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Unicode value associated with this action
  ///
  /// @return Unicode value associated with this action, for keyboard input.
  ///
  wchar_t GetUnicode() const { return m_unicode; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Time in ms that the key has been held
  ///
  /// @return Time that the key has been held down in ms.
  ///
  unsigned int GetHoldTime() const { return m_holdTime; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Time since last repeat in ms
  ///
  /// @return Time since last repeat in ms. Returns 0 if unknown.
  ///
  float GetRepeat() const { return m_repeat; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Button code that triggered this action
  ///
  /// @return Button code
  ///
  unsigned int GetButtonCode() const { return m_buttonCode; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_gui_input_action
  /// @brief Checks action id is from an analog action control.
  ///
  /// @param[in] actionID ID to use, see @ref ADDON_ACTION about possible values
  /// @return True if analog action, false otherwise
  ///
  bool IsAnalog(ADDON_ACTION actionID) const
  {
    switch (actionID)
    {
      case ADDON_ACTION_ANALOG_SEEK_FORWARD:
      case ADDON_ACTION_ANALOG_SEEK_BACK:
      case ADDON_ACTION_SCROLL_UP:
      case ADDON_ACTION_SCROLL_DOWN:
      case ADDON_ACTION_ANALOG_FORWARD:
      case ADDON_ACTION_ANALOG_REWIND:
      case ADDON_ACTION_ANALOG_MOVE:
      case ADDON_ACTION_ANALOG_MOVE_X_LEFT:
      case ADDON_ACTION_ANALOG_MOVE_X_RIGHT:
      case ADDON_ACTION_ANALOG_MOVE_Y_UP:
      case ADDON_ACTION_ANALOG_MOVE_Y_DOWN:
      case ADDON_ACTION_CURSOR_LEFT:
      case ADDON_ACTION_CURSOR_RIGHT:
      case ADDON_ACTION_VOLUME_UP:
      case ADDON_ACTION_VOLUME_DOWN:
      case ADDON_ACTION_ZOOM_IN:
      case ADDON_ACTION_ZOOM_OUT:
        return true;
      default:
        return false;
    }
  }
  //----------------------------------------------------------------------------

private:
  friend class kodi::gui::CWindow;

  CAction(const struct kodi_gui_action* action)
  {
    m_id = static_cast<ADDON_ACTION>(action->id);
    m_name = action->name ? action->name : "";
    for (size_t i = 0; i < KODI_GUI_ACTION_MAX_AMOUNTS; ++i)
      m_amount[i] = action->amount[i];
    m_repeat = action->repeat;
    m_holdTime = action->hold_time;
    m_buttonCode = action->button_code;
    m_unicode = action->unicode;
    m_text = action->text ? action->text : "";
    m_actionHdl = action->action_hdl;
  }

  ADDON_ACTION m_id;
  std::string m_name;

  float m_amount[KODI_GUI_ACTION_MAX_AMOUNTS] = {};

  float m_repeat = 0.0f;
  unsigned int m_holdTime = 0;
  unsigned int m_buttonCode = 0;
  wchar_t m_unicode = 0;
  std::string m_text;

  KODI_GUI_ACTION_HANDLE m_actionHdl = nullptr;
};

} /* namespace input */
} /* namespace gui */
} /* namespace kodi */

#endif /* __cplusplus */

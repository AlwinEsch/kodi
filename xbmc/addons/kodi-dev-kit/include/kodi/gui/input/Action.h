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

namespace addon
{
  class CInstanceWeb;
} /* namespace addon */

namespace gui
{

class CWindow;

namespace input
{

/// @defgroup cpp_kodi_gui_Defs_CAction class CAction
/// @ingroup cpp_kodi_gui_Defs
/// @brief **Action signal class**\n
/// Class e to manage given actions calls from Kodi.
///
/// This used by @ref kodi::gui::CWindow::OnAction() and partly on
/// other parts.
///
///@{
class CAction
{
public:
  //============================================================================
  /// @brief Default constructor with values set to default/empty.
  ///
  CAction() = default;
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Constructor for unicode date e.g. from Keyboard.
  ///
  /// @param[in] actionID ID to use, see @ref ADDON_ACTION about possible values
  /// @param[in] unicode Unicode character to give on construction
  ///
  CAction(ADDON_ACTION actionID, wchar_t unicode)
    : m_id(actionID),
      m_unicode(unicode)
  {
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Constructor with set also used name.
  ///
  /// @param[in] actionID ID to use, see @ref ADDON_ACTION about possible values
  /// @param[in] name Action name to set
  ///
  CAction(ADDON_ACTION actionID, const std::string& name)
    : m_id(actionID),
      m_name(name)
  {
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Constructor about joystick analog signals.
  ///
  /// @param[in] actionID ID to use, see @ref ADDON_ACTION about possible values
  /// @param[in] amount1
  /// @param[in] amount2
  /// @param[in] name ]optional] Action name to set
  /// @param[in] holdTime ]optional] Time how long the key was pressed
  ///
  CAction(ADDON_ACTION actionID,
          float amount1 = 1.0f,
          float amount2 = 0.0f,
          const std::string& name = "",
          unsigned int holdTime = 0)
    : m_id(actionID),
      m_name(name),
      m_holdTime(holdTime)
  {
    m_amount[0] = amount1;
    m_amount[1] = amount2;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Constructor about joystick analog signals.
  ///
  /// @param[in] actionID ID to use, see @ref ADDON_ACTION about possible values
  /// @param[in] state
  /// @param[in] posX
  /// @param[in] posY
  /// @param[in] offsetX
  /// @param[in] offsetY
  /// @param[in] posX
  /// @param[in] velocityX
  /// @param[in] velocityY
  /// @param[in] name ]optional] Action name to set
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
    : m_id(actionID),
      m_name(name),
      m_holdTime(state)
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
  /// @brief Clone constructor.
  ///
  CAction(const CAction& other) { *this = other; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Copy class to another.
  ///
  ///
  CAction& operator=(const CAction& rhs)
  {
    if (this != &rhs)
    {
      m_id = rhs.m_id;
      for (unsigned int i = 0; i < ADDON_ACTION_AMOUNT_MAX; i++)
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
  /// @brief Identifier of the action.
  ///
  /// @return id of the action
  ///
  ADDON_ACTION GetID() const { return m_id; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Is this an action from the mouse.
  ///
  /// @return true if this is a mouse action, false otherwise
  ///
  bool IsMouse() const
  {
    return (m_id >= ADDON_ACTION_MOUSE_START && m_id <= ADDON_ACTION_MOUSE_END);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Check action is gesture signal.
  ///
  /// @return True if gesture, false otherwise
  ///
  bool IsGesture() const
  {
    return (m_id >= ADDON_ACTION_GESTURE_NOTIFY && m_id <= ADDON_ACTION_GESTURE_END);
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Check action is analog signal.
  ///
  /// @return True if analog, false otherwise
  ///
  bool IsAnalog() const
  {
    switch (m_id)
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

  //============================================================================
  /// @brief Human-readable name of the action.
  ///
  /// @return name of the action
  ///
  const std::string& GetName() const { return m_name; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Text of the action if any.
  ///
  /// @return text payload of this action.
  ///
  const std::string& GetText() const { return m_text; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Set the text payload of the action.
  ///
  /// @param text to be set
  ///
  void SetText(const std::string& text) { m_text = text; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Get an amount associated with this action.
  ///
  /// @param[in] index [optional] index of amount to retrieve, defaults to 0
  /// @return an amount associated with this action
  ///
  float GetAmount(unsigned int index = 0) const
  {
    return (index < ADDON_ACTION_AMOUNT_MAX) ? m_amount[index] : 0;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Reset all amount values to zero.
  ///
  void ClearAmount()
  {
    for (float& amount : m_amount)
      amount = 0;
  }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Unicode value associated with this action.
  ///
  /// @return unicode value associated with this action, for keyboard input.
  ///
  wchar_t GetUnicode() const { return m_unicode; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Time in ms that the key has been held.
  ///
  /// @return time that the key has been held down in ms.
  ///
  unsigned int GetHoldTime() const { return m_holdTime; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Time since last repeat in ms.
  ///
  /// @return time since last repeat in ms. Returns 0 if unknown.
  ///
  float GetRepeat() const { return m_repeat; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief Button code that triggered this action.
  ///
  /// @return button code
  ///
  unsigned int GetButtonCode() const { return m_buttonCode; }
  //----------------------------------------------------------------------------

private:
  friend class kodi::gui::CWindow;
  friend class kodi::addon::CInstanceWeb;

  CAction(const addon_action_data* addonData)
  {
    if (!addonData)
      return;

    m_id = addonData->id;
    m_name = addonData->name;
    m_repeat = addonData->repeat;
    m_holdTime = addonData->hold_time;
    m_buttonCode = addonData->button_code;
    m_unicode = addonData->unicode;
    m_text = addonData->text;
    for (unsigned int i = 0; i < ADDON_ACTION_AMOUNT_MAX; ++i)
      m_amount[i] = addonData->amount[i];
  }

  ADDON_ACTION m_id = ADDON_ACTION_NONE;
  std::string m_name;

  float m_amount[ADDON_ACTION_AMOUNT_MAX] = {};

  float m_repeat = 0.0f;
  unsigned int m_holdTime = 0;
  unsigned int m_buttonCode = 0;
  wchar_t m_unicode = 0;
  std::string m_text;
};
///@}

} /* namespace input */
} /* namespace gui */
} /* namespace kodi */

#endif /* __cplusplus */

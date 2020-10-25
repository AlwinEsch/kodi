/*
 *  Copyright (C) 2016-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "addons/kodi-dev-kit/include/kodi/c-api/addon-instance/web.h"
#include "input/keyboard/KeyboardTypes.h"

namespace KODI
{
namespace WEB
{

/*!
 * \ingroup games
 * \brief Translates data types from web API to the corresponding format in Kodi.
 *
 * This class is stateless.
 */
class CWebAddonTranslator
{
  CWebAddonTranslator() = delete;

public:
  /*!
   * \brief Translates web errors to string representation (e.g. for logging).
   * \param error The error to translate.
   * \return Translated error.
   */
  static const char* ToString(WEB_ADDON_ERROR error);

  /*!
   * \brief Translate key modifiers (Kodi to web API).
   * \param modifiers The key modifiers to translate (e.g. Shift, Ctrl).
   * \return Translated key modifiers.
   */
  static WEB_KEY_MOD GetModifiers(KEYBOARD::Modifier modifier);
};

} /* namespace WEB */
} /* namespace KODI */

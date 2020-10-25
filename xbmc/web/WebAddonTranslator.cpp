/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "WebAddonTranslator.h"

using namespace KODI::WEB;

const char* CWebAddonTranslator::ToString(WEB_ADDON_ERROR error)
{
  switch (error)
  {
    case WEB_ADDON_ERROR_NO_ERROR:
      return "no error";
    case WEB_ADDON_ERROR_NO_ERROR_REOPEN:
      return "no error reopen needed";
    case WEB_ADDON_ERROR_UNKNOWN:
      return "unknown error";
    case WEB_ADDON_ERROR_NOT_IMPLEMENTED:
      return "not implemented";
    case WEB_ADDON_ERROR_REJECTED:
      return "rejected by the client";
    case WEB_ADDON_ERROR_INVALID_PARAMETERS:
      return "invalid parameters for this method";
    case WEB_ADDON_ERROR_FAILED:
      return "the command failed";
    default:
      break;
  }
  return "unknown error";
}

WEB_KEY_MOD CWebAddonTranslator::GetModifiers(KEYBOARD::Modifier modifier)
{
  using namespace KEYBOARD;

  unsigned int mods = WEB_KEY_MOD_NONE;

  if (modifier & Modifier::MODIFIER_CTRL)
    mods |= WEB_KEY_MOD_CTRL;
  if (modifier & Modifier::MODIFIER_SHIFT)
    mods |= WEB_KEY_MOD_SHIFT;
  if (modifier & Modifier::MODIFIER_ALT)
    mods |= WEB_KEY_MOD_ALT;
  if (modifier & Modifier::MODIFIER_RALT)
    mods |= WEB_KEY_MOD_ALT;
  if (modifier & Modifier::MODIFIER_META)
    mods |= WEB_KEY_MOD_META;
  if (modifier & Modifier::MODIFIER_SUPER)
    mods |= WEB_KEY_MOD_SUPER;
  if (modifier & Modifier::MODIFIER_NUMLOCK)
    mods |= WEB_KEY_MOD_NUMLOCK;
  if (modifier & Modifier::MODIFIER_CAPSLOCK)
    mods |= WEB_KEY_MOD_CAPSLOCK;
  if (modifier & Modifier::MODIFIER_SCROLLLOCK)
    mods |= WEB_KEY_MOD_SCROLLOCK;

  return static_cast<WEB_KEY_MOD>(mods);
}

/*
 *  Copyright (C) 2012-2013 Team XBMC
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include <android/input.h>

#include "AndroidJoyStick.h"
#include "platform/android/activity/XBMCApp.h"

bool CAndroidJoyStick::onJoyStickEvent(AInputEvent* event)
{
  int32_t source = AInputEvent_getSource(event);

  // only handle input events from a gamepad or joystick
  if ((source & (AINPUT_SOURCE_GAMEPAD | AINPUT_SOURCE_JOYSTICK)) != 0)
    return CXBMCApp::onInputDeviceEvent(event);

  CXBMCApp::android_printf("CAndroidJoyStick::onJoyStickEvent(type = %d, keycode = %d, source = %d): ignoring non-joystick input event",
                           AInputEvent_getType(event), AKeyEvent_getKeyCode(event), source);
  return false;
}

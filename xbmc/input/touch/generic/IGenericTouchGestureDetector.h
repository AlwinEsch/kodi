/*
 *  Copyright (C) 2013 Team XBMC
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <array>

#include "input/touch/ITouchInputHandling.h"
#include "input/touch/TouchTypes.h"

/*!
 * \ingroup touch_generic
 * \brief Interface defining methods to perform gesture recognition
 */
class IGenericTouchGestureDetector : public ITouchInputHandling
{
public:
  IGenericTouchGestureDetector(ITouchActionHandler *handler, float dpi)
    : m_done(false),
      m_dpi(dpi)
  {
    RegisterHandler(handler);
  }
  ~IGenericTouchGestureDetector() override = default;
  static constexpr int MAX_POINTERS = 2;

  /*!
   * \brief Check whether the gesture recognition is finished or not
   *
   * \return True if the gesture recognition is finished otherwise false
   */
  bool IsDone() { return m_done; }

  /*!
   * \brief A new touch pointer has been recognised.
   *
   * \param index     Index of the given touch pointer
   * \param pointer   Touch pointer that has changed
   *
   * \return True if the event was handled otherwise false
   */
  virtual bool OnTouchDown(unsigned int index, const Pointer &pointer) = 0;
  /*!
   * \brief An active touch pointer has vanished.
   *
   * If the first touch pointer is lifted and there are more active touch
   * pointers, the remaining pointers change their index.
   *
   * \param index     Index of the given touch pointer
   * \param pointer   Touch pointer that has changed
   *
   * \return True if the event was handled otherwise false
   */
  virtual bool OnTouchUp(unsigned int index, const Pointer &pointer) { return false; }
  /*!
   * \brief An active touch pointer has moved.
   *
   * \param index     Index of the given touch pointer
   * \param pointer   Touch pointer that has changed
   *
   * \return True if the event was handled otherwise false
   */
  virtual bool OnTouchMove(unsigned int index, const Pointer &pointer) { return false; }
  /*!
   * \brief An active touch pointer's values have been updated but no event has
   *        occured.
   *
   * \param index     Index of the given touch pointer
   * \param pointer   Touch pointer that has changed
   *
   * \return True if the event was handled otherwise false
   */
  virtual bool OnTouchUpdate(unsigned int index, const Pointer &pointer) { return false; }

protected:
  /*!
   * \brief Whether the gesture recognition is finished or not
   */
  bool m_done;
  /*!
   * \brief DPI value of the touch screen
   */
  float m_dpi;
  /*!
   * \brief Local list of all known touch pointers
   */
  std::array<Pointer, MAX_POINTERS> m_pointers;
};

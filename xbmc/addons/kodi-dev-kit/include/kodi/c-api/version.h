/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_VERSION_H
#define C_API_VERSION_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * Major version of this KDK.
 *
 * For example: 1 for r1.
 */
#define __KDK_MAJOR__ 1

/**
 * Minor version of this KDK.
 *
 * For example: 0 for r1 and 1 for r1b.
 */
#define __KDK_MINOR__ 0

/**
 * Set to 0 if this is a release build, or 1 for beta 1,
 * 2 for beta 2, and so on.
 */
#define __KDK_BETA__ 0

/**
 * Set to 1 if this is a canary build, 0 if not.
 */
#define __KDK_CANARY__ 1

#define __KODI_API_FUTURE__ 10000

#ifndef __KODI_API__
#define __KODI_API__ __KODI_API_FUTURE__
#endif

#define __KODI_API_1__ 1

#ifndef __INTRODUCED_IN_KODI
#define __INTRODUCED_IN_KODI(api_level)
#endif
#ifndef __DEPRECATED_IN_KODI
#define __DEPRECATED_IN_KODI(api_level)
#endif
#ifndef __REMOVED_IN_KODI
#define __REMOVED_IN_KODI(api_level)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !C_API_VERSION_H */

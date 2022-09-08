/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "SharedData.h"

#if HAS_GL
#define GL_TYPE_STRING "GL"
// always define GL_GLEXT_PROTOTYPES before include gl headers
#if !defined(GL_GLEXT_PROTOTYPES)
#define GL_GLEXT_PROTOTYPES
#endif
#if defined(TARGET_LINUX)
#include <GL/gl.h>
#include <GL/glext.h>
#elif defined(TARGET_FREEBSD)
#include <GL/gl.h>
#elif defined(TARGET_DARWIN)
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#elif defined(WIN32)
#error Use of GL under Windows is not possible
#endif
#elif HAS_GLES >= 2
#define GL_TYPE_STRING "GLES"
#if defined(WIN32)
#if defined(HAS_ANGLE)
#include <angle_gl.h>
#else
#error Use of GLES only be available under Windows by the use of angle
#endif
#elif defined(TARGET_DARWIN)
#if HAS_GLES == 3
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#else
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif
#else
#if HAS_GLES == 3
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif
#endif
#endif

#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>

struct texture_storage_metadata_t
{
  int width;
  int height;
  int fourcc;
  EGLint offset;
  EGLint stride;
  int num_planes;
  EGLuint64KHR modifiers;
};

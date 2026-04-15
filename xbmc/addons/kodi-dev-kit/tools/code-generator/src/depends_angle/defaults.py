# -*- coding: utf-8 -*-

#   Copyright (C) 2026 Team Kodi
#   This file is part of Kodi - https://kodi.tv
#
#   SPDX-License-Identifier: GPL-2.0-or-later
#   See LICENSES/README.md for more information.

# Global includes
import os

# Default values
class defaults:
    '''
    Class for default values to use on generate
    '''
    
    OWN_BASE_NAME = "kodi"
    '''
    The name where becomes used for branches by git.
    E.g. original Angle use /chromium/7258 with with provider name on begin

    Can be becomes changed by use of -N YOUR_NAME command line value.
    '''

    BUILD_DIRECTORY = os.path.join(os.path.dirname(os.path.dirname(os.getcwd())), "build", "angle")
    '''
    Global code generator value
    Directory where becomes used about works and builds on the code generator calls

    Can be becomes changed by use of --build-path command line value.
    '''

    SCRIPT_DIRECTORY = os.path.join(os.getcwd(), "src", "depends_angle")
    '''
    Directory where the startet script has his submodules
    '''

    CURRENT_DEFAULT_VERSION = "7258" # Must be match to present a branch on GIT_URL_REMOTE_UPSTREAM_ORIGIN
    '''
    Default version of ANGLE where becomes used, recommended to use command line
    value --version.

    NOTE: Update version here from time to time to match the active used one on
    add-ons.
    '''

    GIT_URL_REMOTE_UPSTREAM = "https://github.com/AlwinEsch/angle-kodi.git"
    '''
    Default Own CLang related ANGLE main repository, possible to use command line value --url to set 
    for own repo.
    '''

    GIT_URL_REMOTE_UPSTREAM_CHROMIUM = "https://github.com/google/angle.git" # Mirror from "https://chromium.googlesource.com/angle/angle"
    '''
    Default Chromium ANGLE repository where becomes used as source for our own one, possible to use
    command line value --remote to set for another one.
    '''

    GITIGNORE_ALLOW_PARTS = { '/third_party/zlib' }
    '''
    Parts where we allow to add as parts into our modified angle.

    Edit and add parts to here if Angle includes in newer version needed parts
    '''

    GITIGNORE_IGNORE_PARTS = { 'build/' }
    '''
    Add new parts where we not want to add in own git commits
    '''

    WEBKIT_BUILDSYSTEM_COMMIT = "0742522b24152262b04913242cb0b3c48de92ba0"
    '''
    Commit checksum to script where downloaded from URL defined with WEBKIT_GNI_TO_CMAKE_URL.
    '''

    DEPENDS_ANGLE_FETCH_DATE_INTERVAL = 86400 # One day
    '''
    The time intervall to check upstreams about code changes.
    '''

    WEBKIT_FILES = [ 
        # Path from source to get
        "https://github.com/WebKit/WebKit/raw/{}".format(WEBKIT_BUILDSYSTEM_COMMIT),

        # URL to from WebKit used script to generate from ninja makefiles the for us needed CMake makefiles.
        #
        # NOTE: If "gni-to-cmake.py" in folder "kodi-dev-kit/tools/code-generator/src/devkit" present becomes no download done!
        [ "/Source/ThirdParty/ANGLE/gni-to-cmake.py", "cf9dab1b59cd44b9ce05a2dc4636115e770dfe7d5cfbd90c3fef659628ac8155c57b866480f7cfe9a0afeb31ff5ce5eed74473f386120a7fc5910e8b032bd61d" ],

        # Parts from WebKit where we need in our Angle
        [ 
            [ "/Source/ThirdParty/ANGLE/include/CMakeLists.txt", "/include/CMakeLists.txt", "a7ddf3c6df7565e232f87ec651cc4fd84240b8866609e23e3e6e41d22532fd34c70e0f3b06120fd3d6d930ca29c1d0d470d4c8cb7003a66f8c1a840a42f32949" ],
        ],
    ]
    '''
    Fetch additional files from WebKit ANGLE buildsystem
    
    First value base url from where values taken.
    Second value defines the used gni-to-cmake.py
    Third value splitted in three parts:
        1. Path from source to get
        2. Path to where it is stored
        3. SHA512 hash to check
    '''

    THIRDPARTY_ZLIB = {
        "URL":      "https://chromium.googlesource.com/chromium/src/third_party/zlib",
        "SHA256":   "b80f1d1e5256ac25f6aea3f31f13d458981cb1f9"
    }
    '''
    URL to from chromium modified zlib version.

    NOTE: git clone required!

    Values splitted in two parts:
        1. URL from source to get
        2. SHA256 to check file
    '''

    ANGLE_FILES_TO_REMOVE = [ 
        '.gitmodules' 
    ]
    '''
    Files to remove from from basic Angle to match our used Angle.
    '''

    ROOT_GNI_FILES_TO_CONVERT = [
      [ "compiler.gni", "Compiler.cmake" ],
      [ "libGLESv2.gni", "GLESv2.cmake" ],
    ]
    '''
    '''

    RENDERER_GN_FILES_TO_CONVERT = [
      [ "libANGLE/renderer/d3d/BUILD.gn", "D3D.cmake" ],
      [ "libANGLE/renderer/gl/BUILD.gn", "GL.cmake" ],
      [ "libANGLE/renderer/metal/BUILD.gn", "Metal.cmake" ],
    ]
    '''
    '''

   

#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#   Copyright (C) 2026 Team Kodi
#   This file is part of Kodi - https://kodi.tv
#
#   SPDX-License-Identifier: GPL-2.0-or-later
#   See LICENSES/README.md for more information.

# Global includes
from email.policy import default
from optparse import OptionParser
import configparser, os

# Own includes
from src.depends_angle.defaults import defaults
from src.depends_angle.settings import settings
from src.depends_angle.sourceGITClone import AngleSetupOriginalSourceCode as SetupSource
from src.depends_angle.sourceChangeAngleFiles import AngleSetupFilesChange as SetupFilesChange
from src.depends_angle.sourceDownloadAdditionalFiles import DownloadAdditionalFiles as SetupDownloadFiles
from src.depends_angle.sourceGenerateCMake import AngleSetupCMake as SetupCMake
from src.depends_angle.sourcePatchFiles import AngleApplyPatches as SetupApplyPatches
from src.helper_Log import Log

def GenerateParts(settings):
    options = settings.options

    # Small check to prevent set of uppest disk folder "/" about POSIX, "< 3" about Windows "e.g. C:\"
    if (True if options.buildpath == "/" else False) or \
        options.buildpath.startswith("/") == False and len(options.buildpath) <= (4 if options.buildpath.endswith("\\") else 3):
        Log.PrintFatal("The specified build directory \"{}\" not allowed to use on uppest disk folder!".format(options.buildpath))
        return False
    if (True if options.sourcepath == "/" else False) or \
        options.sourcepath.startswith("/") == False and len(options.sourcepath) <= (4 if options.sourcepath.endswith("\\") else 3):
        Log.PrintFatal("The mandatory source path with \"{}\" not allowed to use on uppest disk folder!".format(options.sourcepath))
        return False

    # Create build directories if not already present
    if not os.path.exists(options.buildpath):
        os.makedirs(options.buildpath)
    if not os.path.exists(options.sourcepath):
        os.makedirs(options.sourcepath)

    if not SetupSource(settings).Process():
        return False
    if not SetupFilesChange(settings).Process():
        return False
    if not SetupDownloadFiles(settings).Process():
        return False
    if not SetupCMake(settings).Process():
        return False
    if not SetupApplyPatches(settings).Process():
        return False

    Log.Init(options)
    Log.PrintMainStartBegin("Auto generation finished")
    Log.PrintTextLine("Check generated parts are correct, build works and usable on add-ons.")
    Log.PrintMainStartEnd()

    return True


# ===============================================================================
if __name__ == "__main__":
    # parse command-line options
    disc = """\
This utility autogenerate a to CMake converted version about chromium/angle.
Needed to become usable outside of chromiums ninja build system.
"""
    parser = OptionParser(description=disc)
    # Global values where used on all scripts
    parser.add_option(
        "-B",
        "",
        metavar="DIR",
        dest="buildpath",
        default=defaults.BUILD_DIRECTORY,
        help="Explicitly specify a build directory. Default is \"./build\" on \"kodi-dev-kit\" folder.",
    )
    parser.add_option(
        "-S",
        "",
        metavar="DIR",
        dest="sourcepath",
        default="",
        help="[recommended] The source folder to parse. If not set becomes as default ./build/src/angle used",
    )
    parser.add_option(
        "-V",
        "",
        metavar="TEXT",
        dest="version",
        default="",
        help=f"[recommended] The \"angle\" source version to use. If not set and git clone becomes needed, becomes version {defaults.CURRENT_DEFAULT_VERSION} used",
    )
    parser.add_option(
        "-N",
        "",
        metavar="TEXT",
        dest="name",
        default=defaults.OWN_BASE_NAME,
        help=f"[recommended] Name where becomes additionally used on own git branches \"angle\" If not set becomes this name {defaults.OWN_BASE_NAME} used",
    )
    parser.add_option(
        "-D",
        "--dev",
        metavar="TEXT",
        dest="devbranch",
        default="",
        help="[optional] The name about development branch, becomes with additional dash added to selected angle version, e.g. by name \"fix\" becomes branch \"7258-fix\" created",
    )
    parser.add_option(
        "-R",
        "--repo",
        metavar="TEXT",
        dest="git_url_base",
        default=defaults.GIT_URL_REMOTE_UPSTREAM,
        help=f"[optional] The source repository url to use, if unset becomes default main repo \"{defaults.GIT_URL_REMOTE_UPSTREAM}\" used and no upstream about added",
    )
    parser.add_option(
        "",
        "--remote",
        metavar="TEXT",
        dest="git_url_remote_upstream",
        default=defaults.GIT_URL_REMOTE_UPSTREAM,
        help=f"[optional] The remote where wanted to use, normally \"{defaults.GIT_URL_REMOTE_UPSTREAM}\" becomes used",
    )
    parser.add_option(
        "",
        "--full-remote",
        action="store_true",
        dest="fullremote",
        default=False,
        help="Add the complete remote source, as standard becomes only the used branch forked, if one time set it stays activated as long the created sources are present",
    )
    parser.add_option(
        "-f",
        "--force",
        action="store_true",
        dest="force",
        default=False,
        help="Force the generation of auto code",
    )
    parser.add_option(
        "",
        "--debug-output",
        action="store_true",
        dest="debug",
        default=False,
        help="Put generator in a debug mode.",
    )
    (options, args) = parser.parse_args()

    # Set path to ./xbmc/xbmc/addons/kodi-dev-kit/build/src/angle
    # The Place ./xbmc/xbmc/addons/kodi-dev-kit/build is defined for DevKit own
    # builds where also included in .gitignore.
    fallback_path = os.path.join(defaults.BUILD_DIRECTORY, "src", "angle")
    fallback_version = defaults.CURRENT_DEFAULT_VERSION

    # Print configuration values to have always visible on builds
    Log.Init(options)
    Log.PrintMainStartBegin("Auto generation ANGLE CMake system", "Used options:")
    Log.PrintUsedStringValueLine("-V", "Used ANGLE version", options.version, fallback_version, True)
    Log.PrintUsedStringValueLine("-N", "Used ANGLE branch group name", options.name, defaults.OWN_BASE_NAME)
    Log.PrintUsedStringValueLine("-D", "Used development branch name", options.devbranch)
    Log.PrintUsedStringValueLine("-S", "Specified source directory", options.sourcepath, fallback_path, True)
    Log.PrintUsedStringValueLine("-B", "Specified build directory", options.buildpath, defaults.BUILD_DIRECTORY)
    Log.PrintUsedStringValueLine("-U", "Own Git repository", options.git_url_base)
    Log.PrintUsedStringValueLine("--remote", "Our used remote Git repository", options.git_url_remote_upstream)
    Log.PrintUsedBooleanValueLine("--full-remote", "Fetch complete from Chromium", options.fullremote)
    Log.PrintUsedBooleanValueLine("--force", "Force generation of auto code", options.force)
    Log.PrintUsedBooleanValueLine("--debug-output", "Debug script mode", options.debug)
    Log.PrintTextLine("")

    if not options.sourcepath:
        options.sourcepath = fallback_path
    if not options.version:
        options.version = fallback_version

    settings = settings(options, defaults)
    if not settings.InitSettings(parser):
        exit(-1)

    if os.path.exists(options.settings_local):
        Log.PrintTextLine(Log.style.CYAN + "Note: " + Log.style.WHITE + "Used created setting files as default, see \"*.ini\" files in build folder.")
        Log.PrintTextLine(Log.style.WHITE + "      To update setting files can be one time the changed value given as commandline value.")
        Log.PrintTextLine(Log.style.WHITE + "      Boolean commandline values becomes not stored and must be given all the time where needed.")
    else:
        Log.PrintTextLine(Log.style.CYAN + "Note: " + Log.style.WHITE + "Used programmed values as default")
    Log.PrintMainStartEnd()

    GenerateParts(settings)

    exit(0)
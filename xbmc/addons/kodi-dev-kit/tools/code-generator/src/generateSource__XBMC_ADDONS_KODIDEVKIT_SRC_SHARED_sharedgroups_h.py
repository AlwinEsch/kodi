# -*- coding: utf-8 -*-

#   Copyright (C) 2021-2022 Team Kodi
#   This file is part of Kodi - https://kodi.tv
#
#   SPDX-License-Identifier: GPL-2.0-or-later
#   See LICENSES/README.md for more information.

# Own includes
from code_generator import DEVKIT_DIR, KODI_DIR, INTERFACE_DIR
from .tools.helper_Log import *
from .tools.helpers import *

# Global includes
import glob, os, re


def GenerateSource__XBMC_ADDONS_KODIDEVKIT_SRC_SHARED_sharedgroups_h_RelatedCheck(
    filename,
):
    """
    This function is called by git update to be able to assign changed files to the dev kit.
    """
    return (
        True if filename == "{}/src/shared/SharedGroups.h".format(DEVKIT_DIR) else False
    )


def GenerateSource__XBMC_ADDONS_KODIDEVKIT_SRC_SHARED_sharedgroups_h(options):
    """
    This function generate the "SharedGroups.h in xbmc/addons/kodi-dev-kit/src/shared.

    The whole file not generated, only the related group enums becomes updated
    """

    source_file = "{}{}/src/shared/SharedGroups.h".format(KODI_DIR, DEVKIT_DIR)

    Log.PrintBegin(
        "Generate used interface groups in {}".format(source_file.replace(KODI_DIR, ""))
    )
    Log.PrintFollow(" (Hand edited file with auto part update)")

    if not os.path.exists(source_file):
        Log.PrintResult(Result.FAILURE)
        Log.PrintFatal(
            "Needed {} not present!".format(source_file.replace(KODI_DIR, ""))
        )
        exit(1)

    new_source_insert = ""
    search_dir = "{}{}/include/kodi/c-api/".format(KODI_DIR, DEVKIT_DIR)
    dirs = sorted(glob.glob("{}**/".format(search_dir), recursive=True))
    for dir in dirs:
        # Scan for used interface headers
        src_parts = sorted(glob.glob("{}*.h*".format(dir), recursive=False))
        for src_part in src_parts:
            if not FileContainsInterfaceAPICalls(src_part):
                continue

            # Translate the dir to a C usable ident string
            new_source_insert += "  funcGroup_{},\n".format(
                TranslatePathToCUsableString(src_part, search_dir)
            )

    # Read SharedGroups.h and add wanted enums
    new_source = ""
    f = open(source_file, "r")
    data = f.read()
    start = False
    for line in data.splitlines():
        if "/*---AUTO_GEN_PARSE<FUNC_GROUP_ENUM>---*/" in line:
            start = True
            new_source += line + "\n"
            new_source += "  /* Code below autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */\n"
            new_source += new_source_insert

        if not start:
            new_source += line + "\n"

        if "/*---AUTO_GEN_PARSE<FUNC_GROUP_ENUM_END>---*/" in line:
            start = False
            new_source += line + "\n"
        if start:
            continue

    f.close()

    # If differences or force update the file
    if new_source != open(source_file).read():
        with open(source_file, "w") as f:
            f.write(new_source)
        os.system("clang-format -style=file -i {}".format(source_file))
        Log.PrintResult(Result.UPDATE)
    else:
        Log.PrintResult(Result.ALREADY_DONE)

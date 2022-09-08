# -*- coding: utf-8 -*-

#   Copyright (C) 2021-2022 Team Kodi
#   This file is part of Kodi - https://kodi.tv
#
#   SPDX-License-Identifier: GPL-2.0-or-later
#   See LICENSES/README.md for more information.

# Own includes
from code_generator import DEVKIT_DIR, KODI_DIR, INTERFACE_DIR
from .tools.helper_Log import *
from .tools.sub_file_hdl import *

# Global includes
import glob, os, filecmp, shutil


def GenerateSource__XBMC_ADDONS_KODIDEVKIT_SRC_SHARED_instances_h(options):
    """
    This function generate automatic code inside xbmc/addons/kodi-dev-kit/src/shared/Instances.h
    """
    Log.PrintBegin("Generate file {}/src/shared/Instances.h".format(DEVKIT_DIR))

    target_file = "{}{}/src/shared/Instances.h".format(KODI_DIR, DEVKIT_DIR)
    if not os.path.exists(target_file):
        Log.PrintResult(Result.FAILURE)
        Log.PrintFatal("Error: File {} not exists!".format(target_file))
        exit(1)

    USED_INCLUDES = ""
    USED_UNIONS = ("    uint32_t dummy;\n"
                   "    union KODI_ADDON_FUNC_UNION kodi_addon;\n")

    search_dir = "{}{}/src/shared/api/".format(KODI_DIR, DEVKIT_DIR)
    scan_dir = "{}**/".format(search_dir)

    dirs = sorted(glob.glob(scan_dir, recursive=True))
    for dir in dirs:
        src_parts = sorted(glob.glob(dir + "*.h", recursive=False))
        for src_part in src_parts:
            data = read_file(src_part)
            if not data:
                Log.PrintResult(Result.FAILURE)
                Log.PrintFatal("Error: {}".format(code_generator.LAST_ERROR))
                exit(1)

            found = ""
            for line in data.splitlines():
                if "union KODI_" in line:
                    found = line

            if not found:
                continue

            USED_INCLUDES += (
                '#include "src/shared/api/' + src_part.replace(search_dir, "") + '"\n'
            )

            USED_UNIONS += "    {} {};\n".format(
                found, found.replace("union ", "").replace("_FUNC_UNION", "").lower()
            )

    data = read_file(target_file)
    if not data:
        Log.PrintResult(Result.FAILURE)
        Log.PrintFatal("Error: {}".format(code_generator.LAST_ERROR))
        exit(1)

    is_set_1 = False
    is_set_2 = False
    start_1 = False
    start_2 = False
    file_text = ""
    for line in data.splitlines():
        if "/*---AUTO_GEN_PARSE<USED_INCLUDES>---*/" in line:
            start_1 = True
        elif "/*---AUTO_GEN_PARSE<USED_INCLUDES_END>---*/" in line:
            is_set_1 = False
            start_1 = False
        elif start_1 and not is_set_1:
            file_text += USED_INCLUDES
            is_set_1 = True
            continue

        if "/*---AUTO_GEN_PARSE<USED_UNIONS>---*/" in line:
            start_2 = True
        elif "/*---AUTO_GEN_PARSE<USED_UNIONS_END>---*/" in line:
            is_set_2 = False
            start_2 = False
        elif start_2 and not is_set_2:
            file_text += USED_UNIONS
            is_set_2 = True
            continue

        if not is_set_1 and not is_set_2:
            file_text += line + "\n"

    # Make now the creation and copy of file
    temp_file = target_file.replace("{}xbmc/addons".format(KODI_DIR), "./tmp")
    if not write_file(temp_file, file_text, True):
        Log.PrintResult(Result.FAILURE)
        Log.PrintFatal("Error: {}".format(code_generator.LAST_ERROR))
        exit(1)

    if options.force or not filecmp.cmp(target_file, temp_file):
        shutil.copyfile(temp_file, target_file)
        Log.PrintResult(Result.UPDATE)
    else:
        Log.PrintResult(Result.ALREADY_DONE)

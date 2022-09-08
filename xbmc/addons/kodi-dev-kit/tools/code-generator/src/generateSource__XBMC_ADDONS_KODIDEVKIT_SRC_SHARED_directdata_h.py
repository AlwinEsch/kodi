# -*- coding: utf-8 -*-

#   Copyright (C) 2021-2022 Team Kodi
#   This file is part of Kodi - https://kodi.tv
#
#   SPDX-License-Identifier: GPL-2.0-or-later
#   See LICENSES/README.md for more information.

# Own includes
from code_generator import DEVKIT_DIR, KODI_DIR, INTERFACE_DIR, LAST_ERROR
from .tools.helper_Log import *
from .tools.helpers import *
from .tools.sub_file_hdl import *

# Global includes
import glob, os, filecmp, shutil


def GenerateSource__XBMC_ADDONS_KODIDEVKIT_SRC_SHARED_directdata_h_RelatedCheck(
    filename,
):
    """
    This function is called by git update to be able to assign changed files to the dev kit.
    """
    True if filename == "{}/src/shared/DirectData.h".format(DEVKIT_DIR) else False


def GenerateSource__XBMC_ADDONS_KODIDEVKIT_SRC_SHARED_directdata_h(options):
    search_dir = "{}{}/include/kodi/c-api/".format(KODI_DIR, DEVKIT_DIR)
    target_file = "{}{}/src/shared/DirectData.h".format(KODI_DIR, DEVKIT_DIR)
    scan_dir = "{}**/".format(search_dir)

    Log.PrintBegin("Generate {}".format(target_file.replace(KODI_DIR, "")))

    code_include = []
    code_struct_to_kodi = []
    code_struct_to_addon = []
    dirs = sorted(glob.glob(scan_dir, recursive=True))
    for dir in dirs:
        source_dir = dir.replace(search_dir, "")
        src_parts = sorted(glob.glob("{}*.h*".format(dir), recursive=True))
        for src_part in src_parts:
            if not FileContainsInterfaceAPICalls(src_part):
                continue

            header_group_id = TranslatePathToCUsableString(src_part, search_dir)
            code_struct_to_kodi.append(
                "  directFuncToKodi_{} {};".format(header_group_id, header_group_id)
            )
            code_struct_to_addon.append(
                "  directFuncToAddon_{} {};".format(header_group_id, header_group_id)
            )
            code_include.append(
                '#include "{}"'.format(src_part.replace(search_dir, "src/shared/api/"))
            )

    if len(code_struct_to_kodi) == 0:
        code_struct_to_kodi.append("  int dummyValue;")
    if len(code_struct_to_addon) == 0:
        code_struct_to_addon.append("  int dummyValue;")

    # Make final DirectData.h
    file_text = """{0}
/*
 * This header is used to allow direct api interface between addon and Kodi.
 * Becomes used in case the addon is loaded as dynamic lib and without use of
 * shared memory.
 *
 * This can be in case if higher performance needed or OS not support
 * independent apps (e.g. iOS/tvos)
 *
 * The structure defined here is only used between library and Kodi not used
 * inside addon itself, with this are changes here not critical.
 */

#pragma once

{1}
#include "src/shared/api/processor.h"

namespace KODI
{{
namespace ADDONS
{{
namespace INTERFACE
{{

struct DirectAddonIfcToKodi
{{
{2}
}};

struct DirectAddonIfcToAddon
{{
{3}
  directFuncToAddon_processor_h processor_h;
}};

struct DirectAddonIfc
{{
  DirectAddonIfcToKodi to_kodi;
  DirectAddonIfcToAddon to_addon;
}};

}} /* namespace INTERFACE */
}} /* namespace ADDONS */
}} /* namespace KODI */
                """.format(
        HEADER_FILE_COPYRIGHT,
        "\n".join(code_include),
        "\n".join(code_struct_to_kodi),
        "\n".join(code_struct_to_addon),
    )

    # Make now the creation and copy of file
    temp_file = target_file.replace("{}xbmc/addons".format(KODI_DIR), "./tmp")
    if not write_file(temp_file, file_text, True):
      Log.PrintResult(Result.FAILURE)
      Log.PrintFatal(
          "Error: {}".format(code_generator.LAST_ERROR)
      )
      exit(1)

    present = os.path.isfile(target_file)
    if (options.force or not present or not filecmp.cmp(target_file, temp_file)):
        os.makedirs(os.path.dirname(target_file), exist_ok=True)
        shutil.copyfile(temp_file, target_file)
        Log.PrintResult(Result.NEW if not present else Result.UPDATE)
    else:
        Log.PrintResult(Result.ALREADY_DONE)

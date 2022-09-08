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
import glob, os


def GenerateSource__XBMC_ADDONS_INTERFACE_interface_cpp_h(options):
    base_dir_cpp = "{}{}/Interface.cpp".format(KODI_DIR, INTERFACE_DIR)
    base_dir_h = "{}{}/Interface.h".format(KODI_DIR, INTERFACE_DIR)

    Log.PrintBegin(
        "Generate part changes in {} and {}".format(
            base_dir_cpp.replace(KODI_DIR, ""), base_dir_h.replace(KODI_DIR, "")
        )
    )

    for entry in [base_dir_cpp, base_dir_h]:
        if not os.path.exists(entry):
            Log.PrintResult(Result.FAILURE)
            Log.PrintFatal(
                "ERROR: Needed {} not present!".format(entry.replace(KODI_DIR, ""))
            )
            exit(1)

    src_list_include = ""
    src_list_set1 = ""
    src_list_set2 = ""
    hdr_list_set1 = ""
    hdr_list_set2 = ""
    search_dir = "{}{}/api/".format(KODI_DIR, INTERFACE_DIR)
    scan_dir = "{}**/".format(search_dir)

    dirs = sorted(glob.glob(scan_dir, recursive=True))
    for dir in dirs:
        source_dir = dir.replace(search_dir, "")

        src_parts = sorted(glob.glob(dir + "*.h", recursive=False))
        for src_part in src_parts:
            src_list_include += '#include "api/{}"\n'.format(
                src_part.replace(search_dir, "")
            )
            part_name = TranslatePathToCUsableString(src_part, search_dir)
            src_list_set1 += (
                "  kodi_{0} = new CHdl_kodi_{0}(*this);\n"
                "  m_handlers[funcGroup_{0}] = kodi_{0};\n".format(part_name)
            )

            src_list_set2 += "    kodi_{0}->InitDirect(&m_directIfc->to_kodi.{0}, &m_directIfc->to_addon.{0});\n".format(
                part_name
            )

            hdr_list_set1 += "class CHdl_kodi_{0};\n".format(part_name)
            hdr_list_set2 += "  CHdl_kodi_{0}* kodi_{0};\n".format(part_name)

    Log.PrintResult(Result.SEE_BELOW)

    # ---------------------------------------------------------------------------

    Log.PrintBegin(" - Check {}".format(base_dir_cpp.replace(KODI_DIR, "")))

    data = read_file(base_dir_cpp)
    if not data:
        Log.PrintResult(Result.FAILURE)
        Log.PrintFatal("Error: {}".format(code_generator.LAST_ERROR))
        exit(1)

    new_code = ""
    INTERFACE_CPP_SET_INCLUDE = False
    INTERFACE_CPP_H_SET = False
    for line in data.splitlines():
        if not INTERFACE_CPP_SET_INCLUDE and not INTERFACE_CPP_H_SET:
            new_code += "{}\n".format(line)

        if "/*---AUTO_GEN_PARSE<INTERFACE_CPP_SET_INCLUDE>---*/" in line:
            INTERFACE_CPP_SET_INCLUDE = True

        if "/*---AUTO_GEN_PARSE<INTERFACE_CPP_SET_INCLUDE_END>---*/" in line:
            INTERFACE_CPP_SET_INCLUDE = False
            new_code += "{}{}\n".format(src_list_include, line)

        if "/*---AUTO_GEN_PARSE<INTERFACE_CPP_H_SET>---*/" in line:
            INTERFACE_CPP_H_SET = True

        if u"/*---AUTO_GEN_PARSE<INTERFACE_CPP_H_SET_END>---*/" in line:
            INTERFACE_CPP_H_SET = False
            new_code += (
                "{}"
                "\n"
                "  if (m_directIfc != nullptr)\n"
                "  {{\n"
                "{}"
                "  }}\n"
                "{}\n".format(src_list_set1, src_list_set2, line)
            )

    if data != new_code:
        Log.PrintResult(Result.UPDATE)
        write_file(base_dir_cpp, new_code, True)
    else:
        Log.PrintResult(Result.ALREADY_DONE)

    # ---------------------------------------------------------------------------

    Log.PrintBegin(" - Check {}".format(base_dir_h.replace(KODI_DIR, "")))

    data = read_file(base_dir_h)
    if not data:
        Log.PrintResult(Result.FAILURE)
        Log.PrintFatal("Error: {}".format(code_generator.LAST_ERROR))
        exit(1)

    new_code = ""
    INTERFACE_CPP_H_SET1 = False
    INTERFACE_CPP_H_SET2 = False
    for line in data.splitlines():
        if not INTERFACE_CPP_H_SET1 and not INTERFACE_CPP_H_SET2:
            new_code += "{}\n".format(line)

        if "/*---AUTO_GEN_PARSE<INTERFACE_CPP_H_SET1>---*/" in line:
            INTERFACE_CPP_H_SET1 = True

        if "/*---AUTO_GEN_PARSE<INTERFACE_CPP_H_SET1_END>---*/" in line:
            INTERFACE_CPP_H_SET1 = False
            new_code += "{}{}\n".format(hdr_list_set1, line)

        if "/*---AUTO_GEN_PARSE<INTERFACE_CPP_H_SET2>---*/" in line:
            INTERFACE_CPP_H_SET2 = True

        if "/*---AUTO_GEN_PARSE<INTERFACE_CPP_H_SET2_END>---*/" in line:
            INTERFACE_CPP_H_SET2 = False
            new_code += "{}{}\n".format(hdr_list_set2, line)

    if data != new_code:
        Log.PrintResult(Result.UPDATE)
        write_file(base_dir_h, new_code, True)
    else:
        Log.PrintResult(Result.ALREADY_DONE)

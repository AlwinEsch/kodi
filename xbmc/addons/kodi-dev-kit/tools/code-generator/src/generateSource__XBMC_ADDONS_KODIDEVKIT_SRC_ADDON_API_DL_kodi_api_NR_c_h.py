# -*- coding: utf-8 -*-

#   Copyright (C) 2021-2022 Team Kodi
#   This file is part of Kodi - https://kodi.tv
#
#   SPDX-License-Identifier: GPL-2.0-or-later
#   See LICENSES/README.md for more information.

# Own includes
from code_generator import DEVKIT_DIR, KODI_DIR, INTERFACE_DIR
from .generateScan__USED_VALUES import *
from .tools.helper_Log import *
from .tools.sub_file_hdl import *
from .tools.helpers import *

# Global includes
from hashlib import sha256
import glob, os, re, filecmp, shutil


class LibraryDLGenerator:
    def __init__(self, options, api):
        self.options = options
        self.api = api
        self.api_list_code = []
        self.api_list_typedef = []
        self.api_list_struct = []

    def Generate(self):
        search_dir = "{}{}/include/kodi/c-api/".format(KODI_DIR, DEVKIT_DIR)
        scan_dir = "{}/**/".format(search_dir)

        #-----------------------------------------------------------------------
        # Create list of used API code parts
        #{{{

        Log.PrintBegin("Generate DL code load part files for API: ")
        Log.PrintFollow(str(self.api))

        highest_found_api = 0

        includes_h = ""
        includes_c = ""
        structs = ""
        dirs = sorted(glob.glob(scan_dir, recursive=True))
        for dir in dirs:
            source_dir = dir.replace(search_dir, "")

            src_parts = sorted(glob.glob("{}*.h*".format(dir), recursive=True))
            for src_part in src_parts:
                if not FileContainsInterfaceAPICalls(src_part):
                    continue

                api_list_typedef = []
                api_list_struct = []
                api_list_code = []
                with open(src_part) as f:
                    file_text = f.read()
                    f.close()

                function_addon = ""
                for line in file_text.splitlines():
                    if "ATTR_DLL_EXPORT" in line and not "#define" in line:
                        line = line.strip()
                        if line.startswith("//") or line.startswith("/*"):
                            continue
                        function_addon = line
                    elif function_addon != "":
                        function_addon += line
                    if function_addon != "" and ";" in line:
                        function_addon = " ".join(function_addon.split())

                        scanner = RegexScanExport(function_addon, self.options)
                        scanner.ScanFunction()

                        if scanner.api_added > highest_found_api:
                            highest_found_api = scanner.api_added

                        if scanner.api_added > self.api:
                            function_addon = ""
                            continue
                        if (
                            scanner.api_removed != -1
                            and scanner.api_removed <= self.api
                        ):
                            function_addon = ""
                            continue

                        if scanner.api_added > highest_found_api:
                            highest_found_api = scanner.api_added

                        func_values = ""
                        current_value = 1
                        for value in scanner.values:
                            if value != "...":
                                value_name = value.split()[-1].strip()
                                value_type = value[: -len(value_name)].strip()
                                if "[" in value_name:
                                    value_type += "[" + value_name.split("[", 1)[-1]

                                include = '#include "../{}"\n'.format(
                                    src_part.replace(search_dir, "")
                                )
                                check_type = (
                                    value_type.replace("*", "")
                                    .replace("[]", "")
                                    .replace("const", "")
                                    .strip()
                                )
                                if (
                                    not check_type in known_STANDARD_C_TYPES
                                    and check_type != "char"
                                    and check_type != "void"
                                ):
                                    if "struct " in value_type:
                                        struct = "{};\n".format(check_type)
                                        if not struct in structs:
                                            structs += struct
                                    else:
                                        if not include in includes_h:
                                            includes_h += include
                            else:
                                value_name = ""
                                value_type = "..."

                            func_values += value_type + (
                                ", " if current_value < len(scanner.values) else ""
                            )
                            current_value += 1

                        include = '#include "../{}"\n'.format(
                            src_part.replace(search_dir, "")
                        )
                        check_type = (
                            scanner.retval.replace("*", "")
                            .replace("[]", "")
                            .replace("const", "")
                            .strip()
                        )
                        if (
                            not check_type in known_STANDARD_C_TYPES
                            and check_type != "char"
                            and check_type != "void"
                        ):
                            if "struct " in scanner.retval:
                                struct = "{};\n".format(check_type)
                                if not struct in structs:
                                    structs += struct
                            else:
                                if not include in includes_h:
                                    includes_h += include
                        include_c = include.replace('"../', '"include/kodi/c-api/')
                        if not include_c in includes_c and not include in includes_h:
                            includes_c += include_c

                        entry = "  api->{} = &{};".format(
                            scanner.funcname, scanner.funcname
                        )
                        api_list_code.append(entry)

                        entry = "typedef {}(ATTR_DL_APIENTRYP PFN_{}_V{})({});".format(
                            scanner.retval,
                            scanner.funcname.upper(),
                            scanner.api_added,
                            func_values,
                        )
                        api_list_typedef.append(entry)

                        entry = "PFN_{}_V{} {};".format(
                            scanner.funcname.upper(),
                            scanner.api_added,
                            scanner.funcname,
                        )
                        api_list_struct.append(entry)
                        function_addon = ""
                        continue

                if api_list_struct and api_list_typedef:
                    self.api_list_typedef.append(
                        "// {}".format(src_part.replace(search_dir, ""))
                    )
                    self.api_list_typedef += sorted(api_list_typedef)
                    self.api_list_typedef.append("")

                    self.api_list_struct.append(
                        "// {}".format(src_part.replace(search_dir, ""))
                    )
                    self.api_list_struct += sorted(api_list_struct)
                    self.api_list_struct.append("")

                    self.api_list_code.append(
                        "  // {}".format(src_part.replace(search_dir, ""))
                    )
                    self.api_list_code += sorted(api_list_code)
                    self.api_list_code.append("")

        Log.PrintResult(Result.SEE_BELOW)

        #}}}

        #-----------------------------------------------------------------------
        # Create file include/kodi/c-api/dl/kodi_api_[API].h
        #{{{

        Log.PrintBegin(" - Create file: ")
        Log.PrintFollow("{}/include/kodi/c-api/dl/kodi_api_{}.h".format(
                DEVKIT_DIR, self.api
            )
        )

        code_part = ""
        for entry in self.api_list_typedef:
            code_part += "  {}\n".format(entry)
        code_part += "  struct kodi_api_{}\n".format(self.api)
        code_part += "  {\n"
        for entry in self.api_list_struct:
            code_part += "  {}\n".format(entry)
        code_part += "  };\n"

        hashedWord = sha256(code_part.encode("utf-8")).hexdigest()

        code_h = (
            "{0}"
            "\n"
            "/* API_HASH<{5}> */\n"
            "\n"
            "#ifndef C_API_DL_API_{1}_H\n"
            "#define C_API_DL_API_{1}_H\n"
            "\n"
            '#include "../addon_base.h"\n'
            "{2}"
            "\n"
            "#ifndef ATTR_DL_APIENTRYP\n"
            "#define ATTR_DL_APIENTRYP ATTR_APIENTRYP\n"
            "#endif\n"
            "\n"
            "#ifndef ATTR_DL_DLL_EXPORT\n"
            "#define ATTR_DL_DLL_EXPORT ATTR_DLL_EXPORT\n"
            "#endif\n"
            "\n"
            "#ifdef __cplusplus\n"
            'extern "C"\n'
            "{{\n"
            "#endif /* __cplusplus */\n"
            "\n"
            "  struct kodi_api_{1};\n"
            "\n"
            "  ATTR_DL_DLL_EXPORT bool load_kodi_api_{1}(struct kodi_api_{1}* api);\n"
            "\n"
            "{3}"
            "\n"
            "{4}"
            "\n"
            "#ifdef __cplusplus\n"
            '}} /* extern "C" */\n'
            "#endif /* __cplusplus */\n"
            "\n"
            "#endif /* C_API_DL_API_{1}_H */\n"
        ).format(
            HEADER_FILE_COPYRIGHT, self.api, includes_h, structs, code_part, hashedWord
        )

        target_file = "{}{}/include/kodi/c-api/dl/kodi_api_{}.h".format(
            KODI_DIR, DEVKIT_DIR, self.api
        )

        temp_file = "./tmp/tmp-auto-gen.h"
        if not write_file(temp_file, code_h, True):
            Log.PrintResult(Result.FAILURE)
            Log.PrintFatal("Error: {}".format(code_generator.LAST_ERROR))
            exit(1)

        if (
            self.options.force
            or not os.path.isfile(target_file)
            or not filecmp.cmp(target_file, temp_file)
        ):
            os.makedirs(os.path.dirname(target_file), exist_ok=True)
            shutil.copyfile(temp_file, target_file)
            Log.PrintResult(Result.UPDATE)
        else:
            Log.PrintResult(Result.ALREADY_DONE)

        #}}}

        #-----------------------------------------------------------------------
        # Create file src/addon/api/dl/kodi_api_[API].c
        #{{{
        Log.PrintBegin(" - Create file: ")
        Log.PrintFollow("{}/src/addon/api/dl/kodi_api_{}.c".format(
                DEVKIT_DIR, self.api
            )
        )

        code_c = (
            "{0}"
            "\n"
            "/* API_HASH<{5}> */\n"
            "\n"
            '#include "../../../../include/kodi/c-api/dl/kodi_api_{1}.h"\n'
            "\n"
            "{2}"
            "\n"
            "ATTR_DL_DLL_EXPORT bool load_kodi_api_{1}(struct kodi_api_{1}* api)\n"
            "{{\n"
            "  if (api == NULL)\n"
            "  {{\n"
            "    return false;\n"
            "  }}\n"
            "\n"
            "{6}"
            "\n"
            "  return true;\n"
            "}}\n"
        ).format(
            HEADER_FILE_COPYRIGHT,
            self.api,
            includes_c,
            structs,
            code_part,
            hashedWord,
            "\n".join(self.api_list_code),
        )

        target_file = "{}{}/src/addon/api/dl/kodi_api_{}.c".format(
            KODI_DIR, DEVKIT_DIR, self.api
        )

        temp_file = "./tmp/tmp-auto-gen.c"
        if not write_file(temp_file, code_c, True):
            Log.PrintResult(Result.FAILURE)
            Log.PrintFatal("Error: {}".format(code_generator.LAST_ERROR))
            exit(1)

        if (
            self.options.force
            or not os.path.isfile(target_file)
            or not filecmp.cmp(target_file, temp_file)
        ):
            os.makedirs(os.path.dirname(target_file), exist_ok=True)
            shutil.copyfile(temp_file, target_file)
            Log.PrintResult(Result.UPDATE)
        else:
            Log.PrintResult(Result.ALREADY_DONE)

        #}}}

        return True if highest_found_api > self.api else False

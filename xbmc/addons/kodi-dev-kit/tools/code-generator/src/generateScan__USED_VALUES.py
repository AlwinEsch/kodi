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
import glob, re

known_STANDARD_C_TYPES = [ 'unsigned int',
                           'signed int',
                           'int',
                           'unsigned short',
                           'signed short',
                           'short',
                           'unsigned long',
                           'signed long',
                           'long',
                           'unsigned long long',
                           'signed long long',
                           'long long',
                           'bool',
                           'float',
                           'double',
                           'long double',
                           'int64_t',
                           'int32_t',
                           'int16_t',
                           'int8_t',
                           'uint64_t',
                           'uint32_t',
                           'uint16_t',
                           'uint8_t',
                           'size_t',
                           'ssize_t',
                           'time_t' ]

known_STANDARD_C_PTR_TYPES = [ 'void*',
                               'unsigned int*',
                               'signed int*',
                               'int*',
                               'unsigned short*',
                               'signed short*',
                               'short*',
                               'unsigned long*',
                               'signed long*',
                               'long*',
                               'unsigned long long*',
                               'signed long long*',
                               'long long*',
                               'bool*',
                               'float*',
                               'double*',
                               'long double*',
                               'int64_t*',
                               'int32_t*',
                               'int16_t*',
                               'int8_t*',
                               'uint64_t*',
                               'uint32_t*',
                               'uint16_t*',
                               'uint8_t*',
                               'size_t*',
                               'ssize_t*' ]

known_STANDARD_C_TYPES_SIGNED = [ 'signed int',
                                  'int',
                                  'signed short',
                                  'short',
                                  'signed long',
                                  'long',
                                  'signed long long',
                                  'long long',
                                  'int64_t',
                                  'int32_t',
                                  'int16_t',
                                  'int8_t',
                                  'ssize_t',
                                  'time_t' ]

known_STANDARD_C_TYPES_UNSIGNED = [ 'unsigned int',
                                    'unsigned short',
                                    'unsigned long',
                                    'unsigned long long',
                                    'uint64_t',
                                    'uint32_t',
                                    'uint16_t',
                                    'uint8_t',
                                    'size_t' ]

KNOWN_VOID_PTR_DEFINES = []  # List created by ScanAllHdlVoidPointer
KNOWN_ENUMS = []  # List created by ScanAllEnums
KNOWN_ENUMS_DEFAULT = []  # List created by ScanAllEnums


def ScanAllHdlVoidPointer(options):
    Log.PrintBegin("Check used void pointers: ")

    search_dir = "{}{}/include/kodi/c-api/".format(KODI_DIR, DEVKIT_DIR)
    scan_dir = "{}/**/".format(search_dir)

    dirs = sorted(glob.glob(scan_dir, recursive=True))
    for dir in dirs:
        source_dir = dir.replace(search_dir, "")

        src_parts = sorted(glob.glob(dir + "*.h*", recursive=False))
        for src_part in src_parts:
            data = read_file(src_part)
            if not data:
                Log.PrintResult(Result.FAILURE)
                Log.PrintFatal("Error: {}".format(code_generator.LAST_ERROR))
                exit(1)

            for line in data.splitlines():
                m = re.search("^.*?#define\ *([a-zA-Z[0-9_]*)\ *(void\*|KODI_ADDON_INSTANCE_HDL)", line)
                if m and m.group(1) and not m.group(1) in KNOWN_VOID_PTR_DEFINES:
                    KNOWN_VOID_PTR_DEFINES.append(m.group(1))
                    continue
                m = re.search("^.*typedef* (void|KODI_ADDON_INSTANCE_HDL)\** ([a-zA-Z[0-9_]*)\ *;$", line)
                if m and m.group(2) and not m.group(2) in KNOWN_VOID_PTR_DEFINES:
                    KNOWN_VOID_PTR_DEFINES.append(m.group(2))
                    continue

    Log.PrintFollow(
        "{} different values with 'void*'".format(len(KNOWN_VOID_PTR_DEFINES))
    )
    Log.PrintResult(Result.OK)

    if options.debug:
        Log.PrintBegin("- Used:")
        Log.PrintResult(Result.SEE_BELOW)
        Log.PrintFollow(
            "".join(
                "                        {}\n".format(entry)
                for entry in KNOWN_VOID_PTR_DEFINES
            )
        )


def ScanAllEnums(options):
    Log.PrintBegin("Check used enum's: ")

    search_dir = "{}{}/include/kodi/c-api/".format(KODI_DIR, DEVKIT_DIR)
    scan_dir = "{}/**/".format(search_dir)

    dirs = sorted(glob.glob(scan_dir, recursive=True))
    for dir in dirs:
        source_dir = dir.replace(search_dir, "")

        src_parts = sorted(glob.glob(dir + "*.h*", recursive=False))
        found_one = ""
        found_use = ""
        for src_part in src_parts:
            data = read_file(src_part)
            if not data:
                Log.PrintResult(Result.FAILURE)
                Log.PrintFatal("Error: {}".format(code_generator.LAST_ERROR))
                exit(1)

            for line in data.splitlines():
                if found_one:
                    if "{" in line:
                        found_use = found_one
                        if not found_one in KNOWN_ENUMS:
                            KNOWN_ENUMS.append(found_one)
                    found_one = ""
                    continue
                if found_use:
                    if "//" in line or "/*" in line:
                        continue
                    KNOWN_ENUMS_DEFAULT.append(
                        [
                            found_use,
                            line.strip().split(",")[0].split("=")[0].strip(),
                        ]
                    )
                    found_use = ""

                if not "enum " in line:
                    continue

                m = re.search("^.*?enum\ *([a-zA-Z[0-9_]*)", line)
                if m and m.group(1) and not m.group(1) in KNOWN_ENUMS:
                    found_one = m.group(1)
                    continue

    Log.PrintFollow("{} different enums".format(len(KNOWN_ENUMS)))
    Log.PrintResult(Result.OK)

    if options.debug:
        Log.PrintBegin("- Used:")
        Log.PrintResult(Result.SEE_BELOW)
        Log.PrintFollow(
            "".join(
                "                        {}\n".format(entry) for entry in KNOWN_ENUMS
            )
        )


def CheckKnownEnum(value):
    if len(value.split()) > 1:
        m = re.search("^.*?enum\ *([a-zA-Z[0-9_]*)", value)
        if m and m.group(1) and m.group(1) in KNOWN_ENUMS:
            return True
    elif value in KNOWN_ENUMS:
        return True
    return False

def ContainsHdlVoidPointer(value):
  value = value.replace('const ', '')
  if value.split(' ', 1)[0] in KNOWN_VOID_PTR_DEFINES:
    return True
  return False


def ContainsHdlVoidAsReturnPointer(value):
  print(value)
  if value.count("*") == 1 and value.split('*', 1)[0].strip() in KNOWN_VOID_PTR_DEFINES:
      return True
  return False


def FindDefaultEnumReturn(retval):
  for enum in KNOWN_ENUMS_DEFAULT:
    if retval == enum[0]:
      return enum[1]
  return ' 0'

def GetDefaultReturn(retval, function_complete, file_text, scan_file = ''):
  if retval == 'void':
    return ''
  elif 'bool' in retval:
    return ' false'
  elif '*' in retval:
    return ' nullptr'
  elif ContainsHdlVoidPointer(retval):
    return ' nullptr'
  elif 'float' in retval:
    return ' 0.0f'
  elif 'double' in retval:
    return ' 0.0'
  elif retval in known_STANDARD_C_TYPES_UNSIGNED:
    return ' 0'
  elif retval in known_STANDARD_C_TYPES_SIGNED:
    return ' -1'
  elif 'enum ' in retval:
    return ' ' + FindDefaultEnumReturn(retval.replace('enum ', ''))
  else:
    for line in file_text.splitlines():
      if retval in line:
        m = re.search('typedef .* \(\*' + retval + '\)\(.*\);', line)
        if m:
          return ' nullptr'

    print('FATAL: Can\'t get default return for:')
    print('       \'{}\''.format(function_complete))
    if scan_file:
      print('       On file {}'.format(scan_file))
    if not 'enum ' in retval and CheckKnownEnum('enum ' + retval):
      print('       You must set for enum a "C" conform value where must begin with enum ("enum {}")'.format(retval))
    else:
      print('       Detected non void return value and can\'t find a usable default about!')
    raise

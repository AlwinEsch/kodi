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
import re

LOWEST_API = -1
HIGHEST_API = -1


def ScanUsedAPI(options):
    global LOWEST_API
    global HIGHEST_API

    Log.PrintBegin("Check used API versions")

    data = read_file("{}{}/include/kodi/c-api/version.h".format(KODI_DIR, DEVKIT_DIR))
    if not data:
        Log.PrintResult(Result.FAILURE)
        Log.PrintFatal("Error: {}".format(code_generator.LAST_ERROR))
        exit(1)

    version_list = []
    for line in data.splitlines():
        m = re.search("^#define\ __KODI_API_([0-9]*)__[ \t]*([0-9]*)?[ \t]*", line)
        if m and m.group(1) and m.group(2):
            if m.group(1) != m.group(2):
                Log.PrintResult(Result.FAILURE)
                Log.PrintFatal(
                    'Inside "kodi-dev-kit/include/kodi/c-api/version.h" for API {} define set with different number {}.\n'
                    "                             Both should equal!".format(
                        m.group(1), m.group(2)
                    )
                )
                exit(1)
            if int(m.group(1)) > HIGHEST_API:
                HIGHEST_API = int(m.group(1))
                version_list.append(HIGHEST_API)
            if int(m.group(1)) < LOWEST_API or LOWEST_API < 0:
                LOWEST_API = int(m.group(1))

    start_api = LOWEST_API
    for api in version_list:
        if api != start_api:
            Log.PrintResult(Result.FAILURE)
            Log.PrintFatal(
                "It seems between Kodi min API {} and max API {} is API {} missing!\n"
                '                             Is "kodi-dev-kit/include/kodi/c-api/version.h" set correct?'.format(
                    LOWEST_API, HIGHEST_API, start_api
                )
            )
            exit(1)
        start_api += 1

    if HIGHEST_API < 0:
        Log.PrintResult(Result.FAILURE)
        Log.PrintFatal(
            " Failed to get currently highest used api version!\n"
            '                              Is "kodi-dev-kit/include/kodi/c-api/version.h" available and set correct?'
        )
        exit(1)

    Log.PrintResult(Result.SEE_BELOW)
    Log.PrintBegin("- Highest on scan found and used Kodi API version: ")
    Log.PrintFollow(str(HIGHEST_API))
    Log.PrintResult(Result.OK)
    Log.PrintBegin("- Lowest on scan found and used Kodi API version: ")
    Log.PrintFollow(str(LOWEST_API))
    Log.PrintResult(Result.OK)
    return HIGHEST_API


def GetLowestUsedAPI():
    """
    This function return the lowest used API version from Kodi dev kit.

    On first call the "kodi/c-api/version.h" becomes checked, on every next call
    the found value becomes reused.

    Returns:
      integer: The lowest API used.

    Raises:
      NameError: If something within scan goes wrong, error message comes before.

    Examples:
      >>> api = GetLowestUsedAPI()
    """
    global LOWEST_API
    if LOWEST_API > 0:
        return LOWEST_API

    # If not present start ScanUsedAPI where scan on init both
    ScanUsedAPI()
    return LOWEST_API


def GetHighestUsedAPI():
    """
    This function return the highest used API version from Kodi dev kit.

    On first call the "kodi/c-api/version.h" becomes checked, on every next call
    the found value becomes reused.

    Returns:
      integer: The highest API used.

    Raises:
      NameError: If something within scan goes wrong, error message comes before.

    Examples:
      >>> api = GetHighestUsedAPI()
    """
    global HIGHEST_API
    if HIGHEST_API > 0:
        return HIGHEST_API

    # If not present start ScanUsedAPI where scan on init both
    ScanUsedAPI()
    return HIGHEST_API

def IsHighestUsedFunctionVersion(file_name, function, ifc_name_func, api_added):
  with open(file_name, 'r') as f:
    file_text = f.read()
    f.close()

    for line in file_text.splitlines():
      if line.strip().startswith('PFN_{}_V'.format(function.upper())):
        api_max = GetHighestUsedAPI()
        while api_max > int(api_added):
          if line.strip().startswith('PFN_{}_V{} {};'.format(function.upper(), api_max, ifc_name_func)):
            return False

          api_max -= 1

  for line in file_text.splitlines():
    if line.strip().startswith('PFN_{}_V{} {};'.format(function.upper(), int(api_added), ifc_name_func)):
      return True

  print('FATAL: Failed to find "{}"!'.format('PFN_{}_V{} {};'.format(function.upper(), int(api_added), ifc_name_func)))
  print('       Is "{}" set correct?'.format(file_name))
  raise NameError('Dev kit scan failure')

def IsLowerToUsedFunctionVersion(file_name, function, ifc_name_func, api_added):

  if api_added <= GetLowestUsedAPI():
    return False

  with open(file_name, 'r') as f:
    file_text = f.read()
    f.close()

    for line in file_text.splitlines():
      if line.strip().startswith('PFN_{}_V{} {};'.format(function.upper(), api_added, ifc_name_func)):
        return False

    return True

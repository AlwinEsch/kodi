#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#   Copyright (C) 2021-2022 Team Kodi
#   This file is part of Kodi - https://kodi.tv
#
#   SPDX-License-Identifier: GPL-2.0-or-later
#   See LICENSES/README.md for more information.

KODI_DIR = "../../../../../"
DEVKIT_DIR = "xbmc/addons/kodi-dev-kit"
INTERFACE_DIR = "xbmc/addons/interface"
LAST_ERROR = ""

# Global includes
from optparse import OptionParser

# Own includes
from src.interface_code_generator import *
from src.commitChanges import *
from src.generateCMake__CMAKE_TREEDATA_COMMON_addon_dev_kit_txt import *
from src.generateCMake__CMAKE_TREEDATA_COMMON_addon_interface_txt import *
from src.generateCMake__XBMC_ADDONS_KODIDEVKIT_INCLUDE_KODI_allfiles import *
from src.generateCMake__XBMC_ADDONS_KODIDEVKIT_SRC_ADDON_allfiles import *
from src.generateCMake__XBMC_ADDONS_KODIDEVKIT_SRC_SHARED_cmakelists_txt import *
from src.generateScan__USED_API import *
from src.generateScan__USED_VALUES import *
from src.generateScan__CALLBACK_USE_PLACES import *
from src.generateSource__XBMC_ADDONS_INTERFACE_interface_cpp_h import *
from src.generateSource__XBMC_ADDONS_KODIDEVKIT_SRC_ADDON_API_DL_kodi_api_NR_c_h import *
from src.generateSource__XBMC_ADDONS_KODIDEVKIT_SRC_ADDON_CORE_addon_control_cpp import *
from src.generateSource__XBMC_ADDONS_KODIDEVKIT_SRC_SHARED_directdata_h import *
from src.generateSource__XBMC_ADDONS_KODIDEVKIT_SRC_SHARED_instances_h import *
from src.generateSource__XBMC_ADDONS_KODIDEVKIT_SRC_SHARED_sharedgroups_h import *
from src.tools.helper_Log import *


#===============================================================================
def GenerateSourceParts(options, data):
    Log.PrintGroupStart("Generate source code parts")

    # Generate xbmc/addons/kodi-dev-kit/src/shared/DirectData.h
    GenerateSource__XBMC_ADDONS_KODIDEVKIT_SRC_SHARED_directdata_h(options)

    # Generate automatic code inside xbmc/addons/interface/interface.cpp and .h
    GenerateSource__XBMC_ADDONS_INTERFACE_interface_cpp_h(options)

    # Generate automatic code inside xbmc/addons/kodi-dev-kit/src/addon/core/addon_control.cpp
    GenerateSource__XBMC_ADDONS_KODIDEVKIT_SRC_ADDON_CORE_addon_control_cpp(options)

    # This generate automatic code inside xbmc/addons/kodi-dev-kit/src/shared/Instances.h
    GenerateSource__XBMC_ADDONS_KODIDEVKIT_SRC_SHARED_instances_h(options)

    # Update xbmc/addons/kodi-dev-kit/src/shared/SharedGroups.h
    GenerateSource__XBMC_ADDONS_KODIDEVKIT_SRC_SHARED_sharedgroups_h(options)


# ===============================================================================
def GenerateCMakeParts(options):
    Log.PrintGroupStart("Generate cmake parts")

    # Generate all the "CMakeLists.txt" files in xbmc/addons/kodi-dev-kit/include/kodi
    GenerateCMake__XBMC_ADDONS_KODIDEVKIT_INCLUDE_KODI_all_files(options)

    # Generate the "CMakeLists.txt" in xbmc/addons/kodi-dev-kit/src/shared
    GenerateCMake__XBMC_ADDONS_KODIDEVKIT_SRC_SHARED_cmakelists_txt(options)

    # Generate all the "CMakeLists.txt" files in xbmc/addons/kodi-dev-kit/src/addon
    GenerateCMake__XBMC_ADDONS_KODIDEVKIT_SRC_ADDON_all_files(options)

    # This call generate the "CMakeLists.txt" in xbmc/addons/interface
    GenerateCMake__XBMC_ADDONS_INTERFACE_all_files(options)

    # Generate Kodi's cmake system related include files to find related parts
    GenerateCMake__CMAKE_TREEDATA_COMMON_addon_dev_kit_txt(options)
    GenerateCMake__CMAKE_TREEDATA_COMMON_addon_interface_txt(options)


# ===============================================================================
if __name__ == "__main__":
    # parse command-line options
    disc = """\
This utility autogenerate the interface between Kodi and addon.
It is currently still in the initial phase and will be expanded in the future.
"""
    parser = OptionParser(description=disc)
    parser.add_option(
        "-f",
        "--force",
        action="store_true",
        dest="force",
        default=False,
        help="Force the generation of auto code",
    )
    parser.add_option(
        "-d",
        "--debug",
        action="store_true",
        dest="debug",
        default=False,
        help="Add debug identifiers to generated files",
    )
    parser.add_option(
        "-c",
        "--commit",
        action="store_true",
        dest="commit",
        default=False,
        help="Create automatic a git commit about API changes (WARNING: No hand edits should be present before!)",
    )
    (options, args) = parser.parse_args()

    Log.Init(options)
    Log.PrintMainStart(options)

    ##----------------------------------------------------------------------------
    #
    Log.PrintGroupStart("Auto generation of addon interface code started")

    if os.path.isdir('./tmp'):
      shutil.rmtree('./tmp')

    ScanUsedAPI(options)
    ScanAllHdlVoidPointer(options)
    ScanAllEnums(options)

    callbacks = FindCallbackUsePlaces(options)
    data = interface_code_generator(options, callbacks)

    api = 1
    while True:
        dl_gen = LibraryDLGenerator(options, api)
        if not dl_gen.Generate():
          break
        api += 1

    ##----------------------------------------------------------------------------
    # Source code generation
    GenerateSourceParts(options, data)

    ##----------------------------------------------------------------------------
    # CMake generation
    GenerateCMakeParts(options)

    ##----------------------------------------------------------------------------
    # Commit GIT changes generation (only makes work if '-c' option is used)
    if options.commit:
        Log.PrintGroupStart("Git update")
        CommitChanges(options)



##===============================================================================
#def GenerateSourceParts(options, data):
  #print('.-------------------------------------------------------------------------------')
  #print('· Generate source code parts ...')

  ## Generate xbmc/addons/kodi-dev-kit/src/shared/DirectData.h
  #generate__SOURCE__xbmc_addons_kodidevkit_src_shared_DIRECTDATA_H(options)

  ### Generate automatic code inside xbmc/addons/interface/interface.cpp and .h
  #generate__SOURCE__xbmc_addons_interface_INTERFACE_CPP_H(options.force)

  ### Generate automatic code inside xbmc/addons/kodi-dev-kit/src/addon/core/addon_control.cpp
  #generate__SOURCE__xbmc_addons_kodidevkit_src_addon_core_ADDON_CONTROL_CPP(options.force)

  ## This generate automatic code inside xbmc/addons/kodi-dev-kit/src/shared/Instances.h
  #generate__SOURCE__xbmc_addons_kodidevkit_src_shared_INSTANCES_H(options)

  ## Update xbmc/addons/kodi-dev-kit/src/shared/SharedGroups.h
  #generate__SOURCE__xbmc_addons_kodidevkit_src_shared_SHAREDGROUPS_H(options.force)












  ### This generate all automatic code inside xbmc/addons/kodi-dev-kit/src/shared/kodi/
  ##generate__SOURCE__xbmc_addons_kodidevkit_src_shared_kodi_ALL_FILES(options.force)

  ### Generate automatic code inside xbmc/addons/kodi-dev-kit/src/addon/api_in
  ##generate__SOURCE__xbmc_addons_kodidevkit_src_addon_api_in_kodi_ALL_FILES(options.force)

  ### Generate automatic code inside xbmc/addons/kodi-dev-kit/src/addon/api_out
  ##generate__SOURCE__xbmc_addons_kodidevkit_src_addon_api_out_kodi_ALL_FILES(options.force)

  ### Generate automatic code inside xbmc/addons/interface/api_in
  ##generate__SOURCE__xbmc_addons_interface_api_in_kodi_ALL_FILES(options.force)

  ### Generate automatic code inside xbmc/addons/interface/api_out
  ##generate__SOURCE__xbmc_addons_interface_api_out_kodi_ALL_FILES(options.force)


  ### Generate or update all files in xbmc/addons/interface/api
  ### WARNING This must be called last as it scan other created dirs
  ##generate__SOURCE__xbmc_addons_interface_api_ALL_FILES(options.force)


##===============================================================================
#def print_error(msg):
  #print('Error: %s\nSee --help for usage.' % msg)

##===============================================================================
#if __name__ == "__main__":
  ## parse command-line options
  #disc = """
  #This utility autogenerate the interface between Kodi and addon.
  #"""
  #parser = OptionParser(description=disc)
  #parser.add_option(
      #'-f',
      #'--force',
      #action='store_true',
      #dest='force',
      #default=False,
      #help='force the generation of auto code')
  #parser.add_option(
      #'-d',
      #'--debug',
      #action='store_true',
      #dest='debug',
      #default=False,
      #help='Add debug indendifiers to generated files')
  #(options, args) = parser.parse_args()

  #print('Auto generation of addon interface code started ...')

  #if os.path.isdir('./tmp'):
    #shutil.rmtree('./tmp')

  #GetHighestUsedAPI() # Init it
  #ScanAllHdlVoidPointer()
  #ScanAllEnums()

  #callbacks = find_callback_use_places(options)
  #data = interface_code_generator(options, callbacks)

  #api = 1
  #while True:
    #dl_gen = LibraryDLGenerator(options, api)
    #if not dl_gen.Generate():
      #break
    #api += 1

  ##-----------------------------------------------------------------------------
  ## Source code generation
  #data = ''
  #GenerateSourceParts(options, data)

  ###-----------------------------------------------------------------------------
  ## CMake generation
  ##GenerateCMakeParts(options)



##ScanAllHdlVoidPointer(kodi_dir)
##ScanAllFunctionAutoGenHelp(kodi_dir)

##generate__SOURCE__xbmc_addons_kodidevkit_src_shared_kodi_ALL_FILES(kodi_dir, force)
#####generate__SOURCE__xbmc_addons_kodidevkit_src_shared_kodi_addoninstance_INSTANCES_H(kodi_dir, force)
##generate__SOURCE__xbmc_addons_kodidevkit_src_addon_api_in__ALL_FILES(kodi_dir, force)








##generate__SOURCE__xbmc_addons_kodidevkit_src_addon_core_addon_control_cpp(kodi_dir, force)
##generate__SOURCE__xbmc_addons_interface_api(kodi_dir, force)
##generate__SOURCE__api_autogen(kodi_dir, force)

###generate__SOURCE__xbmc_addons_kodidevkit_src_shared_kodi(kodi_dir, force)


#>>>>>>> 59b1afa1dd (update)

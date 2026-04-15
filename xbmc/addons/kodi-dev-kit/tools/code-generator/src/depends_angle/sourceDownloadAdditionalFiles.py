# -*- coding: utf-8 -*-

#   Copyright (C) 2026 Team Kodi
#   This file is part of Kodi - https://kodi.tv
#
#   SPDX-License-Identifier: GPL-2.0-or-later
#   See LICENSES/README.md for more information.

# Global includes
import shutil, sys, os

try:
    import git
except ImportError:
    sys.exit('Please run `pip3 install gitpython`')

# Own includes
from .toolsGITworks import GitApplyCommit, GitGetVersion, GitCheckFileInRepo
from ..helper_Log import Log, Result
from ..helper_Tools import RemoveDirectoryTree, DownloadFile


class DownloadAdditionalFiles():
    '''
    Download additional files to add in our chromium/angle version.
    '''

    def __init__(self, settings):
        self.settings = settings

    def __del__(self) -> None:
        pass

    def Process(self):
        '''
        From outside called Process function where do all needed works on called sub
        functions in this class.
        '''

        Log.PrintGroupStart("Download additional used files")

        if not self.__download_gni_to_cmake__():
            return False
        if not self.__download_zlib__():
            return False
        if not self.__download_webkit_files__():
            return False

        return True

    def __download_gni_to_cmake__(self):
        '''
        Download needed script to translate the Chromium Ninja build files to
        our CMake needed system.

        This script becomes stored outside from our modified Angle code.

        Script taken from https://github.com/WebKit/WebKit/Source/ThirdParty/ANGLE/gni-to-cmake.py
        '''

        defaults = self.settings.defaults # Reduce line lengths
        options = self.settings.options

        webkit_gni_to_cmake_url = defaults.WEBKIT_FILES[0] + defaults.WEBKIT_FILES[1][0]
        webkit_gni_to_cmake_hash = defaults.WEBKIT_FILES[1][1]

        Log.PrintBegin("Get file: ")
        Log.PrintFollow("gni-to-cmake.py")

        path = os.path.join(options.buildpath, 'gni-to-cmake.py')
        path_own_version = os.path.join(defaults.SCRIPT_DIRECTORY, '/gni-to-cmake.py')
        if os.path.exists(path_own_version):
            self.settings.used_gni_to_cmake_path = path_own_version
            Log.PrintResult(Result.OWN_SET, "Own version present in script source")
            return True

        lastusedURL = ""
        if os.path.exists(path + ".url"):
            with open(path + ".url", 'r' ) as f:
                lastusedURL = f.read()
                lastusedURL = lastusedURL.strip(' \n')

        if os.path.exists(path) and lastusedURL == webkit_gni_to_cmake_url and not options.force:
            self.settings.used_gni_to_cmake_path = path
            Log.PrintResult(Result.OWN_SET, "Correct version already present in build dir")
            return True

        Log.PrintResult(Result.SEE_BELOW)

        Log.PrintBegin(" - Download needed ")
        Log.PrintFollow("Used commit version {}".format(defaults.WEBKIT_BUILDSYSTEM_COMMIT))
        Log.PrintResult(Result.EMPTY)

        path = webkit_gni_to_cmake_url
        
        # Added for progress bar tests, unset for this test use
        #path = "https://mirrors.kodi.tv/releases/windows/win64/kodi-21.3-Omega-x64.exe"
        #path = "https://mirrors.kodi.tv/releases/windows/win64/kodi-21.3-Omega-x64.wrong"
        #path = "http://download.thinkbroadband.com/100MB.zip"

        ret = DownloadFile(path, options.buildpath, False, webkit_gni_to_cmake_hash)
        if not ret[0]:
            return False
        self.settings.used_gni_to_cmake_path = ret[1]

        # Make text file where contains last used url, needed for check on next call
        with open(self.settings.used_gni_to_cmake_path + ".url", 'w', newline='\n') as f:
            f.write(path)
            f.close()

        return True

    def __download_zlib__(self):
        ''' 
        Download needed zlib from Chromium where is only via git clone available.
        '''

        defaults = self.settings.defaults # Reduce line lengths
        path = os.path.join(self.settings.options.sourcepath, 'third_party', 'zlib')
        cmakelists = os.path.join(path, "CMakeLists.txt")
        git_path =os.path.join(path, ".git")

        Log.PrintBegin("Remove git submodule: ")
        Log.PrintFollow("/third_party/zlib")

        # Remove git submodule 
        repo = git.Repo.init(self.settings.options.sourcepath)
        ret = GitCheckFileInRepo(repo, path)
        if os.path.exists(path) and not os.path.isfile(path) and not os.listdir(path):
            index = git.index.base.IndexFile(repo)
            values = { "f": True,
                       "ignore-unmatch": True }
            index.remove(path, **values)

            Log.PrintResult(Result.OK)
        else:
            Log.PrintResult(Result.ALREADY_DONE)

        # Create path new if removed, as now removed from git before
        if not os.path.exists(path):
            os.makedirs(path)

        Log.PrintBegin("Get repo directory: ")
        Log.PrintFollow("/third_party/zlib")

        if self.settings.options.force:
            RemoveDirectoryTree(path)

        if not os.path.exists(cmakelists):
            # Check we have git version higher or equal 2.49 to allow "git clone --revision=XXX"
            version = GitGetVersion()
            if version[0] >= 3 or (version[0] == 2 and version[1] >= 49):
                repo = git.Repo.clone_from(defaults.THIRDPARTY_ZLIB["URL"], 
                                           path,
                                           multi_options = [ "--revision=" + defaults.THIRDPARTY_ZLIB["SHA256"],
                                                             "--depth=1",
                                                             "--single-branch" ])
            else:
                repo = git.Repo.init(path)
                remote = git.remote.Remote(repo, "origin")
                if not remote.exists():
                    remote.create(repo, "origin", defaults.defaults.THIRDPARTY_ZLIB["URL"])
                    remote = git.remote.Remote(repo, "origin")

                options = {}
                options["depth"] = "1"
                remote.fetch(defaults.defaults.THIRDPARTY_ZLIB["SHA256"], 
                             verbose = self.settings.options.debug, 
                             **options)
                repo.git.checkout(defaults.defaults.THIRDPARTY_ZLIB["SHA256"])

        if not os.path.exists(cmakelists):
            Log.PrintFatal("Clone about zlib failed")
            return False

        # Remove the .git part as we include the source in our Angle package
        if os.path.exists(git_path):
            shutil.rmtree(git_path)

            Log.PrintResult(Result.OK)

            text = (
                "Implement the dependency source about zlib direct into here and not use the gitmodules like before.\n"
                "This needed on Kodi's code style where all the code should included in one repo."
            )
            if not GitApplyCommit(self.settings, 'Add third_party zlib source', text):
                return False
        else:
            Log.PrintResult(Result.ALREADY_DONE)

        return True


    def __download_webkit_files__(self):
        '''
        Download from us needed webkit files.
        '''

        Log.PrintBegin("Download needed WebKit files")
        Log.PrintResult(Result.SEE_BELOW)

        defaults = self.settings.defaults # Reduce line lengths
        options = self.settings.options

        base_url = defaults.WEBKIT_FILES[0]
        for e in defaults.WEBKIT_FILES[2]:
            Log.PrintBegin(" - Download: ")
            Log.PrintFollow(e[1])

            source_path = base_url + e[0]
            target_path = options.sourcepath + e[1]
            source_hash = e[2]

            if options.force:
                try: 
                    os.remove(target_path)
                except FileNotFoundError:
                    pass

            if not os.path.exists(target_path):
                Log.PrintResult(Result.EMPTY)
                DownloadFile(source_path, target_path, True, source_hash)
            else:
                Log.PrintResult(Result.ALREADY_DONE)

        commit = "Apply from WebKit taken files"
        text = (
            f"This commit contains downloads about unchanged files from {base_url} to use in our Angle version."
            )

        if not GitApplyCommit(self.settings, commit, text):
            return False

        return True

    def __download_repo_sub_path__(self, url, dest):

        ret = False


        return ret


# -*- coding: utf-8 -*-

#   Copyright (C) 2026 Team Kodi
#   This file is part of Kodi - https://kodi.tv
#
#   SPDX-License-Identifier: GPL-2.0-or-later
#   See LICENSES/README.md for more information.

# Global includes
import os, re, sys

try:
    import git
except ImportError:
    sys.exit('Please run `pip3 install gitpython`')

# Own includes
from .toolsGITworks import GitApplyCommit
from ..helper_Log import Log, Result


class AngleSetupFilesChange():
    """
    Peform changes on cloned Angle his own files where needed.
    """

    settings = None

    def __init__(self, settings):
        self.settings = settings

    def __del__(self) -> None:
        pass

    def Process(self):
        """
        Process files where we need to change in Angle.
        """

        Log.PrintGroupStart("Checking about cloned Angle own files to change")

        if not self.__change_git_ignore__():
            return False
        if not self.__remove_not_wanted_files__():
            return False

        return True

    def __change_git_ignore__(self):
        """
        Change in chromium used .gitignore to our usage,
        """

        defaults = self.settings.defaults # Reduce line lengths

        Log.PrintBegin("Change file: ")
        Log.PrintFollow(".gitignore")

        gitignore = os.path.join(self.settings.options.sourcepath, '.gitignore')
        if not os.path.exists(gitignore):
            Log.PrintFatal("Code not include this file: {}!".format(gitignore.sourcepath))
            return False

        if os.path.exists(gitignore):
            with open(gitignore, 'r' ) as f:
                content = f.read()
                content_new = content
                for entry in defaults.GITIGNORE_ALLOW_PARTS:
                    content_new = re.sub('\n{}'.format(entry), '\n# {} # Commented out for Kodi use'.format(entry), content_new, flags = re.M)

                if defaults.GITIGNORE_IGNORE_PARTS:
                    content_added = ''
                    for entry in defaults.GITIGNORE_IGNORE_PARTS:
                        string = '{} # Added for Kodi use'.format(entry)
                        if not string in content_new:
                            content_added += string + '\n'
                    if content_added:
                        content_new += '\n' + content_added
                f.close()
                if content_new != content:
                    with open(gitignore, 'w', newline='\n') as f:
                        f.write(content_new)
                        f.close()

                if content_new != content:
                    Log.PrintResult(Result.OK)
                elif defaults.GITIGNORE_ALLOW_PARTS:
                    Log.PrintResult(Result.ALREADY_DONE)
        else:
            Log.PrintFollow(".gitignore not present")
            Log.PrintResult(Result.IGNORED)

        if not GitApplyCommit(self.settings, 'Change .gitignore to match our usage', changed_files=[ '.gitignore' ]):
            return False

        return True


    def __remove_not_wanted_files__(self):
        """
        Remove .gitmodules if present (we not use), can be become present by sync.
        to newer angle version from chromium.
        """

        files_removed = []
        for entry in self.settings.defaults.ANGLE_FILES_TO_REMOVE:
            Log.PrintBegin("Remove file: ")
            Log.PrintFollow(entry)

            file = os.path.join(self.settings.options.sourcepath, entry)
            if os.path.exists(file):
                os.remove(file)
                files_removed.append(entry)

                Log.PrintResult(Result.OK)
            else:
                Log.PrintResult(Result.ALREADY_DONE)

        if not GitApplyCommit(self.settings, "Remove not wanted files from origin", changed_files=files_removed):
            return False

        return True

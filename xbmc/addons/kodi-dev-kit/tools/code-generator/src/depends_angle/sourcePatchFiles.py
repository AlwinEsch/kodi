# -*- coding: utf-8 -*-

#   Copyright (C) 2026 Team Kodi
#   This file is part of Kodi - https://kodi.tv
#
#   SPDX-License-Identifier: GPL-2.0-or-later
#   See LICENSES/README.md for more information.

# Global includes
import os, sys

try:
    from patch_ng import fromfile as PatchFromFile
except ImportError:
    sys.exit('Please run `pip3 install patch_ng`')

# Own includes
from .toolsGITworks import GitApplyCommit
from ..helper_Log import Log, Result
from ..helper_Tools import DownloadFile


class AngleApplyPatches():
    """
    Perform the apply of own patches to the forked chromium/angle.

    It use the patches where stored on:
    - kodi-dev-kit/tools/code-generator/src/depends_angle/add_to_angle_source/patches

    They must have a file ending with .patch or .diff and must be in numerical flow.
    """

    settings = None

    def __init__(self, settings):
        self.settings = settings

    def __del__(self) -> None:
        pass

    def Process(self):
        Log.PrintGroupStart("Apply needed patches")

        patch_directory = os.path.join(self.settings.defaults.SCRIPT_DIRECTORY, 'add_to_angle_source', 'patches')
        for name in os.listdir(patch_directory):
            if not name.endswith('.patch') and not name.endswith('.diff'):
                continue

            Log.PrintBegin(" - Found patch: ")
            Log.PrintFollow(name)

            path = os.path.join(patch_directory, name)
            patch = PatchFromFile(path)
            if not patch.apply(root=self.settings.options.sourcepath):
                Log.PrintFatal("Failed to apply patch {}".format(path))
                return False
            else:
                Log.PrintResult(Result.OWN_SET, "Patch applied")

        GitApplyCommit(self.settings, "Applied code change patches to Angle")

        return True


# -*- coding: utf-8 -*-

#   Copyright (C) 2021-2022 Team Kodi
#   This file is part of Kodi - https://kodi.tv
#
#   SPDX-License-Identifier: GPL-2.0-or-later
#   See LICENSES/README.md for more information.

import code_generator
import os, sys

def read_file(name, normalize=True):
    """Read a file."""
    try:
        with open(name, "r", encoding="utf-8") as f:
            # read the data
            data = f.read()
            if normalize:
                # normalize line endings
                data = data.replace("\r\n", "\n")
            return data
    except IOError as e:
        (errno, strerror) = e.args
        code_generator.LAST_ERROR = "Failed to write file {}: {}".format(name, strerror)
        return False


def write_file(name, data, clang=False):
    """Write a file."""
    try:
        os.makedirs(os.path.dirname(name), exist_ok=True)
        with open(name, "w", encoding="utf-8") as f:
            # write the data
            if sys.version_info.major == 2:
                f.write(data.decode("utf-8"))
            else:
                f.write(data)
    except IOError as e:
        (errno, strerror) = e.args
        code_generator.LAST_ERROR = "Failed to write file {}: {}".format(name, strerror)
        return False
    if clang:
        os.system("clang-format -style=file -i {}".format(name))

    return True

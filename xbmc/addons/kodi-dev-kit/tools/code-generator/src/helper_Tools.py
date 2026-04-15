# -*- coding: utf-8 -*-

#   Copyright (C) 2026 Team Kodi
#   This file is part of Kodi - https://kodi.tv
#
#   SPDX-License-Identifier: GPL-2.0-or-later
#   See LICENSES/README.md for more information.

# Global includes
import importlib, importlib.util
import re, subprocess, sys, os
import urllib.request as urllib2
import urllib.parse as urlparse
import hashlib

try:
    from rich import console, progress
except ImportError:
    sys.exit('Please run `pip3 install rich`')

try:
    import git
except ImportError:
    sys.exit('Please run `pip3 install gitpython`')

# Own includes
from .helper_Log import *

def ExceptionInfo(ex):
    excpt_type = type(ex).__name__
    excpt_file = ex.__traceback__.tb_frame.f_code.co_filename
    excpt_func = ex.__traceback__.tb_frame.f_code.co_name
    excpt_lineno = ex.__traceback__.tb_lineno
    excpt_args = ex.args

    message = (
        f"An exception of type \"{excpt_type}\" occurred.\n"
        f"File: {excpt_file} Function: {excpt_func} Line: {excpt_lineno}\n"
        f"Arguments:\n"
        f"{excpt_args!r}"
    )
    Log.PrintFatal(message)
    return


def RemoveDirectoryTree(start_directory: str, first: bool = True):
    """Recursively and permanently removes the specified directory, all of its
    subdirectories, and every file contained in any of those folders."""
    for name in os.listdir(start_directory):
        path = os.path.join(start_directory, name)
        if os.path.isfile(path):
            os.remove(path)
        else:
            RemoveDirectoryTree(path, False)

    if not first:
        os.rmdir(start_directory)


def DownloadFile(url, dest, inlude_target_filename = False, hash = None):
    """ 
    Download and save a file specified by url to dest directory,
    """

    if not url:
        Log.PrintFatal("No url set for download")
        return False

    if not dest:
        Log.PrintFatal("No target dir set for download")
        return False

    ret = False
    filename = ""
    progressbar = None

    try:
        u = urllib2.urlopen(url)

        scheme, netloc, path, query, fragment = urlparse.urlsplit(url)
        filename = os.path.basename(path)
        if not filename:
            filename = 'downloaded.file'

        if not inlude_target_filename:
            filepath = os.path.join(dest, filename)
            path = dest
        else:
            filepath = dest
            path = os.path.dirname(dest)

        dir_present = os.path.isdir(path)
        if os.path.exists(path) and not dir_present:
            Log.PrintFatal("Target dir \"{}\" ref to a present file")
            return False
        if not dir_present:
            os.makedirs(path)

        # Remove Old
        if os.path.exists(filename):
            os.remove(filename)

        with open(filepath, 'wb') as f:
            meta = u.info()
            meta_func = meta.getheaders if hasattr(meta, 'getheaders') else meta.get_all
            meta_length = meta_func("Content-Length")
            file_size = None
            if meta_length:
                file_size = int(meta_length[0])

            Log.PrintBegin(" - Get: ")
            Log.PrintFollow("{0} Bytes: {1}".format(filename, file_size))
            Log.PrintResult(Result.EMPTY)

            dt_string = datetime.utcnow().strftime("%d/%m/%Y %H:%M:%S")

            progressbar = progress.Progress(
                # *progress.Progress.get_default_columns(),
                progress.TextColumn("[{}{}{}] ".format(Log.style.MAGENTA, dt_string, Log.style.RESET)),
                progress.SpinnerColumn(),
                progress.TextColumn("[progress.description]{task.description}"),
                progress.BarColumn(),
                progress.TextColumn("[progress.percentage]{task.percentage:>3.0f}%"),
                "eta",
                progress.TimeRemainingColumn(),
                progress.TextColumn("{task.fields[message]}"),
                console=console.Console(),
                transient=False,
            )
            progressbar.start()

            active_task = progressbar.add_task(
                description = "Downloading",
                total = file_size,
                message = "",
            )

            file_size_dl = 0
            block_sz = 8192
            while True:
                buffer = u.read(block_sz)
                if not buffer:
                    break

                file_size_dl += len(buffer)
                f.write(buffer)

                status = "[bright_black]{} ".format(file_size_dl)
                if file_size:
                    status += "of {} ".format(file_size)
                status += "bytes"

                progressbar.update(
                    task_id = active_task,
                    completed = file_size_dl,
                    message = status,
                )

        if hash:
            with open(filepath, 'r') as f:
                progressbar.update(
                    task_id = active_task,
                    message = f"[bright_black]Testing SHA512 hash",
                )

                content = f.read()

                # Create a new sha512 hash object
                sha512_hash = hashlib.sha512()

                # Update the hash object with the bytes of the message
                sha512_hash.update(content.encode('utf-8'))
    
                # Return the hexadecimal representation of the digest
                computed_hash = sha512_hash.hexdigest()

                if computed_hash != hash:
                    Log.PrintFatal("SHA512 hash not match download file")
                    return False

        progressbar.update(
            task_id = active_task,
            message = f"[bright_black]Done",
        )
        progressbar.stop()

        ret = True

    except urllib2.HTTPError as err:
        Log.PrintFatal("Failed to download {} Error code - {} - {}".format(err.url, err.code, err.reason))

    except KeyboardInterrupt:
        progressbar.update(
            task_id = active_task,
            message = f"[bright_red]Stopped",
        )
        progressbar.stop()
        Log.PrintFatal("Download stopped by keyboard interrupt")

    # Delete on errors incomplete file if exists
    if not ret and os.path.exists(filename):
        os.remove(filename)

    return [ ret, filepath ]


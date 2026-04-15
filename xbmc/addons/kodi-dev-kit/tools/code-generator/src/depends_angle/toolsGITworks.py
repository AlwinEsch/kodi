# -*- coding: utf-8 -*-

#   Copyright (C) 2026 Team Kodi
#   This file is part of Kodi - https://kodi.tv
#
#   SPDX-License-Identifier: GPL-2.0-or-later
#   See LICENSES/README.md for more information.

"""
Helper script about Git related works

@todo currently is this file fixed to use in main "code_generator__addon_depends_angle.py".
Rework this to become available also on other main scripts, e.g. code_generator__addon_devkit.py 
Depends mainly to settings, make chenge to add global settings where relates to all main scripts.
"""

# Global includes
from datetime import datetime, timezone
import os, re, subprocess, sys

try:
    from rich import console, progress
except ImportError:
    sys.exit('Please run `pip3 install rich`')

try:
    import git
except ImportError:
    sys.exit('Please run `pip3 install gitpython`')

# Own includes
from ..helper_Log import Log, Result
from ..helper_Tools import ExceptionInfo


def GitGetVersion():
    '''
    Get OS installed GIT version.

    @return The version of installed git as [ Major, Minor, Fix ]
    '''

    version = git.Git().execute([ "git", "-v"])
    r = re.search('(\\d+)\\.(\\d+)\\.(\\d+)+', version)
    return [ int(r.group(1)), int(r.group(2)), int(r.group(3)) ]


def GitValidateRepositoryURL(url):
    """
    Validate Git repository URL with multiple checks

    @param  url (str): Git repository URL

    @return dict: Validation result
    """

    ## Regex validation patterns
    patterns = {
        'https': r'^https://.*\.git$',
        'ssh': r'^git@.*:.*\.git$',
        'git': r'^git://.*\.git$'
    }

    ## Validation result structure
    result = {
        'is_valid': False,
        'protocol': None,
        'error': ''
    }

    ## Check URL format
    if not url:
        result['errors'] = 'Empty URL'
        return result

    ## Regex validation
    for protocol, pattern in patterns.items():
        if re.match(pattern, url):
            result['protocol'] = protocol
            break

    if not result['protocol']:
        result['errors'] = 'Invalid URL format'
        return result

    ## Network accessibility check
    try:
        subprocess.run(
            ['git', 'ls-remote', url],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=10,
            check=True
        )
        result['is_valid'] = True
    except subprocess.CalledProcessError:
        result['errors'] = 'Repository inaccessible'
    except subprocess.TimeoutExpired:
        result['errors'] = 'Connection timeout'

    return result

def GitCheckFileInRepo(repo, filePath):
    '''
    Check Git repo about presence of file.

    @param repo         A gitPython Repo object   
    @param filePath     The full path to the file from the repository root
    @return             true if file is found in the repo at the specified path, false otherwise
    '''

    pathdir = os.path.dirname(filePath)

    # Build up reference to desired repo path
    rsub = repo.head.commit.tree

    for path_element in pathdir.split(os.path.sep):
        # If dir on file path is not in repo, neither is file. 
        try: 
            rsub = rsub[path_element]

        except KeyError: 
            return False

    return (filePath in rsub)


def GitApplyCommit(settings, commit, description = "", changed_files = None):
    """
    Check source and apply Git commit if changed.

    Inside "kodi-dev-kit/build/angle/patches" becomes then all patches stored
    to see performed changes within angle.

    @param settings         The settings used in this app
    @param commit           The main name about the commit to add
    @param description      [opt] Additional detailed test to add on commit
    @param changed_files    [opt] A list of changed files, e.g. [ ".gitignore", "README.md" ]
                            If unused all files becomes checked
    @return True If work performed correct, False in case of error
    """

    if changed_files is None:
        changed_files = []

    patch_directory = os.path.join(settings.options.buildpath, "patches")
    remote_repo = f"remotes/upstream-google/chromium/{settings.options.version}"

    try:
        r = git.Repo(settings.options.sourcepath)
    except git.InvalidGitRepositoryError:
        Log.PrintError(f"Not a valid Git-Repository: {settings.options.sourcepath}")
        return False


    #---------------------------------------------------------------------------
    # Check repo contains changes
    #

    try:
        if changed_files:
            for f in changed_files:
                r.git.add(f)
        else:
            r.git.add('-A')

        diff = r.index.diff("HEAD")

        # If nothing changed return
        if not diff:
            return True

    except git.GitCommandError as ex:
        GITExceptionInfo(ex)
        return False

    #---------------------------------------------------------------------------
    # Inform the user and apply commit
    #

    # Make first line of commit to the console
    Log.PrintBegin(" - Apply GIT commit: ")
    Log.PrintFollow(commit.split('\n', 1)[0])
    Log.PrintResult(Result.EMPTY)

    # Print now a list of changed files
    first_done = False
    for x in diff:
        if not first_done:
            Log.PrintBegin("   Changed files: ")
            first_done = True
        else:
            Log.PrintBegin("                  ")
        Log.PrintFollow(x.b_path)
        Log.PrintResult(Result.EMPTY)

    # Get amount of commits added, needed to make identifier number on 
    # autogenerated commits
    count = int(r.git.rev_list('--count', 'HEAD'))

    # Time where the commit was created by script
    current_time = datetime.now(timezone.utc).strftime("%d/%m/%Y %H:%M:%S")

    # If we become description add line break to have empty line between both
    commit += "\n" if len(description) > 0 else ""

    # Commit our changes
    text = (
        f"[auto][cmake][{count:03n}] {commit}\n"
        f"{description}"
        f"\n"
        f"Autogenerated changes to add CMakeSupport into Angle\n"
        f"\n"
        f"This commit automatic generated by script 'kodi-dev-kit/tools/code_generator__addon_depends_angle.py'.\n"
        f"Commit performed date (UTC): {current_time}\n"
    )                   
   
    try:
        r.index.commit(text, author = settings.git_author, committer = settings.git_committer)
    except git.GitCommandError as ex:
        GITExceptionInfo(ex)
        return False

    #---------------------------------------------------------------------------
    # Make and update a list about commits as patches
    # Nice as overview what is changed and what becomes pushed
    #

    if not os.path.exists(patch_directory):
        os.makedirs(patch_directory)

    # Remove currently listed patches
    #
    # As we never know exactly how the patches are named on next function call
    # and to have numbering correct, must we always get all commits and to
    # delete the old ones before.
    #
    # NOTE: Used folder present in the build directory and is only used for the
    #       patches created here.
    #      
    for f in os.listdir(patch_directory):
        p = os.path.join(patch_directory, f)
        if os.path.isfile(p) and (f.endswith('.patch') or f.endswith('.diff')):
            os.remove(p)

    try:
        r.git.format_patch(remote_repo, '-o', patch_directory)
    except git.GitCommandError as ex:
        Log.PrintWarning(f"Failed to get patches and to store on {patch_directory}. Further works becomes continued.")
        GITExceptionInfo(ex)
        return False

    return True


class GitRemoteProgress(git.RemoteProgress):
    """
    A for gitpython used process class where show on console a progress bar.

    @note This function can't be used direct! Only to give by some function calls of
          gitpython as callback and to become on bigger works a visible progress bar.

    @param git.RemoteProgress The caller inside gitpython where calls this.
    """

    OP_CODES = [
        "BEGIN",
        "CHECKING_OUT",
        "COMPRESSING",
        "COUNTING",
        "END",
        "FINDING_SOURCES",
        "RECEIVING",
        "RESOLVING",
        "WRITING",
    ]
    OP_CODE_MAP = {
        getattr(git.RemoteProgress, _op_code): _op_code for _op_code in OP_CODES
    }

    firstMessagePresented = False

    def __init__(self) -> None:
        super().__init__()
        self.progressbar = progress.Progress(
            progress.SpinnerColumn(),
            progress.TextColumn("[progress.description]{task.description}"),
            progress.BarColumn(),
            progress.TextColumn("[progress.percentage]{task.percentage:>3.0f}%"),
            "eta",
            progress.TimeRemainingColumn(),
            progress.TextColumn("{task.fields[message]}"),
            console = console.Console(),
            transient=False,
        )
        self.progressbar.start()
        self.active_task = None

    def __del__(self) -> None:
        if not self.firstMessagePresented:
            Log.PrintBegin("   Fetching progress ...")
            Log.PrintResult(Result.OK)
        self.progressbar.stop()

    @classmethod
    def get_curr_op(cls, op_code: int) -> str:
        """Get OP name from OP code."""
        # Remove BEGIN- and END-flag and get op name
        op_code_masked = op_code & cls.OP_MASK
        return cls.OP_CODE_MAP.get(op_code_masked, "?").title()

    def update(
        self,
        op_code: int,
        cur_count: str | float,
        max_count: str | float | None = None,
        message: str | None = "",
    ) -> None:
        # Start new bar on each BEGIN-flag
        if op_code & self.BEGIN:
            if not self.firstMessagePresented:
                self.firstMessagePresented = True

            self.curr_op = self.get_curr_op(op_code)
            self.active_task = self.progressbar.add_task(
                description=self.curr_op,
                total=max_count,
                message=message,
            )

        self.progressbar.update(
            task_id=self.active_task,
            completed=cur_count,
            message=message,
        )

        # End progress monitoring on each END-flag
        if op_code & self.END:
            self.progressbar.update(
                task_id=self.active_task,
                message=f"[bright_black]{message}",
            )


def GITExceptionInfo(ex):
    """
    Function to give a small backtrace about GIT error to know where here and who was the caller.

    Only two backtraces are show:
    - First the place where brought the exception
    - And as second the place where called the function who created exception

    Used to find easier the place where created a problem and as the used Git calls here, 
    normally only have a direct call without some Git actions before.

    @param ex   Exception class to use
    """

    caller = ex.__traceback__.tb_frame.f_back

    excpt_type = type(ex).__name__
    excpt_args = ex.args

    excpt_1_file = os.path.split(ex.__traceback__.tb_frame.f_code.co_filename)[1]
    excpt_1_func = ex.__traceback__.tb_frame.f_code.co_name
    excpt_1_lineno = ex.__traceback__.tb_lineno

    excpt_2_file = os.path.split(caller.f_code.co_filename)[1]
    excpt_2_func = caller.f_code.co_name
    excpt_2_lineno = caller.f_lineno

    message = (
        f"An exception of type {excpt_type} occurred.\n"
        f"Arguments:\n"
        f"  {excpt_args!r}\n"
        f"Traceback (1st Exception place and 2nd Function caller)\n"
        f" - File: {excpt_1_file} - Function: {excpt_1_func}(...) - Line: {excpt_1_lineno}\n"
        f" - File: {excpt_2_file} - Function: {excpt_2_func}(...) - Line: {excpt_2_lineno}"
    )
    Log.PrintFatal(message)
    return

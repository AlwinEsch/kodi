# -*- coding: utf-8 -*-

#   Copyright (C) 2026 Team Kodi
#   This file is part of Kodi - https://kodi.tv
#
#   SPDX-License-Identifier: GPL-2.0-or-later
#   See LICENSES/README.md for more information.

# Global includes
from __future__ import annotations
from datetime import datetime
import os, re, sys, time

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
from .toolsGITworks import GitRemoteProgress, GitValidateRepositoryURL, GITExceptionInfo


class AngleSetupOriginalSourceCode():
    '''
    Peform the needed GIT parts about our used Angle to clone it and add
    needed upstream.
    '''

    settings = None
    '''
    The place where with constructor from from caller given settings becomes
    stored and to have available in all functions here.
    '''

    cloned = False
    '''
    Value to know code was cloned in the call, used to prevent some update
    processes as the code then up to date.
    '''

    force_works = False
    '''
    If True to force to override the time delays for git works
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

        Log.PrintGroupStart("Checking about used GIT branch")

        # Download our angle code if was not available before
        if not self.__download_used_own_angle__():
            return False

        # Get after repo is available his class to use from
        self.repo = git.Repo(self.settings.options.sourcepath)

        # Check by user was another branch set as before and to force updates
        # for this case.
        branch_active = int(str(self.repo.active_branch).split("/")[1]) if '/' in str(self.repo.active_branch) else self.repo.active_branch
        branch_needed = int(self.settings.options.version)
        if branch_active != branch_needed:
            Log.PrintBegin("Found branch version change: ")
            Log.PrintFollow(f"Current: {branch_active}, needed {branch_needed}")
            self.force_works = True
            Log.PrintResult(Result.EMPTY)
        else:
            self.force_works = False

        # Do all the git repository works below to have our code up to date
        # and all remotes available.
        #
        # Note:
        # We use two upstreams, one about the original chromium/angle and one
        # where we have our released codes.
        #
        # Calls here have further a time check to make, normally becomes his
        # works only done in a 24 hours interval to prevent to much online
        # works. In case of some changed can it be forced to make earlier.
        if not self.__download_upstream_angle_own__():
            return False
        if not self.__download_upstream_angle_chromium__():
            return False
        if not self.__update_cloned_own_angle__():
            return False
        if not self.__checkout_needed_branch__():
            return False
        if not self.__merge_google_to_our_main__():
            return False

        # If a force was done set the date, force_works can come checks or
        # if something done, about some reasons, a update before.
        if self.force_works:
            # As here was the last repo update in calls set in settings his
            # time to have again a time delay for the next update.
            self.settings.SetAngleRemoteUpdate(datetime.now())

        # We confirm with the calls below that the repo is what we want and
        # to set in settings the needed SHA and last chromium/angle commit
        # date.
        Log.PrintBegin("Check needed values for programming code generation:")
        Log.PrintResult(Result.SEE_BELOW)

        if not self.__get_angle_confirm_branch_name__():
            return False
        if not self.__get_angle_last_commit_sha__():
            return False
        if not self.__get_angle_last_commit_date__():
            return False

        return True


    def __download_used_own_angle__(self):
        '''
        Perform the download of our wanted angle repository.
        '''

        Log.PrintBegin("Checking repo present: ")

        # URL about repo where we do the CMake generation
        git_url_base = self.settings.options.git_url_base

        # Place where the cloned repo becomes stored
        sourcepath = self.settings.options.sourcepath

        try:
            # Do a clone of our wanted angle code if it is not already present
            if (not os.path.isdir(os.path.join(sourcepath, ".git")) or
                not os.path.isfile(os.path.join(sourcepath, ".gitignore"))):

                Log.PrintFollow("Repository not present and becomes cloned")
                Log.PrintResult(Result.SEE_BELOW)

                # Confirm correct Git URL is used
                validation = GitValidateRepositoryURL(git_url_base)
                if not validation['is_valid']:
                    Log.PrintFatal(f"GIT clone about \"{git_url_base}\" failed!")
                    Log.PrintFatal(f" - Protocol: {validation['protocol']}!")
                    Log.PrintFatal(f" - Error: {validation['errors']}!")
                    return False

                # Clone the repo where we do the changes
                git.Repo.clone_from(
                    url = git_url_base, 
                    to_path = sourcepath,
                    progress = GitRemoteProgress()
                )

                # Set cloned to true and prevent some update steps, as it is up to date
                self.cloned = True
            else:
                Log.PrintFollow("Repository already present")
                Log.PrintResult(Result.ALREADY_DONE)

            # Check the clone has succeeded by confirm the .git dir present
            # and one file in source folder.
            if (not os.path.isdir(os.path.join(sourcepath, ".git")) or
                not os.path.isfile(os.path.join(sourcepath, ".gitignore"))):

                Log.PrintFatal(f"Wanted repository not present (Needs: {sourcepath}/.git)!")
                return False

        except git.GitCommandError as ex:
            Log.PrintResult(Result.FAILURE)

            GITExceptionInfo(ex)
            return False

        except Exception as ex:
            Log.PrintResult(Result.FAILURE)

            ExceptionInfo(ex)
            return False

        return True


    def __download_upstream_angle_own__(self):
        '''
        Update/add in our repo the upstream to main repository about.

        It use from "self.settings.defaults.GIT_URL_BASE" the fixed value about the
        main target repository.

        In case the locally cloned repository based upon GIT_URL_BASE becomes
        this upstream not added as it is equal to the origin of the repo.
        '''

        # Reduce line lengths
        defaults = self.settings.defaults
        options = self.settings.options

        Log.PrintBegin(" - Check own modified Chromium Angle remote source present: ")

        try:
            #-------------------------------------------------------------------
            # Step 1:
            # Check our cloned repo is based upon origin or upn upstream, if origin
            # return and do nothing

            if not self.settings.isForkedRepository:
                Log.PrintFollow("Processed repository based about origin and not forked")
                Log.PrintResult(Result.IGNORED)
                return True

            #-------------------------------------------------------------------
            # Step 2:
            # Check the remote is already present, if not add it to our repo
            # Basically a "git remote add upstream URL" becomes called

            remote = git.remote.Remote(self.repo, "upstream")
            if not remote.exists():
                # Confirm correct Git URL is used
                validation = GitValidateRepositoryURL(options.git_url_remote_upstream)
                if not validation['is_valid']:
                    Log.PrintResult(Result.FAILURE)
                    Log.PrintFatal(f"GIT remote add about \"{options.git_url_remote_upstream}\" failed!")
                    Log.PrintFatal(f" - Protocol: {validation['protocol']}!")
                    Log.PrintFatal(f" - Error: {validation['errors']}!")
                    return False

                Log.PrintFollow("Remote branch becomes added")

                # Make the git calls, e.g. "git remote add upstream URL"
                remote.create(self.repo, "upstream", options.git_url_remote_upstream)

                self.force_works = True # Force on update below, as it was not present (deleted by user?)

                # Confirm our wanted remote exists now
                remote = git.remote.Remote(self.repo, "upstream")
                if not remote.exists():
                    Log.PrintFatal("Failed to get remote repository!")
                    return False

                Log.PrintResult(Result.OK)

            else:
                Log.PrintFollow("remote branch already set")
                Log.PrintResult(Result.ALREADY_DONE)

            #-------------------------------------------------------------------
            # Step 3:
            # Make a load of wanted upstream
            # Basically a "git fetch upstream" becomes called
            #
            # Note:
            # For here always the complete remote becomes loaded, as it is the
            # one where we work and mostly smaller as origin from Chromium.

            Log.PrintBegin(" - Update own Chromium Angle remote source: ")

            # Check the update time comes again, we do it normally only all
            # 24 hours to have not to much web traffic.
            if not options.force and \
               not self.force_works:
                try:
                    date_before = datetime.strptime(self.settings.lastremoteupdate.strip(), "%Y-%m-%d %H:%M:%S.%f")
                    date_now = datetime.now()
                except ValueError:
                    self.settings.SetAngleRemoteUpdate(datetime.now())
                    date_before = datetime.strptime(self.settings.lastremoteupdate.strip(), "%Y-%m-%d %H:%M:%S.%f")
                    date_now = datetime.now()
                finally:
                    difference = date_now - date_before
                    if difference.total_seconds() <= defaults.DEPENDS_ANGLE_FETCH_DATE_INTERVAL:  # True if more than 24 hours otherwise False
                        Log.PrintFollow(f"Last fetch check not pass the {defaults.DEPENDS_ANGLE_FETCH_DATE_INTERVAL/60/60} hours (Last: {date_before})")
                        Log.PrintResult(Result.IGNORED)
                        return True
            else:
                Log.PrintFollow("Fetch remote code" if self.force_works else "Fetch forced by command line value")
                Log.PrintResult(Result.SEE_BELOW)

            # Load data from upstream, e.g. "git fetch upstream"
            remote.fetch(progress = GitRemoteProgress(), 
                         verbose = options.debug)

            # Override the next ones and force work
            self.force_works = True

        except git.GitCommandError as ex:
            Log.PrintResult(Result.FAILURE)

            GITExceptionInfo(ex)
            return False

        except Exception as ex:
            Log.PrintResult(Result.FAILURE)

            ExceptionInfo(ex)
            return False

        return True


    def __download_upstream_angle_chromium__(self):
        '''
        Update/add in our repo the origin upstream based upon "chromium/angle"
        named "upstream-google" to main repository about.

        It use from "self.settings.defaults.GIT_URL_BASE" the fixed value about the
        main target repository.

        In case the locally cloned repository based upon GIT_URL_BASE becomes
        this upstream not added as it is equal to the origin of the repo.
        '''

        # Reduce line lengths
        defaults = self.settings.defaults
        options = self.settings.options

        try:
            Log.PrintBegin(" - Check original Chromium Angle remote source present: ")

            #-------------------------------------------------------------------
            # Step 1:
            # Check the remote is already present, if not add it to our repo
            # Basically a "git remote add upstream-google URL" becomes called

            remote = git.remote.Remote(self.repo, "upstream-google")
            if not remote.exists():
                validation = GitValidateRepositoryURL(defaults.GIT_URL_REMOTE_UPSTREAM_CHROMIUM)
                if not validation['is_valid']:
                    Log.PrintResult(Result.FAILURE)
                    Log.PrintFatal(f"GIT remote add about \"{defaults.GIT_URL_REMOTE_UPSTREAM_CHROMIUM}\" failed!")
                    Log.PrintFatal(f" - Protocol: {validation['protocol']}!")
                    Log.PrintFatal(f" - Error: {validation['errors']}!")
                    return False

                Log.PrintFollow("remote branch becomes added")

                # Make the git calls, e.g. "git remote add upstream-google" URL"
                remote.create(self.repo, "upstream-google", defaults.GIT_URL_REMOTE_UPSTREAM_CHROMIUM)

                self.force_works = True # Force on update below, as it was not present (deleted by user?)

                # Confirm our wanted remote exists now
                remote = git.remote.Remote(self.repo, "upstream-google")
                if not remote.exists():
                    Log.PrintFatal("Failed to get remote repository!")
                    return False

                Log.PrintResult(Result.OK)

            else:
                Log.PrintFollow("remote branch already set")
                Log.PrintResult(Result.ALREADY_DONE)

            #-------------------------------------------------------------------
            # Step 2:
            # Make a load of wanted upstream
            # Basically a "git fetch upstream" becomes called
            #
            # Note:
            # For here normally only the needed branch becomes taken, to make
            # complete takes very long and normally not needed for us.
            # In case the complete repo is needed call script with 
            # "code_generator__addon_depends_angle.py --full-remote"

            Log.PrintBegin(" - Update original Chromium Angle remote source: ")

            # Check the update time comes again, we do it normally only all
            # 24 hours to have not to much web traffic.
            if not options.force and \
               not self.settings.force_fetch_chromium and \
               not self.force_works:
                try:
                    date_before = datetime.strptime(self.settings.lastremoteupdate.strip(), "%Y-%m-%d %H:%M:%S.%f")
                    date_now = datetime.now()
                except ValueError:
                    self.settings.SetAngleRemoteUpdate(datetime.now())
                    date_before = datetime.strptime(self.settings.lastremoteupdate.strip(), "%Y-%m-%d %H:%M:%S.%f")
                    date_now = datetime.now()
                finally:
                    difference = date_now - date_before
                    if difference.total_seconds() <= defaults.DEPENDS_ANGLE_FETCH_DATE_INTERVAL:  # True if more than 24 hours otherwise False
                        Log.PrintFollow(f"Last fetch check not pass the {defaults.DEPENDS_ANGLE_FETCH_DATE_INTERVAL/60/60} hours (Last: {date_before})")
                        Log.PrintResult(Result.IGNORED)
                        return True
            else:
                Log.PrintFollow("Fetch remote code" if self.force_works else "Fetch forced by command line value")

            # Load data from upstream, e.g. "git fetch upstream-google"
            if not options.fullremote:
                # Load only reduced amount
                remote.fetch(refspec=f"chromium/{options.version}", 
                                progress=GitRemoteProgress(), 
                                verbose=options.debug, 
                                **{ "depth": "1" })
            else:
                # Load complete
                remote.fetch(progress=GitRemoteProgress(), 
                                verbose=options.debug)

            # Override the next ones and force work
            self.force_works = True

        except git.GitCommandError as ex:
            Log.PrintResult(Result.FAILURE)

            GITExceptionInfo(ex)
            return False

        except Exception as ex:
            Log.PrintResult(Result.FAILURE)

            ExceptionInfo(ex)
            return False

        return True


    def __update_cloned_own_angle__(self):
        '''
        Update the locally present and cloned repository.

        TODO: Still not usable and need implmentation about!
        '''

        # Reduce line lengths
        defaults = self.settings.defaults
        options = self.settings.options

        Log.PrintBegin(" - Checking our Angle repo needs update: ")

        # If was cloned before no need to do something
        if self.cloned:
            Log.PrintFollow("Repository cloned before, update not needed")
            Log.PrintResult(Result.ALREADY_DONE)
            return True

        try:
            # Check the update time comes again, we do it normally only all
            # 24 hours to have not to much web traffic.
            if not options.force and \
               not self.settings.force_fetch_chromium and \
               not self.force_works:
                try:
                    date_before = datetime.strptime(self.settings.lastremoteupdate.strip(), "%Y-%m-%d %H:%M:%S.%f")
                    date_now = datetime.now()
                except ValueError:
                    self.settings.SetAngleRemoteUpdate(datetime.now())
                    date_before = datetime.strptime(self.settings.lastremoteupdate.strip(), "%Y-%m-%d %H:%M:%S.%f")
                    date_now = datetime.now()
                finally:
                    difference = date_now - date_before
                    if difference.total_seconds() <= defaults.DEPENDS_ANGLE_FETCH_DATE_INTERVAL:  # True if more than 24 hours otherwise False
                        Log.PrintFollow(f"Last fetch check not pass the {defaults.DEPENDS_ANGLE_FETCH_DATE_INTERVAL/60/60} hours (Last: {date_before})")
                        Log.PrintResult(Result.IGNORED)
                        return True
            else:
                Log.PrintFollow("Fetch remote code" if self.force_works else "Fetch forced by command line value")
            
            Log.PrintFollow("Repository update still not supported (in TODO)")

            # TODO: Do works here and change Result.IGNORED to correct ones.

            # Override the next ones and force work
            self.force_works = True

            Log.PrintResult(Result.IGNORED)

        except git.GitCommandError as ex:
            Log.PrintResult(Result.FAILURE)

            GITExceptionInfo(ex)
            return False

        except Exception as ex:
            Log.PrintResult(Result.FAILURE)

            ExceptionInfo(ex)
            return False

        return True


    def __merge_google_to_our_main__(self):
        '''
        This function perform a merge of main branch on repo we use.

        Done to have equal to google/chromium. The main branch is mostly
        unused from us, only the commit history easier viewed in compore
        with it.

        @note The merge is mostly done outside of main branch, but both
        ways are possible.
        '''

        # Reduce line lengths
        options = self.settings.options
        defaults = self.settings.defaults

        Log.PrintBegin("Merge main to match \"upstream-google/main\"")

        try:
            # Check our repo about presence of chromium upstream/main branch
            found_upstream_origin = False
            for ref in self.repo.references:
                if ref.name == "upstream-google/main":
                    found_upstream_origin = True
                    break

            remote = git.remote.Remote(self.repo, "upstream-google")

            # If "main" remote not present fetch it
            if not found_upstream_origin:
                remote.fetch("main", progress=GitRemoteProgress(), verbose=options.debug)
            
            if not options.force and \
               not self.settings.force_fetch_chromium and \
               not self.force_works:
                try:
                    date_before = datetime.strptime(self.settings.lastremoteupdate.strip(), "%Y-%m-%d %H:%M:%S.%f")
                    date_now = datetime.now()
                except ValueError:
                    self.settings.SetAngleRemoteUpdate(datetime.now())
                    date_before = datetime.strptime(self.settings.lastremoteupdate.strip(), "%Y-%m-%d %H:%M:%S.%f")
                    date_now = datetime.now()
                finally:
                    difference = date_now - date_before
                    if difference.total_seconds() <= defaults.DEPENDS_ANGLE_FETCH_DATE_INTERVAL:  # True if more than 24 hours otherwise False
                        Log.PrintFollow(f"Last fetch check not pass the {defaults.DEPENDS_ANGLE_FETCH_DATE_INTERVAL/60/60} hours (Last: {date_before})")
                        Log.PrintResult(Result.IGNORED)
                        return True
            else:
                Log.PrintFollow("Fetch remote code" if self.force_works else "Fetch forced by command line value")

            # Load the remote with gitpython
            if not remote.exists():
                Log.PrintResult(Result.OWN_SET, "ignored as \"upstream-google\" not set for use")
                return True

            Log.PrintResult(Result.EMPTY)

            # Do something like git merge upstream-google
            # @note If crrent branch is not main a different ways becomes used
            if self.repo.active_branch.name != "main":
                remote.fetch(refspec = "main:main", 
                             progress = GitRemoteProgress(), 
                             verbose = options.debug)
            else:
                self.repo.git.merge("upstream-google/main")

            # Override the next ones and force work
            self.force_works = True

        except git.GitCommandError as ex:
            Log.PrintResult(Result.FAILURE)

            GITExceptionInfo(ex)
            return False

        except Exception as ex:
            Log.PrintResult(Result.FAILURE)

            ExceptionInfo(ex)
            return False

        return True


    def __checkout_needed_branch__(self):
        # Reduce line lengths
        defaults = self.settings.defaults
        options = self.settings.options

        if not options.version:
            Log.PrintFatal("Needed angle version not!")
            return False

        branch_active = str(self.repo.active_branch)
        branch_needed = f"{defaults.OWN_BASE_NAME}/{options.version}"
        if options.devbranch:
            branch_wanted = f"{defaults.OWN_BASE_NAME}/{options.version}-{options.devbranch}"
        else:
            branch_wanted = branch_needed

        Log.PrintBegin("Checkout or add our needed branch to use for CMake add: ")
        Log.PrintResult(Result.EMPTY)

        Log.PrintBegin(" Active branch: ")
        Log.PrintFollow(branch_active)
        Log.PrintResult(Result.EMPTY)

        Log.PrintBegin(" Needed branch: ")
        Log.PrintFollow(branch_wanted)
        Log.PrintResult(Result.EMPTY)

        found_needed = False
        found_upstream = False
        found_upstream_origin = False
        for ref in self.repo.references:
            if ref.name == branch_wanted:
                found_needed = True
            if ref.name == f"upstream/{branch_needed}":
                found_upstream = True
            if ref.name == f"upstream-google/chromium/{options.version}":
                found_upstream_origin = True

        Log.PrintBegin(" - Checkout branch: ")
        if branch_wanted == branch_active:
            Log.PrintFollow("Needed branch already set and active")
            Log.PrintResult(Result.ALREADY_DONE)
        else:
            if self.repo.is_dirty():
                Log.PrintFollow("Checkout not possible due to untracked files in current branch")
                Log.PrintResult(Result.FAILURE)
                Log.PrintWarning(f"""Generator stopped!
                      Checkout to \"{branch_wanted}\" not possiple. Code on local branch \"{branch_active}\" contains not commited changes.
                      Commit this changes or reset to have clean!""")
                return False
            if found_needed:
                Log.PrintFollow(f"Needed locally branch present, checking out from \"{branch_active}\" to \"{branch_wanted}\"")
                self.repo.git.checkout(branch_wanted)
                Log.PrintResult(Result.OK)
            elif found_upstream:
                Log.PrintFollow(f"Needed branch on own upstream present, checking out with creation of new from \"{branch_active}\" to \"{branch_wanted}\"")
                self.repo.git.checkout("-b", branch_wanted, "upstream/" + branch_needed)
                Log.PrintResult(Result.OK)
            elif found_upstream_origin:
                Log.PrintFollow(f"Needed branch on origin upstream present, checking out with creation of new from \"{branch_active}\" to \"{branch_wanted}\"")
                self.repo.git.checkout("-b", branch_wanted, "upstream-google/chromium/" + options.version)
                Log.PrintResult(Result.OK)
            else:
                Log.PrintResult(Result.FAILURE)
                return False

        return True

    def __get_angle_confirm_branch_name__(self):
        """
        Confirm branch name match angle version.
        Used in generated source code header files.
        """

        Log.PrintBegin(" - GIT branch version: ")

        version = re.sub('^[kodi|chromium]*/?([0-9]*).*', '\\1', self.repo.active_branch.name, flags = re.M)
        if version == self.settings.options.version:
            Log.PrintFollow(f"{version}")
            Log.PrintResult(Result.OK)
        else:
            Log.PrintFatal(f"Used branch version identified with \"{version}\", needed is \"{self.settings.options.version}\"")
            return False

        return True


    def __get_angle_last_commit_sha__(self):
        """
        Get last commit sha to use in generated source code header files.
        """

        Log.PrintBegin(" - GIT commit SHA: ")

        if self.settings.angleBaseGitCommitSHA == '':
            self.settings.angleBaseGitCommitSHA = self.repo.active_branch.commit
        if self.settings.angleBaseGitCommitSHA != '':
            Log.PrintFollow(f"{self.settings.angleBaseGitCommitSHA}")
            Log.PrintResult(Result.OK)
        else:
            Log.PrintFatal("Not possible to identify commit!")
            return False

        return True


    def __get_angle_last_commit_date__(self):
        """
        This used to get from Angle his last commit date where becomes
        used in source code header files to generate.
        """

        Log.PrintBegin(" - GIT commit date: ")
        if self.settings.angleBaseGitCommitDate == '':
            self.settings.angleBaseGitCommitDate = time.strftime("%Y%m%d-%H%M", time.gmtime(self.repo.active_branch.commit.committed_date))
        if self.settings.angleBaseGitCommitDate != '':
            Log.PrintFollow(self.settings.angleBaseGitCommitDate)
            Log.PrintResult(Result.OK)
        else:
            self.settings.angleBaseGitCommitDate = "unknown"
            Log.PrintFollow("Not possible to identify commit date, set to \"unknown\"")
            Log.PrintResult(Result.WARNING)

        return True

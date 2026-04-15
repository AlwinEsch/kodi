# -*- coding: utf-8 -*-

#   Copyright (C) 2026 Team Kodi
#   This file is part of Kodi - https://kodi.tv
#
#   SPDX-License-Identifier: GPL-2.0-or-later
#   See LICENSES/README.md for more information.

# Global includes
import sys, os, configparser, logging

# Own includes
from ..helper_Log import *

# Default values
class settings:
    isForkedRepository = False
    angleBaseGitCommitSHA = ""
    angleBaseGitCommitDate = ""
    used_gni_to_cmake_path = ""
    git_author = None
    git_committer = None

    def __init__(self, options, defaults) -> None:
        super().__init__()

        self.options = options
        self.defaults = defaults
        self.isForkedRepository = True#options.git_url_base != defaults.GIT_URL_REMOTE_UPSTREAM
        self.force_fetch_chromium = False
        self.lastremoteupdate = ""

    def __del__(self) -> None:
        pass

    def InitSettings(self, parser):
        config = configparser.ConfigParser()

        # Create the own dev-kit build directory if not already present, needed to
        # have configure settings available on further calls.
        #
        # Used here to have fixed place and available all the time
        if not os.path.exists(self.defaults.BUILD_DIRECTORY):
            os.makedirs(self.defaults.BUILD_DIRECTORY)

        # Set used setting file names
        self.options.settings_local = self.defaults.BUILD_DIRECTORY + '/settings-ADDON_DEPENDS_ANGLE.local.ini'
        self.options.settings_global = self.defaults.BUILD_DIRECTORY + '/settings-ADDON_DEPENDS_ANGLE.global.ini'
        self.options.settings_variable = self.defaults.BUILD_DIRECTORY + '/settings-ADDON_DEPENDS_ANGLE.variable.ini'

        # Set the pythongit to debug:
        if self.options.debug:
            os.environ["GIT_PYTHON_TRACE"] = "full" 
            # Use for gitpython only as INFO by debug --debug-output command line value,
            # force with e.g. GIT_PYTHON_DEBUG=1 ./code_generator__addon_depends_angle.py
            # To have here Debug output too.
            if os.getenv('GIT_PYTHON_DEBUG', "not found") == "not found":
                logging.basicConfig(level=logging.INFO)
            else:
                logging.basicConfig(level=logging.DEBUG)

        # Create local related settings
        update_settings = False

        if not os.path.exists(self.options.settings_local):
            self.ResetSettingsLocal()
        elif not config.read(self.options.settings_local):
            Log.PrintFatal("The load about local settings failed! File: \"{}\"".format(self.options.settings_local))
            return False
        else:
            self.options.settings_local_present = True

        if not 'DEFAULT' in config:
            config['DEFAULT'] = {}

        default = config['DEFAULT']
        if settings.is_opt_provided(parser, 'sourcepath') or not 'options.sourcepath' in default:
            default['options.sourcepath'] = self.options.sourcepath
            update_settings = True
        if settings.is_opt_provided(parser, 'buildpath') or not 'options.buildpath' in default:
            default['options.buildpath'] = self.options.buildpath
            update_settings = True
        if update_settings:
            with open(self.options.settings_local, 'w+') as configfile:
                config.write(configfile)

        self.options.sourcepath = default['options.sourcepath']
        self.options.buildpath = default['options.buildpath']

        config.clear()

        # Create public related settings, where relates to upload, e.g. the version
        update_settings = False

        if not os.path.exists(self.options.settings_global):
            self.ResetSettingsGlobal()
        elif not config.read(self.options.settings_global):
            Log.PrintFatal("The load about public settings failed! File: \"{}\"".format(self.options.settings_global))
            return False
        else:
            self.options.settings_global_present = True

        if not 'DEFAULT' in config:
            config['DEFAULT'] = {}

        default = config['DEFAULT']
        if settings.is_opt_provided(parser, 'version') or not 'options.version' in default:
            default['options.version'] = self.options.version
            update_settings = True
        if settings.is_opt_provided(parser, 'git_url_base') or not 'options.git_url_base' in default:
            default['options.git_url_base'] = self.options.git_url_base
            update_settings = True
        if settings.is_opt_provided(parser, 'git_url_remote_upstream') or not 'options.git_url_remote_upstream' in default:
            default['options.git_url_remote_upstream'] = self.options.git_url_remote_upstream
            update_settings = True
        if settings.is_opt_provided(parser, 'devbranch') or not 'options.devbranch' in default:
            default['options.devbranch'] = self.options.devbranch
            update_settings = True
        if update_settings:
            with open(self.options.settings_local, 'w+') as configfile:
                config.write(configfile)

        self.options.version = default['options.version']
        self.options.git_url_base = default['options.git_url_base']
        self.options.git_url_remote_upstream = default['options.git_url_remote_upstream']
        self.options.devbranch = default['options.devbranch']

        config.clear()

        # Create variable settings, used for changing values by every call, e.g. timestamps
        if not os.path.exists(self.options.settings_variable):
            self.ResetSettingsVariable()
        elif not config.read(self.options.settings_variable):
            Log.PrintFatal("The load about variable settings failed! File: \"{}\"".format(self.options.settings_variable))
            return False
        else:
            self.options.settings_variable_present = True

        if not 'DEFAULT' in config:
            config['DEFAULT'] = {}

        default = config['DEFAULT']

        # Set settings if was not present before
        if not 'options.last-angle-remote-update' in default:
            default['options.last-angle-remote-update'] = str(datetime.now())
            update_settings = True
        if not 'options.wasfullremotebefore' in default:
            default['options.wasfullremotebefore'] = str(False)
            update_settings = True

        self.force_fetch_chromium = True if self.options.fullremote and default['options.wasfullremotebefore'] != "True" else False
        self.lastremoteupdate = default['options.last-angle-remote-update']

        return True

    def ResetSettingsLocal(self):
        config = configparser.ConfigParser()

        default = config['DEFAULT']
        default['options.sourcepath'] = self.options.sourcepath
        default['options.buildpath'] = self.options.buildpath
        default['options.fullremote'] = str(self.options.fullremote)
        default['options.devbranch'] = str(self.options.devbranch)

        default['options.author_name'] = self.options.buildpath

        with open(self.options.settings_local, 'w+') as configfile:
            config.write(configfile)

        return True

    def ResetSettingsGlobal(self):
        config = configparser.ConfigParser()

        default = config['DEFAULT']
        default['options.version'] = self.options.version
        print(self.options.git_url_base)
        default['options.git_url_base'] = self.options.git_url_base
        default['options.git_url_remote_upstream'] = self.options.git_url_remote_upstream

        with open(self.options.settings_global, 'w+') as configfile:
            config.write(configfile)

        return True

    def ResetSettingsVariable(self):
        config = configparser.ConfigParser()

        default = config['DEFAULT']
        default['options.last-angle-remote-update'] = str(datetime.now())
        default['options.wasfullremotebefore'] = str(False)

        with open(self.options.settings_variable, 'w+') as configfile:
            config.write(configfile)

        return True

    def SetAngleRemoteUpdate(self, time):
        config = configparser.ConfigParser()
        if config.read(self.options.settings_variable):
            if not 'DEFAULT' in config:
                config['DEFAULT'] = {}

            default = config['DEFAULT']
            default['options.last-angle-remote-update'] = str(time)
            default['options.wasfullremotebefore'] = str(self.options.fullremote)
            self.lastremoteupdate = str(time)

            with open(self.options.settings_variable, 'w+') as configfile:
                config.write(configfile)

    def strtobool (val):
        """Convert a string representation of truth to true (1) or false (0).
        True values are 'y', 'yes', 't', 'true', 'on', and '1'; false values
        are 'n', 'no', 'f', 'false', 'off', and '0'.  Raises ValueError if
        'val' is anything else.
        """
        val = val.lower()
        if val in ('y', 'yes', 't', 'true', 'on', '1'):
            return 1
        elif val in ('n', 'no', 'f', 'false', 'off', '0'):
            return 0
        else:
            raise ValueError("invalid truth value %r" % (val,))

    def is_opt_provided(parser, dest):
        if any (opt.dest == dest and ((opt._long_opts and opt._long_opts[0] in sys.argv[1:]) or (opt._short_opts and opt._short_opts[0] in sys.argv[1:])) for opt in parser._get_all_options()):
            return True
        return False   
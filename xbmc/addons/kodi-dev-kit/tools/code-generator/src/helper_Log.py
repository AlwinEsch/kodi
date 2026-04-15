# -*- coding: utf-8 -*-

#   Copyright (C) 2021 Team Kodi
#   This file is part of Kodi - https://kodi.tv
#
#   SPDX-License-Identifier: GPL-2.0-or-later
#   See LICENSES/README.md for more information.

from datetime import datetime
import re, os

class Result:
    OK = 1
    FAILURE = 2
    WARNING = 3
    UPDATE = 4
    ALREADY_DONE = 5
    NEW = 6
    SEE_BELOW = 7
    IGNORED = 8
    EMPTY = 9
    OWN_SET = 10


class Log:
    log_file = "creation_log.txt"
    current_cursor_pos = 0
    terminal_columns = 120
    padding_spaces = 22

    # Class of different styles
    class style:
        BOLD = "\033[01m"
        BOLDOFF = "\033[22m"
        ITALICS = "\033[3m"
        ITALICSOFF = "\033[23m"
        UNDERLINE = "\033[4m"
        UNDERLINEOFF = "\033[24m"

        BLACK = "\033[30m"
        RED = "\033[31m"
        GREEN = "\033[32m"
        YELLOW = "\033[33m"
        BLUE = "\033[34m"
        MAGENTA = "\033[35m"
        CYAN = "\033[36m"
        AMBER = "\033[38;2;255;191;0m"
        WHITE = "\033[37m"
        RESET = "\033[0m"

    def Init(options):
        # Try to get terminal with, is optional and no matter if something fails
        try:
            columns, rows = os.get_terminal_size(0)
            Log.terminal_columns = columns
        except:
            pass

        if os.path.isfile(Log.log_file):
            if os.path.isfile(Log.log_file + ".old"):
                os.remove(Log.log_file + ".old")
            os.rename(Log.log_file, Log.log_file + ".old")

        if options.debug:
            print("DEBUG: Used command line options: {}".format(str(options)))

        with open(Log.log_file, "w") as f:
            f.write("Used call options: {}\n".format(str(options)))

    def PrintMainStartBegin(text, second_line = ""):
        print("┌{}┐".format("─" * (Log.terminal_columns - 2)))
        print(
            "│ {}{}{}{}{}│".format(
                Log.style.BOLD,
                Log.style.CYAN,
                text,
                Log.style.RESET,
                " " * (Log.terminal_columns - len(text) - 3),
            )
        )
        text = second_line
        print(
            "│ {}{}{}{}{}{}│".format(
                Log.style.BOLD,
                Log.style.WHITE,
                Log.style.UNDERLINE,
                text,
                Log.style.RESET,
                " " * (Log.terminal_columns - len(text) - 3),
            )
        )

    def PrintMainStartEnd():
        print("└{}┘".format("─" * (Log.terminal_columns - 2)))

    def PrintGroupStart(text):
        print("─" * Log.terminal_columns)
        print(
            "{}{} ...{}{}".format(
                Log.style.CYAN,
                text,
                " " * (Log.terminal_columns - len(text) - 4),
                Log.style.RESET,
            )
        )
        with open(Log.log_file, "a") as f:
            f.write("{}...\n".format(text))

    def PrintBegin(text):
        # datetime object containing current date and time
        dt_string = datetime.utcnow().strftime("%d/%m/%Y %H:%M:%S")
        Log.current_cursor_pos = len(text) + len(dt_string) + 3

        print(
            "[{}{}{}] {}{}{}{}".format(
                Log.style.MAGENTA,
                dt_string,
                Log.style.RESET,
                Log.style.WHITE,
                Log.style.BOLD,
                text,
                Log.style.RESET,
            ),
            end="",
        )
        with open(Log.log_file, "a") as f:
            f.write("[{}] {}: ".format(dt_string, text))

    def PrintFollow(text):
        Log.current_cursor_pos += len(text)

        print(Log.style.CYAN + text + Log.style.RESET, end="")
        with open(Log.log_file, "a") as f:
            f.write("{} ".format(text))

    def PrintResult(result_type, result_text=None):
        text = ""
        color = Log.style.WHITE

        if result_type == Result.OK:
            text = "OK"
            color = Log.style.GREEN
        elif result_type == Result.NEW:
            text = "Created new"
            color = Log.style.CYAN
        elif result_type == Result.FAILURE:
            text = "Failed"
            color = Log.style.RED
        elif result_type == Result.WARNING:
            text = "Warning"
            color = Log.style.MAGENTA
        elif result_type == Result.UPDATE:
            text = "Updated"
            color = Log.style.YELLOW
        elif result_type == Result.ALREADY_DONE:
            text = "Already done and up to date"
            color = Log.style.GREEN
        elif result_type == Result.SEE_BELOW:
            text = "See below"
            color = Log.style.BLUE
        elif result_type == Result.IGNORED:
            text = "Ignored"
            color = Log.style.YELLOW
        elif result_type == Result.EMPTY:
            text = ""
            color = Log.style.YELLOW
        elif result_type == Result.OWN_SET:
            text = result_text
            color = Log.style.GREEN
            # Set to None to prevent print in call below
            result_text = None

        print(
            "{}{}{}{}".format(
                color,
                Log.style.BOLD,
                text.rjust(Log.terminal_columns - Log.current_cursor_pos),
                Log.style.RESET,
            )
        )
        f = open(Log.log_file, "a")
        f.write("{}\n".format(text))
        if result_text:
            print("Results of call before:{}\n".format(result_text))
            f.write("Results of call before:{}\n".format(result_text))
        f.close()

    def PrintWarning(error_text):
        # datetime object containing current date and time
        dt_string = datetime.utcnow().strftime("%d/%m/%Y %H:%M:%S")
        Log.current_cursor_pos = len(error_text) + len(dt_string) + 3

        print(
            "[{}{}{}] {}{}WARNING: {}{}".format(
                Log.style.YELLOW,
                dt_string,
                Log.style.RESET,
                Log.style.MAGENTA,
                Log.style.BOLD,
                Log.style.RESET,
                error_text,
            )
        )
        with open(Log.log_file, "a") as f:
            f.write("[{}] {}\n".format(dt_string, error_text))

    def PrintFatal(error_text):
        # datetime object containing current date and time
        dt_string = datetime.utcnow().strftime("%d/%m/%Y %H:%M:%S")
        Log.current_cursor_pos = len(error_text) + len(dt_string) + 3

        error_list = []
        if '\n' in error_text:
            error_list = error_text.split("\n")

        print(
            "[{}{}{}] {}{}FATAL: {}{}".format(
                Log.style.YELLOW,
                dt_string,
                Log.style.RESET,
                Log.style.RED,
                Log.style.BOLD,
                Log.style.RESET,
                error_text if not error_list else error_list[0],
            )
        )

        if error_list and len(error_list) > 1:
            for text in error_list[1:]:
                print(
                    "[{}{}{}] {}{}       {}{}".format(
                        Log.style.YELLOW,
                        dt_string,
                        Log.style.RESET,
                        Log.style.RED,
                        Log.style.BOLD,
                        Log.style.RESET,
                        text,
                    )
                )
        with open(Log.log_file, "a") as f:
            f.write("[{}] {}\n".format(dt_string, error_text))

    def PrintUsedBooleanValueLine(name, description, value, default = False):

        if default == value:
            default_string = Log.style.ITALICS + Log.style.BLUE + " (Default)"
        else:
            default_string = ""

        text = "{:16s} {:30s} {}{}{}".format(
            name,
            description,
            Log.style.GREEN + ("yes" if value else "no"),
            default_string,
            Log.style.RESET,
        )
        print(
            "│ {}{}{}{}{}│".format(
                Log.style.BOLD,
                Log.style.WHITE,
                text,
                Log.style.RESET,
                " " * (Log.terminal_columns - Log.line_width(text) - 3),
            )
        )

    def PrintUsedStringValueLine(name, description, value, default = "", required = False):
        notice_string = ""

        if required:
            if not value:
                if default and value != default:
                    notice_string += Log.style.AMBER
                    notice_string += " <- Warning:" + Log.style.ITALICS + " Using fallback (value \"{}\" not set)!".format(name)
                    value = default
                else:
                    notice_string += Log.style.RED
                    notice_string += "Error:" + Log.style.ITALICS + " Value \"{}\" not Set!".format(name)
        else:
            notice_string += Log.style.ITALICS

            if not value:
                notice_string += Log.style.CYAN + "Unused"
                value = default

            if (default and len(value) == 0) or default == value:
                notice_string += Log.style.BLUE + " (Default)"

        text = "{:16s} {:30s} {}{}".format(
            name,
            description,
            value,
            notice_string,
            Log.style.RESET,
        )
        print(
            "│ {}{}{}{}{}│".format(
                Log.style.BOLD,
                Log.style.WHITE,
                text,
                Log.style.RESET,
                " " * (Log.terminal_columns - Log.line_width(text) - 3),
            )
        )

    def PrintTextLine(text):
        print(
            "│ {}{}{}{}{}│".format(
                Log.style.BOLD,
                Log.style.WHITE,
                text,
                Log.style.RESET,
                " " * (Log.terminal_columns - Log.line_width(text) - 3),
            )
        )

    def line_width(text):
        return len(re.compile(r'\x1b[^m]*m').sub('', text))
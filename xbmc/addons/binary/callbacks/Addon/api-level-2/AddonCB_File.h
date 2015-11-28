#pragma once
/*
 *      Copyright (C) 2015 Team KODI
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with KODI; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "PlatformDefs.h" // for __stat64, ssize_t
#include "addon.api2/AddonLib.h"
#include "filesystem/File.h"

extern "C"
{
namespace AddOnLIB
{
namespace V2
{

class CAddonCB_File
{
public:
  CAddonCB_File();

  void Init(CB_AddOnLib_File *callbacks);

  static void* open_file(
        void*                     hdl,
        const char*               strFileName,
        unsigned int              flags);

  static void* open_file_for_write(
        void*                     hdl,
        const char*               strFileName,
        bool                      bOverwrite);

  static ssize_t read_file(
        void*                     hdl,
        void*                     file,
        void*                     lpBuf,
        size_t                    uiBufSize);

  static bool read_file_string(
        void*                     hdl,
        void*                     file,
        char*                     szLine,
        int                       iLineLength);

  static ssize_t write_file(
        void*                     hdl,
        void*                     file,
        const void*               lpBuf,
        size_t                    uiBufSize);

  static void flush_file(
        void*                     hdl,
        void*                     file);

  static int64_t seek_file(
        void*                     hdl,
        void*                     file,
        int64_t                   iFilePosition,
        int                       iWhence);

  static int truncate_file(
        void*                     hdl,
        void*                     file,
        int64_t                   iSize);

  static int64_t get_file_position(
        void*                     hdl,
        void*                     file);

  static int64_t get_file_length(
        void*                     hdl,
        void*                     file);

  static void close_file(
        void*                     hdl,
        void*                     file);

  static int get_file_chunk_size(
        void*                     hdl,
        void*                     file);

  static bool file_exists(
        void*                     hdl,
        const char*               strFileName,
        bool                      bUseCache);

  static int stat_file(
        void*                     hdl,
        const char*               strFileName,
        struct __stat64*        buffer);

  static bool delete_file(
        void*                     hdl,
        const char*               strFileName);
};

}; /* namespace V2 */
}; /* namespace AddOnLIB */
}; /* extern "C" */

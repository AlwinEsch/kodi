/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "Filesystem.h"

#include "Util.h"
#include "addons/binary-addons/AddonDll.h"
#include "addons/kodi-addon-dev-kit/include/kodi/Filesystem.h"
#include "filesystem/Directory.h"
#include "filesystem/File.h"
#include "filesystem/SpecialProtocol.h"
#include "utils/Crc32.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/log.h"

#include <vector>

using namespace kodi; // addon-dev-kit namespace
using namespace XFILE;

extern "C"
{
namespace ADDON
{

void Interface_Filesystem::Init(AddonGlobalInterface* addonInterface)
{
  addonInterface->toKodi->kodi_filesystem = new AddonToKodiFuncTable_kodi_filesystem();

  addonInterface->toKodi->kodi_filesystem->can_open_directory = can_open_directory;
  addonInterface->toKodi->kodi_filesystem->create_directory = create_directory;
  addonInterface->toKodi->kodi_filesystem->directory_exists = directory_exists;
  addonInterface->toKodi->kodi_filesystem->remove_directory = remove_directory;
  addonInterface->toKodi->kodi_filesystem->get_directory = get_directory;
  addonInterface->toKodi->kodi_filesystem->free_directory = free_directory;

  addonInterface->toKodi->kodi_filesystem->file_exists = file_exists;
  addonInterface->toKodi->kodi_filesystem->stat_file = stat_file;
  addonInterface->toKodi->kodi_filesystem->delete_file = delete_file;
  addonInterface->toKodi->kodi_filesystem->rename_file = rename_file;
  addonInterface->toKodi->kodi_filesystem->copy_file = copy_file;
  addonInterface->toKodi->kodi_filesystem->get_file_md5 = get_file_md5;
  addonInterface->toKodi->kodi_filesystem->get_cache_thumb_name = get_cache_thumb_name;
  addonInterface->toKodi->kodi_filesystem->make_legal_filename = make_legal_filename;
  addonInterface->toKodi->kodi_filesystem->make_legal_path = make_legal_path;
  addonInterface->toKodi->kodi_filesystem->translate_special_protocol = translate_special_protocol;
  addonInterface->toKodi->kodi_filesystem->is_internet_stream = is_internet_stream;
  addonInterface->toKodi->kodi_filesystem->is_on_lan = is_on_lan;
  addonInterface->toKodi->kodi_filesystem->is_remote = is_remote;
  addonInterface->toKodi->kodi_filesystem->is_local = is_local;
  addonInterface->toKodi->kodi_filesystem->is_url = is_url;

  addonInterface->toKodi->kodi_filesystem->open_file = open_file;
  addonInterface->toKodi->kodi_filesystem->open_file_for_write = open_file_for_write;
  addonInterface->toKodi->kodi_filesystem->read_file = read_file;
  addonInterface->toKodi->kodi_filesystem->read_file_string = read_file_string;
  addonInterface->toKodi->kodi_filesystem->write_file = write_file;
  addonInterface->toKodi->kodi_filesystem->flush_file = flush_file;
  addonInterface->toKodi->kodi_filesystem->seek_file = seek_file;
  addonInterface->toKodi->kodi_filesystem->truncate_file = truncate_file;
  addonInterface->toKodi->kodi_filesystem->get_file_position = get_file_position;
  addonInterface->toKodi->kodi_filesystem->get_file_length = get_file_length;
  addonInterface->toKodi->kodi_filesystem->get_file_download_speed = get_file_download_speed;
  addonInterface->toKodi->kodi_filesystem->close_file = close_file;
  addonInterface->toKodi->kodi_filesystem->get_file_chunk_size = get_file_chunk_size;
  addonInterface->toKodi->kodi_filesystem->io_control_get_seek_possible =
      io_control_get_seek_possible;
  addonInterface->toKodi->kodi_filesystem->io_control_get_cache_status =
      io_control_get_cache_status;
  addonInterface->toKodi->kodi_filesystem->io_control_set_cache_rate = io_control_set_cache_rate;
  addonInterface->toKodi->kodi_filesystem->io_control_set_retry = io_control_set_retry;
  addonInterface->toKodi->kodi_filesystem->get_property_values = get_property_values;

  addonInterface->toKodi->kodi_filesystem->curl_create = curl_create;
  addonInterface->toKodi->kodi_filesystem->curl_add_option = curl_add_option;
  addonInterface->toKodi->kodi_filesystem->curl_open = curl_open;
}

void Interface_Filesystem::DeInit(AddonGlobalInterface* addonInterface)
{
  if (addonInterface->toKodi) /* <-- Safe check, needed so long old addon way is present */
  {
    delete addonInterface->toKodi->kodi_filesystem;
    addonInterface->toKodi->kodi_filesystem = nullptr;
  }
}

bool Interface_Filesystem::can_open_directory(void* kodiBase, const char* url)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || url == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', url='{}')",
              __FUNCTION__, kodiBase, static_cast<const void*>(url));
    return false;
  }

  CFileItemList items;
  return CDirectory::GetDirectory(url, items, "", DIR_FLAG_DEFAULTS);
}

bool Interface_Filesystem::create_directory(void* kodiBase, const char* path)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || path == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', path='{}')",
              __FUNCTION__, kodiBase, static_cast<const void*>(path));
    return false;
  }

  return CDirectory::Create(path);
}

bool Interface_Filesystem::directory_exists(void* kodiBase, const char* path)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || path == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', path='{}')",
              __FUNCTION__, kodiBase, static_cast<const void*>(path));
    return false;
  }

  return CDirectory::Exists(path);
}

bool Interface_Filesystem::remove_directory(void* kodiBase, const char* path)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || path == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', path='{}')",
              __FUNCTION__, kodiBase, static_cast<const void*>(path));
    return false;
  }

  // Empty directory
  CFileItemList fileItems;
  CDirectory::GetDirectory(path, fileItems, "", DIR_FLAG_DEFAULTS);
  for (int i = 0; i < fileItems.Size(); ++i)
    CFile::Delete(fileItems.Get(i)->GetPath());

  return CDirectory::Remove(path);
}

static void CFileItemListToVFSDirEntries(VFSDirEntry* entries, const CFileItemList& items)
{
  for (unsigned int i = 0; i < static_cast<unsigned int>(items.Size()); ++i)
  {
    entries[i].label = strdup(items[i]->GetLabel().c_str());
    entries[i].path = strdup(items[i]->GetPath().c_str());
    entries[i].size = items[i]->m_dwSize;
    entries[i].folder = items[i]->m_bIsFolder;
    items[i]->m_dateTime.GetAsTime(entries[i].date_time);
  }
}

bool Interface_Filesystem::get_directory(void* kodiBase,
                                         const char* path,
                                         const char* mask,
                                         struct VFSDirEntry** items,
                                         unsigned int* num_items)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || path == nullptr || mask == nullptr || items == nullptr ||
      num_items == nullptr)
  {
    CLog::Log(LOGERROR,
              "Interface_Filesystem::{} - invalid data (addon='{}', path='{}', mask='{}', "
              "items='{}', num_items='{}'",
              __FUNCTION__, kodiBase, static_cast<const void*>(path),
              static_cast<const void*>(mask), static_cast<void*>(items),
              static_cast<void*>(num_items));
    return false;
  }

  CFileItemList fileItems;
  if (!CDirectory::GetDirectory(path, fileItems, mask, DIR_FLAG_NO_FILE_DIRS))
    return false;

  if (fileItems.Size() > 0)
  {
    *num_items = static_cast<unsigned int>(fileItems.Size());
    *items = new VFSDirEntry[fileItems.Size()];
    CFileItemListToVFSDirEntries(*items, fileItems);
  }
  else
  {
    *num_items = 0;
    *items = nullptr;
  }

  return true;
}

void Interface_Filesystem::free_directory(void* kodiBase,
                                          struct VFSDirEntry* items,
                                          unsigned int num_items)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || items == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', items='{}')",
              __FUNCTION__, kodiBase, static_cast<void*>(items));
    return;
  }

  for (unsigned int i = 0; i < num_items; ++i)
  {
    free(items[i].label);
    free(items[i].path);
  }
  delete[] items;
}

//------------------------------------------------------------------------------

bool Interface_Filesystem::file_exists(void* kodiBase, const char* filename, bool useCache)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || filename == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', filename='{}')",
              __FUNCTION__, kodiBase, static_cast<const void*>(filename));
    return false;
  }

  return CFile::Exists(filename, useCache);
}

bool Interface_Filesystem::stat_file(void* kodiBase,
                                     const char* filename,
                                     struct STAT_STRUCTURE* buffer)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || filename == nullptr || buffer == nullptr)
  {
    CLog::Log(LOGERROR,
              "Interface_Filesystem::{} - invalid data (addon='{}', filename='{}', buffer='{}')",
              __FUNCTION__, kodiBase, static_cast<const void*>(filename),
              static_cast<void*>(buffer));
    return false;
  }

  struct __stat64 statBuffer;
  if (CFile::Stat(filename, &statBuffer) != 0)
    return false;

  buffer->deviceId = statBuffer.st_dev;
  buffer->size = statBuffer.st_size;
  buffer->accessTime = statBuffer.st_atime;
  buffer->modificationTime = statBuffer.st_mtime;
  buffer->statusTime = statBuffer.st_ctime;
  buffer->isDirectory = S_ISDIR(statBuffer.st_mode);
  buffer->isSymLink = S_ISLNK(statBuffer.st_mode);

  return true;
}

bool Interface_Filesystem::delete_file(void* kodiBase, const char* filename)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || filename == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', filename='{}')",
              __FUNCTION__, kodiBase, static_cast<const void*>(filename));
    return false;
  }

  return CFile::Delete(filename);
}

bool Interface_Filesystem::rename_file(void* kodiBase,
                                       const char* filename,
                                       const char* newFileName)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || filename == nullptr || newFileName == nullptr)
  {
    CLog::Log(
        LOGERROR,
        "Interface_Filesystem::{} - invalid data (addon='{}', filename='{}', newFileName='{}')",
        __FUNCTION__, kodiBase, static_cast<const void*>(filename),
        static_cast<const void*>(newFileName));
    return false;
  }

  return CFile::Rename(filename, newFileName);
}

bool Interface_Filesystem::copy_file(void* kodiBase, const char* filename, const char* dest)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || filename == nullptr || dest == nullptr)
  {
    CLog::Log(
        LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', filename='{}', dest='{}')",
        __FUNCTION__, kodiBase, static_cast<const void*>(filename), static_cast<const void*>(dest));
    return false;
  }

  return CFile::Copy(filename, dest);
}

char* Interface_Filesystem::get_file_md5(void* kodiBase, const char* filename)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || filename == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', filename='{})",
              __FUNCTION__, kodiBase, static_cast<const void*>(filename));
    return nullptr;
  }

  std::string string = CUtil::GetFileDigest(filename, KODI::UTILITY::CDigest::Type::MD5);
  char* buffer = strdup(string.c_str());
  return buffer;
}

char* Interface_Filesystem::get_cache_thumb_name(void* kodiBase, const char* filename)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || filename == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', filename='{})",
              __FUNCTION__, kodiBase, static_cast<const void*>(filename));
    return nullptr;
  }

  Crc32 crc;
  crc.ComputeFromLowerCase(filename);
  std::string string = StringUtils::Format("%08x.tbn", static_cast<unsigned int>(crc));
  char* buffer = strdup(string.c_str());
  return buffer;
}

char* Interface_Filesystem::make_legal_filename(void* kodiBase, const char* filename)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || filename == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', filename='{})",
              __FUNCTION__, kodiBase, static_cast<const void*>(filename));
    return nullptr;
  }

  std::string string = CUtil::MakeLegalFileName(filename);
  char* buffer = strdup(string.c_str());
  return buffer;
}

char* Interface_Filesystem::make_legal_path(void* kodiBase, const char* path)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || path == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', path='{})",
              __FUNCTION__, kodiBase, static_cast<const void*>(path));
    return nullptr;
  }

  std::string string = CUtil::MakeLegalPath(path);
  char* buffer = strdup(string.c_str());
  return buffer;
}

char* Interface_Filesystem::translate_special_protocol(void* kodiBase, const char* strSource)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || strSource == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', strSource='{})",
              __FUNCTION__, kodiBase, static_cast<const void*>(strSource));
    return nullptr;
  }

  return strdup(CSpecialProtocol::TranslatePath(strSource).c_str());
}

bool Interface_Filesystem::is_internet_stream(void* kodiBase, const char* path, bool strictCheck)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || path == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', path='{})",
              __FUNCTION__, kodiBase, static_cast<const void*>(path));
    return false;
  }

  return URIUtils::IsInternetStream(path, strictCheck);
}

bool Interface_Filesystem::is_on_lan(void* kodiBase, const char* path)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || path == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', path='{})",
              __FUNCTION__, kodiBase, static_cast<const void*>(path));
    return false;
  }

  return URIUtils::IsOnLAN(path);
}

bool Interface_Filesystem::is_remote(void* kodiBase, const char* path)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || path == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', path='{})",
              __FUNCTION__, kodiBase, static_cast<const void*>(path));
    return false;
  }

  return URIUtils::IsRemote(path);
}

bool Interface_Filesystem::is_local(void* kodiBase, const char* path)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || path == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', path='{})",
              __FUNCTION__, kodiBase, static_cast<const void*>(path));
    return false;
  }

  return CURL(path).IsLocal();
}

bool Interface_Filesystem::is_url(void* kodiBase, const char* path)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || path == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', path='{})",
              __FUNCTION__, kodiBase, static_cast<const void*>(path));
    return false;
  }

  return URIUtils::IsURL(path);
}

//------------------------------------------------------------------------------

void* Interface_Filesystem::open_file(void* kodiBase, const char* filename, unsigned int flags)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || filename == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', filename='{}')",
              __FUNCTION__, kodiBase, static_cast<const void*>(filename));
    return nullptr;
  }

  CFile* file = new CFile;
  if (file->Open(filename, flags))
    return static_cast<void*>(file);

  delete file;
  return nullptr;
}

void* Interface_Filesystem::open_file_for_write(void* kodiBase,
                                                const char* filename,
                                                bool overwrite)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || filename == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', filename='{}')",
              __FUNCTION__, kodiBase, static_cast<const void*>(filename));
    return nullptr;
  }

  CFile* file = new CFile;
  if (file->OpenForWrite(filename, overwrite))
    return static_cast<void*>(file);

  delete file;
  return nullptr;
}

ssize_t Interface_Filesystem::read_file(void* kodiBase, void* file, void* ptr, size_t size)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr || ptr == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', file='{}', ptr='{}')",
              __FUNCTION__, kodiBase, file, ptr);
    return -1;
  }

  return static_cast<CFile*>(file)->Read(ptr, size);
}

bool Interface_Filesystem::read_file_string(void* kodiBase,
                                            void* file,
                                            char* szLine,
                                            int lineLength)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr || szLine == nullptr)
  {
    CLog::Log(LOGERROR,
              "Interface_Filesystem::{} - invalid data (addon='{}', file='{}', szLine=='{}')",
              __FUNCTION__, kodiBase, file, static_cast<void*>(szLine));
    return false;
  }

  return static_cast<CFile*>(file)->ReadString(szLine, lineLength);
}

ssize_t Interface_Filesystem::write_file(void* kodiBase, void* file, const void* ptr, size_t size)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr || ptr == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', file='{}', ptr='{}')",
              __FUNCTION__, kodiBase, file, ptr);
    return -1;
  }

  return static_cast<CFile*>(file)->Write(ptr, size);
}

void Interface_Filesystem::flush_file(void* kodiBase, void* file)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', file='{}')",
              __FUNCTION__, kodiBase, file);
    return;
  }

  static_cast<CFile*>(file)->Flush();
}

int64_t Interface_Filesystem::seek_file(void* kodiBase, void* file, int64_t position, int whence)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', file='{}')",
              __FUNCTION__, kodiBase, file);
    return -1;
  }

  return static_cast<CFile*>(file)->Seek(position, whence);
}

int Interface_Filesystem::truncate_file(void* kodiBase, void* file, int64_t size)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', file='{}')",
              __FUNCTION__, kodiBase, file);
    return -1;
  }

  return static_cast<CFile*>(file)->Truncate(size);
}

int64_t Interface_Filesystem::get_file_position(void* kodiBase, void* file)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', file='{}')",
              __FUNCTION__, kodiBase, file);
    return -1;
  }

  return static_cast<CFile*>(file)->GetPosition();
}

int64_t Interface_Filesystem::get_file_length(void* kodiBase, void* file)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', file='{}')",
              __FUNCTION__, kodiBase, file);
    return -1;
  }

  return static_cast<CFile*>(file)->GetLength();
}

double Interface_Filesystem::get_file_download_speed(void* kodiBase, void* file)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', file='{}')",
              __FUNCTION__, kodiBase, file);
    return 0.0f;
  }

  return static_cast<CFile*>(file)->GetDownloadSpeed();
}

void Interface_Filesystem::close_file(void* kodiBase, void* file)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', file='{}')",
              __FUNCTION__, kodiBase, file);
    return;
  }

  static_cast<CFile*>(file)->Close();
  delete static_cast<CFile*>(file);
}

int Interface_Filesystem::get_file_chunk_size(void* kodiBase, void* file)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::{} - invalid data (addon='{}', file='{}')", __FUNCTION__,
              kodiBase, file);
    return -1;
  }

  return static_cast<CFile*>(file)->GetChunkSize();
}

bool Interface_Filesystem::io_control_get_seek_possible(void* kodiBase, void* file)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::{} - invalid data (addon='{}', file='{}')", __FUNCTION__,
              kodiBase, file);
    return -1;
  }

  return static_cast<CFile*>(file)->IoControl(EIoControl::IOCTRL_SEEK_POSSIBLE, nullptr) != 0
             ? true
             : false;
}

bool Interface_Filesystem::io_control_get_cache_status(void* kodiBase,
                                                       void* file,
                                                       struct VFS_CACHE_STATUS_DATA* status)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr || status == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::{} - invalid data (addon='{}', file='{}, status='{}')",
              __FUNCTION__, kodiBase, file, static_cast<const void*>(status));
    return -1;
  }

  SCacheStatus data = {0};
  int ret = static_cast<CFile*>(file)->IoControl(EIoControl::IOCTRL_CACHE_STATUS, &data);
  if (ret >= 0)
  {
    status->forward = data.forward;
    status->maxrate = data.maxrate;
    status->currate = data.currate;
    status->lowspeed = data.lowspeed;
    return true;
  }
  return false;
}

bool Interface_Filesystem::io_control_set_cache_rate(void* kodiBase, void* file, unsigned int rate)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::{} - invalid data (addon='{}', file='{}')", __FUNCTION__,
              kodiBase, file);
    return -1;
  }

  return static_cast<CFile*>(file)->IoControl(EIoControl::IOCTRL_CACHE_SETRATE, &rate) >= 0 ? true
                                                                                            : false;
}

bool Interface_Filesystem::io_control_set_retry(void* kodiBase, void* file, bool retry)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_VFS::{} - invalid data (addon='{}', file='{}')", __FUNCTION__,
              kodiBase, file);
    return -1;
  }

  return static_cast<CFile*>(file)->IoControl(EIoControl::IOCTRL_SET_RETRY, &retry) >= 0 ? true
                                                                                         : false;
}

char** Interface_Filesystem::get_property_values(
    void* kodiBase, void* file, int type, const char* name, int* numValues)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr || name == nullptr || numValues == nullptr)
  {
    CLog::Log(LOGERROR,
              "Interface_Filesystem::{} - invalid data (addon='{}', file='{}', name='{}', "
              "numValues='{}')",
              __FUNCTION__, kodiBase, file, static_cast<const void*>(name),
              static_cast<void*>(numValues));
    return nullptr;
  }

  XFILE::FileProperty internalType;
  switch (type)
  {
    case ADDON_FILE_PROPERTY_RESPONSE_PROTOCOL:
      internalType = XFILE::FILE_PROPERTY_RESPONSE_PROTOCOL;
      break;
    case ADDON_FILE_PROPERTY_RESPONSE_HEADER:
      internalType = XFILE::FILE_PROPERTY_RESPONSE_HEADER;
      break;
    case ADDON_FILE_PROPERTY_CONTENT_TYPE:
      internalType = XFILE::FILE_PROPERTY_CONTENT_TYPE;
      break;
    case ADDON_FILE_PROPERTY_CONTENT_CHARSET:
      internalType = XFILE::FILE_PROPERTY_CONTENT_CHARSET;
      break;
    case ADDON_FILE_PROPERTY_MIME_TYPE:
      internalType = XFILE::FILE_PROPERTY_MIME_TYPE;
      break;
    case ADDON_FILE_PROPERTY_EFFECTIVE_URL:
      internalType = XFILE::FILE_PROPERTY_EFFECTIVE_URL;
      break;
    default:
      CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', file='{}')",
                __FUNCTION__, kodiBase, file);
      return nullptr;
  };
  std::vector<std::string> values =
      static_cast<CFile*>(file)->GetPropertyValues(internalType, name);
  *numValues = values.size();
  char** ret = static_cast<char**>(malloc(sizeof(char*) * values.size()));
  for (int i = 0; i < *numValues; ++i)
  {
    ret[i] = strdup(values[i].c_str());
  }
  return ret;
}

void* Interface_Filesystem::curl_create(void* kodiBase, const char* url)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || url == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', url='{}')",
              __FUNCTION__, kodiBase, static_cast<const void*>(url));
    return nullptr;
  }

  CFile* file = new CFile;
  if (file->CURLCreate(url))
    return static_cast<void*>(file);

  delete file;
  return nullptr;
}

bool Interface_Filesystem::curl_add_option(
    void* kodiBase, void* file, int type, const char* name, const char* value)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr || name == nullptr || value == nullptr)
  {
    CLog::Log(
        LOGERROR,
        "Interface_Filesystem::{} - invalid data (addon='{}', file='{}', name='{}', value='{}')",
        __FUNCTION__, kodiBase, file, static_cast<const void*>(name),
        static_cast<const void*>(value));
    return false;
  }

  XFILE::CURLOPTIONTYPE internalType;
  switch (type)
  {
    case ADDON_CURL_OPTION_OPTION:
      internalType = XFILE::CURL_OPTION_OPTION;
      break;
    case ADDON_CURL_OPTION_PROTOCOL:
      internalType = XFILE::CURL_OPTION_PROTOCOL;
      break;
    case ADDON_CURL_OPTION_CREDENTIALS:
      internalType = XFILE::CURL_OPTION_CREDENTIALS;
      break;
    case ADDON_CURL_OPTION_HEADER:
      internalType = XFILE::CURL_OPTION_HEADER;
      break;
    default:
      throw std::logic_error("Interface_Filesystem::curl_add_option - invalid curl option type");
      return false;
  };

  return static_cast<CFile*>(file)->CURLAddOption(internalType, name, value);
}

bool Interface_Filesystem::curl_open(void* kodiBase, void* file, unsigned int flags)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiBase);
  if (addon == nullptr || file == nullptr)
  {
    CLog::Log(LOGERROR, "Interface_Filesystem::{} - invalid data (addon='{}', file='{}')",
              __FUNCTION__, kodiBase, file);
    return false;
  }

  return static_cast<CFile*>(file)->CURLOpen(flags);
}

} /* namespace ADDON */
} /* extern "C" */

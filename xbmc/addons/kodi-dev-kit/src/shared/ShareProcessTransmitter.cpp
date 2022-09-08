/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "ShareProcessTransmitter.h"

#ifdef TARGET_POSIX
#include "SharedMemControlPosixMMAP.h"
#elif TARGET_WINDOWS
#include "SharedMemControlWindows.h"
#else
#error No valid OS defined
#endif

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

CShareProcessTransmitter::CShareProcessTransmitter(const std::string& identifier,
                                                   bool mainThread,
                                                   size_t size /* = SHARED_DEFAULT_MEM_SIZE*/)
  : m_identifier(identifier),
    m_mainThread(mainThread),
#ifdef TARGET_POSIX
    m_sharedMem(new CSharedMemControlPosix(identifier, size))
#elif TARGET_WINDOWS
    m_sharedMem(new CSharedMemControlWindows(identifier, size))
#else
    m_sharedMem(nullptr)
#endif
{
}

CShareProcessTransmitter::~CShareProcessTransmitter()
{
  Destroy();
  delete m_sharedMem;
}

bool CShareProcessTransmitter::Create(bool initial)
{
  if (!m_sharedMem->Create(initial))
    return false;

  m_created = true;
  return true;
}

void CShareProcessTransmitter::Destroy()
{
  m_created = false;
  while (m_active)
  {
    m_sharedMem->Unlock_Caller();
  }
  m_sharedMem->Destroy();
}

bool CShareProcessTransmitter::SendMessage(int group, int func)
{
  if (!m_created)
    return false;

  // Lock to prevent other calls as long active.
  m_sharedMem->Lock();
  m_active = true;

  apiShareData* sharedMem = m_sharedMem->GetSharedMem();
  sharedMem->group = group;
  sharedMem->func = func;
  sharedMem->bigger = false;
  sharedMem->data_size_complete = 0;
  sharedMem->data_size = 0;
  sharedMem->main_thread = m_mainThread;

  // Unlock wait process inside kodi to make requested work.
  m_sharedMem->Unlock_Target();

  // Lock here until Kodi finished.
  m_sharedMem->Lock_Caller();

  m_active = false;
  m_sharedMem->Unlock();

  return m_created;
}

bool CShareProcessTransmitter::SendMessage(int group, int func, const msgpack::sbuffer& in)
{
  if (!m_created)
    return false;

  // Lock to prevent other calls as long active.
  m_sharedMem->Lock();
  m_active = true;

  apiShareData* sharedMem = m_sharedMem->GetSharedMem();
  sharedMem->group = group;
  sharedMem->func = func;

  size_t size = in.size();
  sharedMem->bigger = size > DEFAULT_SHARED_MEM_ARRAY_SIZE;
  if (!sharedMem->bigger)
  {
    sharedMem->data_size_complete = size;
    sharedMem->data_size = size;

    memcpy(sharedMem->data, in.data(), sharedMem->data_size);

    m_sharedMem->Unlock_Target();
    m_sharedMem->Lock_Caller();
  }
  else
  {
    size_t ptr = 0;
    sharedMem->data_size_complete = size;

    while (ptr < sharedMem->data_size_complete)
    {
      //       fprintf(stderr, "1 sharedMem->bigger %i %zu %zu\n", sharedMem->bigger, sharedMem->data_size, sharedMem->data_size_complete);

      sharedMem->data_size =
          size > DEFAULT_SHARED_MEM_ARRAY_SIZE ? DEFAULT_SHARED_MEM_ARRAY_SIZE : size;

      size -= DEFAULT_SHARED_MEM_ARRAY_SIZE;

      memcpy(sharedMem->data, in.data() + ptr, sharedMem->data_size);

      m_sharedMem->Unlock_Target();
      m_sharedMem->Lock_Caller();

      ptr += sharedMem->data_size;
    }
  }

  sharedMem->main_thread = m_mainThread;

  m_active = false;
  m_sharedMem->Unlock();

  return m_created;
}

bool CShareProcessTransmitter::SendMessage(int group,
                                           int func,
                                           const msgpack::sbuffer& in,
                                           msgpack::sbuffer& ret)
{
  if (!m_created)
    return false;

  // Lock to prevent other calls as long active.
  m_sharedMem->Lock();
  m_active = true;

  apiShareData* sharedMem = m_sharedMem->GetSharedMem();
  sharedMem->group = group;
  sharedMem->func = func;

  size_t size = in.size();
  sharedMem->bigger = size > DEFAULT_SHARED_MEM_ARRAY_SIZE;
  if (!sharedMem->bigger)
  {
    sharedMem->data_size_complete = size;
    sharedMem->data_size = size;

    memcpy(sharedMem->data, in.data(), sharedMem->data_size);

    m_sharedMem->Unlock_Target();
    m_sharedMem->Lock_Caller();
  }
  else
  {
    size_t ptr = 0;
    sharedMem->data_size_complete = size;

    while (ptr < sharedMem->data_size_complete)
    {
      //       fprintf(stderr, "1 sharedMem->bigger %i %zu %zu\n", sharedMem->bigger, sharedMem->data_size, sharedMem->data_size_complete);

      sharedMem->data_size =
          size > DEFAULT_SHARED_MEM_ARRAY_SIZE ? DEFAULT_SHARED_MEM_ARRAY_SIZE : size;

      size -= DEFAULT_SHARED_MEM_ARRAY_SIZE;

      memcpy(sharedMem->data, in.data() + ptr, sharedMem->data_size);

      m_sharedMem->Unlock_Target();
      m_sharedMem->Lock_Caller();

      ptr += sharedMem->data_size;
    }
  }

  sharedMem->main_thread = m_mainThread;

  if (m_created)
  {
    if (!sharedMem->bigger_return)
    {
      ret.write(sharedMem->data, sharedMem->data_size);
    }
    else
    {
      size_t ptr = 0;
      while (ptr < sharedMem->data_size_complete)
      {
        ret.write(sharedMem->data, sharedMem->data_size);
        ptr += sharedMem->data_size;

        m_sharedMem->Unlock_Target();
        m_sharedMem->Lock_Caller();
      }
    }
  }

  m_active = false;
  m_sharedMem->Unlock();

  return m_created;
}

bool CShareProcessTransmitter::SendMessageOnlyGetReturn(int group, int func, msgpack::sbuffer& ret)
{
  if (!m_created)
    return false;

  // Lock to prevent other calls as long active.
  m_sharedMem->Lock();
  m_active = true;

  apiShareData* sharedMem = m_sharedMem->GetSharedMem();
  sharedMem->group = group;
  sharedMem->func = func;
  sharedMem->data_size = 0;

  // Unlock wait process inside kodi to make requested work.
  m_sharedMem->Unlock_Target();

  // Lock here until Kodi finished.
  m_sharedMem->Lock_Caller();

  if (m_created)
  {
    if (!sharedMem->bigger_return)
    {
      ret.write(sharedMem->data, sharedMem->data_size);
    }
    else
    {
      size_t ptr = 0;
      while (ptr < sharedMem->data_size_complete)
      {
        ret.write(sharedMem->data, sharedMem->data_size);
        ptr += sharedMem->data_size;

        m_sharedMem->Unlock_Target();
        m_sharedMem->Lock_Caller();
      }
    }
  }

  m_active = false;
  m_sharedMem->Unlock();

  return m_created;
}

const std::string& CShareProcessTransmitter::GetLastErrorText() const
{
  return m_sharedMem->GetLastErrorText();
}

int64_t CShareProcessTransmitter::GetPID() const
{
  return m_sharedMem->GetPID();
}

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */

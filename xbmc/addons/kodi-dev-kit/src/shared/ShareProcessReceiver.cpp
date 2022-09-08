/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "ShareProcessReceiver.h"

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

CShareProcessReceiver::CShareProcessReceiver(const std::string& identifier,
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

CShareProcessReceiver::~CShareProcessReceiver()
{
  Destroy();
  delete m_sharedMem;
}

bool CShareProcessReceiver::Create(bool initial, bool useThread)
{
  if (!m_sharedMem->Create(initial))
  {
    fprintf(stderr, "Failed to create receiver, error '%s'\n",
            m_sharedMem->GetLastErrorText().c_str());
    return false;
  }

  m_running = true;
  m_useThread = useThread;
  if (m_useThread)
    m_thread = std::thread([&] { Process(); });

  return true;
}

void CShareProcessReceiver::Destroy()
{
  if (!m_running)
    return;

  m_running = false;
  m_sharedMem->Unlock_Target();

  if (m_useThread)
  {
    if (m_thread.joinable())
    {
      m_thread.join();
    }
  }
  else
  {
    while (m_active)
    {
      m_sharedMem->Unlock_Target();
    }
  }

  m_sharedMem->Destroy();
}

bool CShareProcessReceiver::ProcessOutside()
{
  if (!m_running)
    return false;

  bool ret = false;

  m_active = true;

  if (m_sharedMem->Lock_Target())
  {
    if (!m_running)
    {
      m_active = false;
      return false;
    }

    apiShareData* const sharedMem = m_sharedMem->GetSharedMem();

    char* data;
    size_t data_size;
    bool bigger_used = sharedMem->bigger;
    //     if (sharedMem->bigger)
    //       fprintf(stderr, "2 sharedMem->bigger %i\n", sharedMem->bigger);
    if (!bigger_used)
    {
      data = sharedMem->data;
      data_size = sharedMem->data_size;
    }
    else
    {
      size_t ptr = 0;
      data = new char[sharedMem->data_size_complete];
      data_size = sharedMem->data_size_complete;

      while (true)
      {
        memcpy(data + ptr, sharedMem->data, sharedMem->data_size);
        ptr += sharedMem->data_size;
        if (ptr >= sharedMem->data_size_complete)
          break;

        m_sharedMem->Unlock_Caller();
        m_sharedMem->Lock_Target();
      }
    }

    msgpack::sbuffer retBuffer;
    msgpack::object_handle input;
    if (data_size != 0)
      input = msgpack::unpack(data, data_size);

    if (HandleMessage(sharedMem->group, sharedMem->func, input, retBuffer))
    {
      sharedMem->data_size_complete = retBuffer.size();
      if (sharedMem->data_size_complete <= SHARED_DEFAULT_MEM_SIZE)
      {
        sharedMem->bigger_return = false;
        sharedMem->data_size = sharedMem->data_size_complete;
        memcpy(sharedMem->data, retBuffer.data(), sharedMem->data_size);
      }
      else
      {
        sharedMem->bigger_return = true;
        sharedMem->data_size = SHARED_DEFAULT_MEM_SIZE;

        size_t ptr = 0;
        while (ptr < sharedMem->data_size_complete)
        {
          sharedMem->data_size = retBuffer.size() - ptr > SHARED_DEFAULT_MEM_SIZE
                                     ? SHARED_DEFAULT_MEM_SIZE
                                     : retBuffer.size() - ptr;
          memcpy(sharedMem->data, retBuffer.data() + ptr, sharedMem->data_size);
          ptr += sharedMem->data_size;

          m_sharedMem->Unlock_Caller();
          m_sharedMem->Lock_Target();
        }
      }

      ret = true;
    }
    else
    {
      ret = false;
    }

    if (bigger_used)
    {
      delete[] data;
    }
  }

  m_sharedMem->Unlock_Caller();
  m_active = false;

  return ret;
}

void CShareProcessReceiver::Process()
{
  while (m_running)
  {
    ProcessOutside();
  }
}

void CShareProcessReceiver::RegisterReceiver(SharedProcess func)
{
  m_receivers.emplace_back(func);
}

bool CShareProcessReceiver::HandleMessage(int group,
                                          int func,
                                          const msgpack::unpacked& in,
                                          msgpack::sbuffer& out)
{
  bool ret = false;
  m_sharedMem->SetActive();

  out.clear();

  for (const auto& receiver : m_receivers)
  {
    if (receiver(group, func, in, out))
    {
      ret = true;
      break;
    }
  }

  m_sharedMem->SetInactive();

  return ret;
}

const std::string& CShareProcessReceiver::GetLastErrorText() const
{
  return m_sharedMem->GetLastErrorText();
}

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */

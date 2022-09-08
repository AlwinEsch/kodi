/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <functional>

#include <msgpack.hpp>

#if (defined TARGET_WINDOWS)
typedef HANDLE SEMAPHORE_ID;
typedef unsigned long THREAD_ID;
#elif (defined TARGET_POSIX)
#include <semaphore.h>

typedef sem_t SEMAPHORE_ID;
typedef pthread_t THREAD_ID;
#else
#error Not supported OS used
#endif

#define SHARED_DEFAULT_MEM_SIZE 32768 * 8
#define DEFAULT_SHARED_MEM_ARRAY_SIZE SHARED_DEFAULT_MEM_SIZE - (sizeof(uint32_t) * 10)

/*!
 * @brief Data structure to share memory between independent process apps.
 *
 * This used to exchange data between Kodi and addons.
 *
 * @todo Check for realloc to prevent additional data loops in some cases when
 * more needs to send?
 */
typedef struct apiShareData
{
  //! If set to true related shared memory is inited and available
  bool inited;

  //! Locking mutex of caller where blocks until target has done his work.
  //! Lock called by caller, unlock called by target.
  SEMAPHORE_ID sem_segment_to_caller;

  //! Locking mutex of target where where wait until caller request work.
  //! Lock called by target, unlock called by caller.
  SEMAPHORE_ID sem_segment_to_target;

  //! Normal locking mutex to prevent multiply access to shared memory at same time
  SEMAPHORE_ID sem_segment_lock;

  bool main_thread;
  THREAD_ID calling_thread;

  int api_level_caller;
  int group;
  int func;

  //! To inform about bigger memory size as here in data used and need loop until complete.
  bool bigger;

  //! Like @ref bigger, only related to return of call.
  bool bigger_return;

  //! Data size complete of all, normally equal to @ref SHARED_DEFAULT_MEM_SIZE.
  //! If bigger as @ref data_size target need to copy to itself until complete size done.
  size_t data_size_complete;

  //! Data size used in this shared memory here by @ref data
  //!
  //! @note It should confirm in most cases that no bigger data size is used as then additional
  //! memory copy becomes needed!
  size_t data_size;

  //! Shared memory data array to exchange working data between processes.
  char data[SHARED_DEFAULT_MEM_SIZE / 2];

  size_t data_size_new;

  char data_new[SHARED_DEFAULT_MEM_SIZE / 2];
} apiShareData;

using SharedProcess =
    std::function<bool(int group, int func, const msgpack::unpacked& in, msgpack::sbuffer& out)>;

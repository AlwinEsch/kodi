/*
 *  Copyright (C) 2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "../SharedGroups.h"

#include <string>
#include <tuple>

/*
 * WARNING:
 * This file is not touched by autogeneration!
 *
 * As this here is for internal crosstalk between Kodi and addon dev-kit library
 * must it be set by hand as autogeneration can't know what todo.
 */

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_KILL_CHILD_V1)(void*, const char*);

struct directFuncToAddon_processor_h
{
  void* thisClassHdl;
  PFN_INT_KODI_KILL_CHILD_V1 kodi_kill_child;
};

typedef enum funcParent_Processor_h : int
{
  kodi_processor_CreateForNewThread = 100,
  kodi_processor_CreateForNewProcess = 101,
  kodi_processor_NewProcessStarted = 102,
} funcParent_Processor_h;

// clang-format off
/*---AUTO_GEN_PARSE<FUNC_PARENT_TUPLES>---*/
typedef std::tuple<std::string>                                                   msgParent__IN_kodi_processor_CreateForNewThread;
typedef std::tuple<std::string, int64_t>                                          msgParent_OUT_kodi_processor_CreateForNewThread;
typedef std::tuple<std::string>                                                   msgParent__IN_kodi_processor_CreateForNewProcess;
typedef std::tuple<std::string>                                                   msgParent_OUT_kodi_processor_CreateForNewProcess;
typedef std::tuple<std::string, std::string, int64_t>                             msgParent__IN_kodi_processor_NewProcessStarted;
typedef std::tuple<DummyValue>                                                    msgParent_OUT_kodi_processor_NewProcessStarted;
/*---AUTO_GEN_PARSE<FUNC_PARENT_TUPLES_END>---*/
// clang-format on

//==============================================================================

typedef enum funcChild_Processor_h
{
  kodi_processor_CreateForNewChildThread = 200,
  kodi_processor_KillChild,
} funcChild_Processor_h;

// clang-format off
/*---AUTO_GEN_PARSE<FUNC_CHILD_TUPLES>---*/
typedef std::tuple<std::string>                                                   msgChild__IN_kodi_processor_KillChild;
typedef std::tuple<DummyValue>                                                    msgChild_OUT_kodi_processor_KillChild;
typedef std::tuple<std::string>                                                   msgChild__IN_kodi_processor_CreateForNewThread;
typedef std::tuple<std::string>                                                   msgChild_OUT_kodi_processor_CreateForNewThread;
/*---AUTO_GEN_PARSE<FUNC_CHILD_TUPLES_END>---*/
// clang-format on

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */

/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/*---AUTO_GEN_PARSE<$$CORE_SYSTEM_NAME:linux>---*/

#include "ProcessInfoLinux.h"

#include "utils/StringUtils.h"
#include "utils/TimeUtils.h"

#include <fstream>
#include <vector>

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

#define m_lastTotalUser CProcessInfo::m_value1;
#define m_lastTotalUserLow CProcessInfo::m_value2;
#define m_lastTotalSys CProcessInfo::m_value3;
#define m_lastTotalIdle CProcessInfo::m_value4;

CProcessInfoLinux::CProcessInfoLinux(std::shared_ptr<ADDON::IAddon> addon, int64_t used_pid)
  : CProcessInfo(addon, used_pid)
{
  m_currentInfo.m_addon = m_addon;

  std::ifstream procstat_stream("/proc/stat", std::ios_base::in);
  std::string line;
  std::getline(procstat_stream, line);

  uint64_t user;
  uint64_t nice2;
  uint64_t system;
  uint64_t idle;
  sscanf(line.c_str(), "%*s %lu %lu %lu %lu", &user, &nice2, &system, &idle);
  m_lastTotalCPU = user + nice2 + system + idle;

  char buf[128];
  snprintf(buf, sizeof(buf), "/proc/%ld/stat", m_pid);
  std::ifstream stat_stream(buf, std::ios_base::in);

  int64_t pid;
  std::string comm;
  char state[1];
  int ppid;
  int pgrp;
  int session;
  int tty_nr;
  int tpgid;
  unsigned int flags;
  uint32_t minflt;
  uint32_t cminflt;
  uint32_t majflt;
  uint32_t cmajflt;
  uint32_t utime;
  uint32_t stime;
  int32_t cutime;
  int32_t cstime;
  int32_t priority;
  int32_t nice;
  int32_t num_threads;
  int32_t itrealvalue;
  uint64_t starttime;
  uint32_t vsize;
  uint32_t rss;

  stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >>
      minflt >> cminflt >> majflt >> cmajflt >> utime >> stime >> cutime >> cstime >> priority >>
      nice >> num_threads >> itrealvalue >> starttime >> vsize >> rss;

  stat_stream.close();

  m_lastProcessCPU = utime + stime;

  std::ifstream cpuinfo_stream("/proc/cpuinfo", std::ios_base::in);
  for (std::string line; std::getline(cpuinfo_stream, line);)
  {
    if (StringUtils::StartsWith(line, "processor"))
    {
      ++m_numProcessors;
    }
  }
}

ProcessInfoData CProcessInfoLinux::Calculate()
{
  if (CTimeUtils::GetFrameTime() - m_lastInfoTime < UPDATE_INTERVAL)
    return m_currentInfo;

  m_lastInfoTime = CTimeUtils::GetFrameTime();

  char buf[128];
  snprintf(buf, sizeof(buf), "/proc/%ld/stat", m_pid);
  std::ifstream stat_stream(buf, std::ios_base::in);

  int64_t pid;
  std::string comm;
  char state[1];
  int ppid;
  int pgrp;
  int session;
  int tty_nr;
  int tpgid;
  unsigned int flags;
  uint32_t minflt;
  uint32_t cminflt;
  uint32_t majflt;
  uint32_t cmajflt;
  uint64_t utime;
  uint64_t stime;
  uint64_t cutime;
  uint64_t cstime;
  int32_t priority;
  int32_t nice;
  int32_t num_threads;
  int32_t itrealvalue;
  uint64_t starttime;
  uint32_t vsize;
  uint32_t rss;

  stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >>
      minflt >> cminflt >> majflt >> cmajflt >> utime >> stime >> cutime >> cstime >> priority >>
      nice >> num_threads >> itrealvalue >> starttime >> vsize >> rss;

  stat_stream.close();

  m_currentInfo.m_pid = pid;

  snprintf(buf, sizeof(buf), "/proc/%ld/status", m_pid);
  std::ifstream status_stream(buf, std::ios_base::in);
  for (std::string line; std::getline(status_stream, line);)
  {
    StringUtils::RemoveDuplicatedSpacesAndTabs(line);
    if (StringUtils::StartsWith(line, "RssShmem:"))
    {
      m_currentInfo.m_usedSharedMemoryAmount = atoll(line.c_str() + sizeof("RssShmem:")) * 1024;
    }
    else if (StringUtils::StartsWith(line, "VmSize:"))
    {
      m_currentInfo.m_usedVirtualMemoryAmount = atoll(line.c_str() + sizeof("VmSize:")) * 1024;
    }
    else if (StringUtils::StartsWith(line, "VmRSS:"))
    {
      m_currentInfo.m_usedPhysicalMemoryAmount = atoll(line.c_str() + sizeof("VmRSS:")) * 1024;
    }
    else if (StringUtils::StartsWith(line, "Threads:"))
    {
      m_currentInfo.m_usedThreadsAmount = atoll(line.c_str() + sizeof("Threads:"));
    }
  }

  std::ifstream procstat_stream("/proc/stat", std::ios_base::in);
  std::string line;
  std::getline(procstat_stream, line);

  uint64_t user;
  uint64_t nice2;
  uint64_t system;
  uint64_t idle;
  sscanf(line.c_str(), "%*s %lu %lu %lu %lu", &user, &nice2, &system, &idle);
  uint64_t currentTotalCPU = user + nice2 + system + idle;
  uint64_t currentProcessCPU = utime + stime;

  m_currentInfo.m_usedCPUPercentage = m_numProcessors * (currentProcessCPU - m_lastProcessCPU) *
                                      100 / (float)(currentTotalCPU - m_lastTotalCPU);

  m_lastTotalCPU = currentTotalCPU;
  m_lastProcessCPU = currentProcessCPU;

  return m_currentInfo;
}

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */

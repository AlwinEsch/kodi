/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/*---AUTO_GEN_PARSE<$$CORE_SYSTEM_NAME:linux,freebsd,android,osx,darwin_embedded>---*/

#include "LauncherPosix.h"

#include "../../utils/log.h"

#include <chrono>
#include <thread>

#include <sys/wait.h>
#include <unistd.h>

#if defined(TARGET_LINUX)
#include <sys/prctl.h>
#endif

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

// Set the calling thread's signal mask to new_sigmask and return
// the previous signal mask.
sigset_t SetSignalMask(const sigset_t& new_sigmask)
{
  sigset_t old_sigmask;
#if defined(OS_ANDROID)
  // POSIX says pthread_sigmask() must be used in multi-threaded processes,
  // but Android's pthread_sigmask() was broken until 4.1:
  // https://code.google.com/p/android/issues/detail?id=15337
  // http://stackoverflow.com/questions/13777109/pthread-sigmask-on-android-not-working
  RAW_CHECK(sigprocmask(SIG_SETMASK, &new_sigmask, &old_sigmask) == 0);
#else
  RAW_CHECK(pthread_sigmask(SIG_SETMASK, &new_sigmask, &old_sigmask) == 0);
#endif
  return old_sigmask;
}

void ResetChildSignalHandlersToDefaults()
{
  // The previous signal handlers are likely to be meaningless in the child's
  // context so we reset them to the defaults for now. http://crbug.com/44953
  // These signal handlers are set up at least in browser_main_posix.cc:
  // BrowserMainPartsPosix::PreEarlyInitialization and stack_trace_posix.cc:
  // EnableInProcessStackDumping.
  signal(SIGHUP, SIG_DFL);
  signal(SIGINT, SIG_DFL);
  signal(SIGILL, SIG_DFL);
  signal(SIGABRT, SIG_DFL);
  signal(SIGFPE, SIG_DFL);
  signal(SIGBUS, SIG_DFL);
  signal(SIGSEGV, SIG_DFL);
  signal(SIGSYS, SIG_DFL);
  signal(SIGTERM, SIG_DFL);
}

CLauncherPosix::CLauncherPosix(std::shared_ptr<ADDON::IAddon> addon, const std::string& uuid)
  : ILauncher(addon, uuid)
{
}

bool CLauncherPosix::Launch(const std::vector<char*>& argv, bool waitForExit)
{
  using namespace std::chrono;

  sigset_t full_sigset;
  sigfillset(&full_sigset);
  const sigset_t orig_sigmask = SetSignalMask(full_sigset);

  if (pipe(m_childToParent) < 0)
  {
    CLog::Log(LOGERROR, "CChildLauncherPosix::{}: Failed to create pipe for child process", __func__);
    return false;
  }

  int writeFD = m_childToParent[WRITE_FD];
  int readFD = m_childToParent[READ_FD];

  const auto before_fork = high_resolution_clock::now();
  pid_t pid = fork();

  // Always restore the original signal mask in the parent.
  if (pid != 0)
  {
    const auto after_fork = high_resolution_clock::now();
    SetSignalMask(orig_sigmask);

    duration<double> fork_time = duration_cast<duration<double>>(after_fork - before_fork);
    CLog::Log(LOGDEBUG, "CChildLauncherPosix::{}: Child fork time {}", __func__, fork_time.count());
  }

  if (pid == -1)
  {
    CLog::Log(LOGERROR, "CChildLauncherPosix::{}: Failed to create child process", __func__);
    return false;
  }

  if (pid == 0)
  {
    ResetChildSignalHandlersToDefaults();
    SetSignalMask(orig_sigmask);

#if defined(TARGET_LINUX)
    if (prctl(PR_SET_PDEATHSIG, SIGKILL) != 0)
    {
      fprintf(stderr, "prctl(PR_SET_PDEATHSIG) failed\n");
      _exit(127);
    }
#endif

    // crash_reporter writes output to stderr. Connect it to the status pipe fd.
    if (dup2(writeFD, STDERR_FILENO) == -1)
    {
      fprintf(stderr, "Failed to init stderr handling\n");
      _exit(127);
    }

    close(readFD);

    execvp(argv[0], argv.data());

    _exit(0);
  }

  close(writeFD);

  CLog::Log(
      LOGINFO,
      "CChildLauncherPosix::{}: Started child process for webbrowser addon (pid {}) in wait {}",
      __func__, pid, waitForExit ? "yes" : "no");
  m_pid = pid;

  if (waitForExit)
  {
    int status = 0;
    pid_t ret = HANDLE_EINTR(waitpid(pid, &status, 0));
    if (ret <= 0)
    {
      ProcessStatus(status);
    }
  }

  return true;
}

bool CLauncherPosix::Kill(bool wait)
{
  pid_t pid = m_pid;
  fprintf(stderr, "KILL\n");
  if (pid > 0)
  {
    if (m_lastStatus == ChildStatus::Running)
    {
      int cnt = 10;
      while (cnt-- > 0)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (ProcessActive() != ChildStatus::Running)
        {
          CLog::Log(LOGDEBUG, "CChildLauncherPosix::{}: Child correctly stopped itself (pid {})",
                    __func__, pid);
          return true;
        }
      }

      CLog::Log(LOGDEBUG, "CChildLauncherPosix::{}: Forcing stop of child (pid {})", __func__,
                pid);

      bool did_terminate = kill(pid, SIGTERM) == 0;

      if (wait && did_terminate)
      {
        cnt = 50;
        pid_t ret_pid = 0;
        while (ret_pid <= 0 && cnt-- > 0)
        {
          std::this_thread::sleep_for(std::chrono::milliseconds(50));
          ret_pid = HANDLE_EINTR(waitpid(pid, nullptr, WNOHANG));
        }
        if (ret_pid > 0)
          return true;

        did_terminate = kill(pid, SIGKILL) == 0;
        if (did_terminate)
        {
          ret_pid = HANDLE_EINTR(waitpid(pid, nullptr, 0));
          if (ret_pid > 0)
            return true;
        }
      }

      if (!did_terminate)
      {
        CLog::Log(LOGERROR, "CChildLauncherPosix::{}: Unable to terminate process {}", __func__,
                  pid);
        return false;
      }
    }

    if (m_childToParent[READ_FD] > 0)
    {
      close(m_childToParent[READ_FD]);
      m_childToParent[READ_FD] = -1;
    }
  }

  return true;
}

ChildStatus CLauncherPosix::ProcessActive()
{
  pid_t pid = m_pid;
  if (pid > 0)
  {
    int status = 0;
    pid_t ret = waitpid(pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
    if (ret == 0)
    {
      m_lastStatus = ChildStatus::Running;
    }
    else if (ret == -1)
    {
      if (m_childToParent[READ_FD] > 0)
      {
        constexpr int BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE + 1] = {0};
        m_stacktrace.clear();
        while (read(m_childToParent[READ_FD], buffer, BUFFER_SIZE) > 0)
        {
          m_stacktrace += buffer;
          memset(buffer, 0, BUFFER_SIZE);
        }
        fprintf(stderr, "--------->>>>>>>>>>>>>> %s\n", m_stacktrace.c_str());
        close(m_childToParent[READ_FD]);
        m_childToParent[READ_FD] = -1;
      }

      CLog::Log(LOGERROR, "CChildLauncherPosix::{}: Asked sandbox process pid {} no more present",
                __func__, pid);
      if (m_lastStatus == ChildStatus::Running)
        m_lastStatus = ChildStatus::StoppedByUnknown;
      return m_lastStatus;
    }
    else
    {
      ProcessStatus(status);
    }
  }

  return m_lastStatus;
}

void CLauncherPosix::ProcessStatus(int status)
{
  if (WIFEXITED(status))
  {
    m_exitCode = WEXITSTATUS(status);
    m_lastStatus = ChildStatus::ExitedNormal;
    CLog::Log(LOGDEBUG,
              "CChildLauncherPosix::{}: Sandbox process pid {}, stopped normal with exit code {}",
              __func__, m_pid, m_exitCode);
  }
  else if (WIFSIGNALED(status))
  {
    m_exitCode = WTERMSIG(status);
    fprintf(stderr, "<<<<<<<<<<<<< %i %i %i %i %i %i\n", WSTOPSIG(status), WTERMSIG(status),
            WCOREDUMP(status), WIFSIGNALED(status), WIFSTOPPED(status), WIFEXITED(status));
    if (m_exitCode == SIGSEGV)
    {
      m_lastStatus = ChildStatus::SeqmentionFault;
      CLog::Log(LOGFATAL, "CChildLauncherPosix::{}: Sandbox process pid {} with seqmention fault",
                __func__, m_pid, m_exitCode);
    }
    else
    {
      m_lastStatus = ChildStatus::StoppedBySignal;
      CLog::Log(LOGFATAL, "CChildLauncherPosix::{}: Sandbox process pid {} signaled with {}",
                __func__, m_pid, m_exitCode);
    }
  }
  else if (WIFSTOPPED(status))
  {
    m_exitCode = WSTOPSIG(status);
    m_lastStatus = ChildStatus::StoppedBySignal;
    CLog::Log(LOGFATAL,
              "CChildLauncherPosix::{}: Sandbox process pid {} stopped from outside with {}",
              __func__, m_pid, m_exitCode);
  }
  else
  {
    m_lastStatus = ChildStatus::StoppedByUnknown;
    CLog::Log(LOGFATAL,
              "CChildLauncherPosix::{}: Sandbox process pid {} stopped with unknown status {}",
              __func__, m_pid, status);
  }
}

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */

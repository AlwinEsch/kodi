/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "cxxopts.hpp"

#define BACKWARD_HAS_DW 1
// #include "third_party/backward-cpp/backward.hpp"

#include <stdio.h>
#include <string.h>

#include <dlfcn.h>
#include <kodi/c-api/addon_base.h>
#include <unistd.h>

const std::string text =
    "Add-on helper executable to launch an add-on library as executable in Kodi.\n"
    "Most values are only used within loaded library itself and forwarded to it from here.\n"
    "\n"
    "WARNING: Not used by an user itself, only as helper app inside Kodi!\n";

struct VALUES
{
  std::string m_addon_lib_path;
  std::string m_devkit_path;
  std::string m_devkit_lib;
  std::string m_devkit_report_path;
  std::string m_main_shared;
  bool m_debug{false};
  int m_used_api_level{__KDK_MAJOR__};
};

void parse(int argc, const char* argv[], VALUES& values)
{
  try
  {
    cxxopts::Options options(argv[0], text);
    options.positional_help("[optional args]").show_positional_help();

    options.set_width(70).set_tab_expansion().allow_unrecognised_options().add_options()(
        "D,debug", "Enable debugging", cxxopts::value<bool>()->default_value("false"))(
        "a,addon", "Add-on library to run", cxxopts::value<std::string>())(
        "d,direct_api", "Direct API interface identifier", cxxopts::value<std::string>())(
        "L,dir", "Direct API helper add-on path", cxxopts::value<std::string>())(
        "l,dl", "Dev kit library to use", cxxopts::value<std::string>())(
        "m,main-shared", "Shared memory identifier", cxxopts::value<std::string>())(
        "V,api", "Highest API level version to use", cxxopts::value<int>())(
        "r,report-path", "Add-on problems store path", cxxopts::value<std::string>())(
        "v,version", "Get created executable API support version")("h,help", "Print help");

    auto result = options.parse(argc, argv);

    //sleep(15);
    values.m_debug = result["debug"].as<bool>();
    if (values.m_debug)
    {
      std::printf("Start loading an addon library as an independent process\n");
      std::printf("Used command flags:\n");
      for (const auto& kv : result)
      {
        std::printf(" - %s = %s\n", kv.key().c_str(), kv.value().c_str());
      }
    }

    if (values.m_debug || result.count("version"))
    {
      std::printf("API version: %i.%i.%i\n", __KDK_MAJOR__, __KDK_MINOR__, __KDK_BETA__);
      if (!values.m_debug)
        exit(0);
    }

    if (result.count("api"))
      values.m_used_api_level = result["api"].as<int>();

    if (result.count("help"))
    {
      std::cout << options.help({"", "Group"}) << std::endl;
      exit(0);
    }

    if (result.count("dir"))
    {
      values.m_devkit_path = result["dir"].as<std::string>();
    }

    if (result.count("dl"))
    {
      values.m_devkit_lib = result["dl"].as<std::string>();
    }

    if (result.count("main-shared"))
    {
      values.m_main_shared = result["main-shared"].as<std::string>();
    }

    if (result.count("report-path"))
    {
      values.m_devkit_report_path = result["report-path"].as<std::string>();
    }

    if (result.count("addon"))
    {
      const std::string& addon_library = result["addon"].as<std::string>();
      if (addon_library.empty() || access(addon_library.c_str(), F_OK) != 0)
      {
        std::printf("ERROR: Wanted add-on library not found or usable: %s\n",
                    addon_library.c_str());
        exit(1);
      }

      values.m_addon_lib_path = addon_library;
    }

    if (values.m_addon_lib_path.empty())
    {
      std::printf("ERROR: Needed values not set and unusable!\n");
      exit(1);
    }
  }
  catch (const cxxopts::OptionException& e)
  {
    std::cout << "error parsing options: " << e.what() << std::endl;
    exit(1);
  }
}

ATTR_DLL_EXPORT int main(int argc, const char* argv[])
{
  VALUES values;
  parse(argc, argv, values);

//   backward::StackTrace m_stackTrace;
//   backward::SignalHandling m_signalHandling;
//
//   m_stackTrace.load_here(32);

// using namespace backward;
// StackTrace st; st.load_here(32);
//
//   std::vector<int> signals;
//   signals.push_back(SIGSEGV);
//   SignalHandling sh(signals);

/*Printer p;
p.object = true;
p.color_mode = ColorMode::always;
p.address = true;
p.print(st, stderr);
*/


  if (values.m_debug)
    std::printf("Starting add-on library load to execute in Kodi: %s\n",
                values.m_addon_lib_path.c_str());

  int ret = 1;
  if(values.m_addon_lib_path.substr(values.m_addon_lib_path.find_last_of(".") + 1) == "java")
  {
    const std::string devkit_path = values.m_devkit_path + "java";
    const std::string java_lib_path = "-Djava.library.path=" + values.m_devkit_path + "java";
    const std::string api = std::to_string(values.m_used_api_level);
    const std::string main_shared = "--main-shared=" + values.m_main_shared;
    const std::string report_path = "-XX:ErrorFile=" + values.m_devkit_report_path + "/hs_err_pid" + values.m_main_shared + ".log";

    char* call_values[] = {
      const_cast<char*>("java"),
      const_cast<char*>("-cp"),
      const_cast<char*>(devkit_path.c_str()),
      const_cast<char*>(java_lib_path.c_str()),
      const_cast<char*>(report_path.c_str()),
      const_cast<char*>(values.m_addon_lib_path.c_str()),
      const_cast<char*>(main_shared.c_str()),
      const_cast<char*>("--api"),
      const_cast<char*>(api.c_str()),
      const_cast<char*>("--dir"),
      const_cast<char*>(values.m_devkit_path.c_str()),
      const_cast<char*>("--dl"),
      const_cast<char*>(values.m_devkit_lib.c_str()),
      nullptr
    };

    ret = execvp(call_values[0], call_values);
  }
  else
  {
    void* dll = dlopen(values.m_addon_lib_path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (dll == nullptr)
    {
      std::printf("ERROR: Failed to load add-on library: %s\n", values.m_addon_lib_path.c_str());
      return 1;
    }

    int (*main_call)(int, const char**) = reinterpret_cast<decltype(main_call)>(dlsym(dll, "main"));
    if (main_call == nullptr)
    {
      std::printf("ERROR: Failed to get \"main(int, const char*[])\" from: %s\n",
                  values.m_addon_lib_path.c_str());
      dlclose(dll);
      return 1;
    }

    ret = main_call(argc, argv);

    dlclose(dll);
  }

  if (values.m_debug)
    std::printf("Stopped add-on library execute in Kodi: %s\n", values.m_addon_lib_path.c_str());

  return ret;
}

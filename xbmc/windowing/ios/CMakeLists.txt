set(SOURCES WinEventsIOS.mm
            WinSystemIOS.mm
            VideoSyncIos.cpp)
set(HEADERS WinEventsIOS.h
            WinSystemIOS.h
            VideoSyncIos.h)

if(TARGET ${APP_NAME_LC}::Vulkan)
  list(APPEND SOURCES WinEventsIOSVulkan.mm)
  list(APPEND HEADERS WinEventsIOSVulkan.h)
endif()
if(TARGET ${APP_NAME_LC}::OpenGLES)
  list(APPEND SOURCES WinEventsIOSOpenGLES.mm)
  list(APPEND HEADERS WinEventsIOSOpenGLES.h)
endif()

core_add_library(windowing_ios)

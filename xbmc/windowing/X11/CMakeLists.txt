set(SOURCES OptionalsReg.cpp
            OSScreenSaverX11.cpp
            WinEventsX11.cpp
            WinSystemX11.cpp
            XRandR.cpp
            X11DPMSSupport.cpp)

set(HEADERS OptionalsReg.h
            OSScreenSaverX11.h
            WinEventsX11.h
            WinSystemX11.h
            XRandR.h
            X11DPMSSupport.h)

if(TARGET ${APP_NAME_LC}::GLX)
  list(APPEND SOURCES GLContextGLX.cpp
                      VideoSyncGLX.cpp)
  list(APPEND HEADERS GLContextGLX.h
                      VideoSyncGLX.h)
endif()

if(TARGET ${APP_NAME_LC}::Vulkan)
  list(APPEND SOURCES WinSystemX11Vulkan.cpp)
  list(APPEND HEADERS WinSystemX11Vulkan.h)
endif()
if(TARGET ${APP_NAME_LC}::OpenGl OR TARGET ${APP_NAME_LC}::OpenGLES)
  list(APPEND SOURCES GLContext.cpp
                      GLContextEGL.cpp)
  list(APPEND HEADERS GLContext.h
                      GLContextEGL.h)
endif()
if(TARGET ${APP_NAME_LC}::OpenGl)
  list(APPEND SOURCES VideoSyncOML.cpp
                      WinSystemX11GLContext.cpp)
  list(APPEND HEADERS VideoSyncOML.h
                      WinSystemX11GLContext.h)
endif()
if(TARGET ${APP_NAME_LC}::OpenGLES)
  list(APPEND SOURCES WinSystemX11GLESContext.cpp)
  list(APPEND HEADERS WinSystemX11GLESContext.h)
endif()

core_add_library(windowing_X11)

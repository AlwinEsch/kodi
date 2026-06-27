add_subdirectory(drm)

set(SOURCES OptionalsReg.cpp
            WinSystemGbm.cpp
            VideoSyncGbm.cpp
            GBMUtils.cpp
            GBMDPMSSupport.cpp)

set(HEADERS OptionalsReg.h
            WinSystemGbm.h
            VideoSyncGbm.h
            GBMUtils.h
            GBMDPMSSupport.h)

if(TARGET ${APP_NAME_LC}::Vulkan)
  list(APPEND SOURCES WinSystemGbmVulkan.cpp)
  list(APPEND HEADERS WinSystemGbmVulkan.h)
endif()
if(TARGET ${APP_NAME_LC}::OpenGl)
  list(APPEND SOURCES WinSystemGbmEGLContext.cpp
                      WinSystemGbmGLContext.cpp)
  list(APPEND HEADERS WinSystemGbmEGLContext.h
                      WinSystemGbmGLContext.h)
endif()
if(TARGET ${APP_NAME_LC}::OpenGLES)
  list(APPEND SOURCES WinSystemGbmEGLContext.cpp
                      WinSystemGbmGLESContext.cpp)
  list(APPEND HEADERS WinSystemGbmEGLContext.h
                      WinSystemGbmGLESContext.h)
endif()

core_add_library(windowing_gbm)

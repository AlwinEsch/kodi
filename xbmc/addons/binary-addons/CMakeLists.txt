set(SOURCES BinaryAddonManager.cpp
            AddonDll.cpp
            BinaryAddonBase.cpp
            InstanceHandler.cpp)

set(HEADERS BinaryAddonManager.h
            AddonDll.h
            BinaryAddonBase.h
            DllAddon.h
            InstanceHandler.h)

core_add_library(addons_binary-addons)

set(SOURCES GameClient.cpp
            GameClientController.cpp
            GameClientInGameSaves.cpp
            GameClientInfo.cpp
            GameClientProperties.cpp
            GameClientSubsystem.cpp
            GameClientTranslator.cpp)

set(HEADERS GameClient.h
            GameClientCallbacks.h
            GameClientController.h
            GameClientInGameSaves.h
            GameClientInfo.h
            GameClientProperties.h
            GameClientSubsystem.h
            GameClientTranslator.h)

core_add_library(gameaddons)

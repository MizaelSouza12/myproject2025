# CMakeLists.txt principal para WYDBR 2.0
cmake_minimum_required(VERSION 3.15)
project(WYDBR2.0 VERSION 2.0.0 LANGUAGES CXX)

# Configurações
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opções de build
option(BUILD_CLIENT "Build WYDBR client" ON)
option(BUILD_SERVER "Build server components" ON)
option(BUILD_STUDIO "Build WYDStudio" ON)
option(BUILD_LAUNCHER "Build WYDLauncher" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" ON)

# Dependências
find_package(OpenSSL REQUIRED)
find_package(WebRTC REQUIRED)
find_package(DirectX REQUIRED)
find_package(FFmpeg REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)

# Incluir diretórios
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${OPENSSL_INCLUDE_DIR}
    ${WEBRTC_INCLUDE_DIR}
    ${DIRECTX_INCLUDE_DIR}
    ${FFMPEG_INCLUDE_DIR}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
)

# Adicionar subdiretórios
add_subdirectory(src)

# Configurar executáveis
if(BUILD_CLIENT)
    add_executable(WYDBR
        src/client/main.cpp
        src/client/ClientApp.cpp
        src/client/GameWindow.cpp
        src/client/InputManager.cpp
        src/client/Renderer.cpp
        src/client/ResourceManager.cpp
        src/client/NetworkManager.cpp
        src/client/AudioManager.cpp
        src/client/UIManager.cpp
        src/client/GameState.cpp
        src/client/PlayerManager.cpp
        src/client/WorldManager.cpp
        src/client/ItemManager.cpp
        src/client/QuestManager.cpp
        src/client/GuildManager.cpp
        src/client/TradeManager.cpp
        src/client/ChatManager.cpp
        src/client/PartyManager.cpp
        src/client/CombatManager.cpp
        src/client/InventoryManager.cpp
        src/client/CharacterManager.cpp
        src/client/SkillManager.cpp
        src/client/EffectManager.cpp
        src/client/WeatherManager.cpp
        src/client/TimeManager.cpp
        src/client/EventManager.cpp
        src/client/MiniMapManager.cpp
        src/client/HelpManager.cpp
        src/client/OptionManager.cpp
        src/client/KeySettingManager.cpp
        src/client/MacroManager.cpp
        src/client/FilterManager.cpp
        src/client/WhisperManager.cpp
        src/client/NoticeManager.cpp
        src/client/MailManager.cpp
        src/client/ShopManager.cpp
        src/client/StorageManager.cpp
        src/client/RepairManager.cpp
        src/client/ComposeManager.cpp
        src/client/RefineManager.cpp
        src/client/UpgradeManager.cpp
        src/client/QuestManager.cpp
        src/client/TeleportManager.cpp
        src/client/PartyManager.cpp
        src/client/GuildManager.cpp
        src/client/UnionManager.cpp
        src/client/TerritoryManager.cpp
        src/client/WarManager.cpp
        src/client/EventManager.cpp
        src/client/WeatherManager.cpp
        src/client/TimeManager.cpp
        src/client/MiniMapManager.cpp
        src/client/HelpManager.cpp
        src/client/OptionManager.cpp
        src/client/KeySettingManager.cpp
        src/client/MacroManager.cpp
        src/client/FilterManager.cpp
        src/client/WhisperManager.cpp
        src/client/NoticeManager.cpp
        src/client/MailManager.cpp
        src/client/ShopManager.cpp
        src/client/StorageManager.cpp
        src/client/RepairManager.cpp
        src/client/ComposeManager.cpp
        src/client/RefineManager.cpp
        src/client/UpgradeManager.cpp
    )
    
    target_link_libraries(WYDBR
        PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        WebRTC::Core
        DirectX::Core
        FFmpeg::Core
        Qt5::Core
        Qt5::Widgets
        Qt5::Network
    )
endif()

if(BUILD_LAUNCHER)
    add_executable(WYDLauncher
        src/launcher/main.cpp
        src/launcher/LauncherApp.cpp
        src/launcher/UpdateManager.cpp
        src/launcher/ConfigManager.cpp
        src/launcher/ServerManager.cpp
        src/launcher/UIManager.cpp
    )
    
    target_link_libraries(WYDLauncher
        PRIVATE
        Qt5::Core
        Qt5::Widgets
        Qt5::Network
    )
endif()

if(BUILD_STUDIO)
    add_executable(WYDStudio
        src/studio/main.cpp
        src/studio/StudioApp.cpp
        src/studio/AdminPanel.cpp
        src/studio/ServerManager.cpp
        src/studio/DatabaseManager.cpp
        src/studio/LogManager.cpp
        src/studio/ConfigManager.cpp
        src/studio/UIManager.cpp
        src/studio/PlayerManager.cpp
        src/studio/ItemManager.cpp
        src/studio/WorldManager.cpp
        src/studio/EventManager.cpp
        src/studio/BackupManager.cpp
        src/studio/MonitorManager.cpp
        src/studio/DebugManager.cpp
    )
    
    target_link_libraries(WYDStudio
        PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        Qt5::Core
        Qt5::Widgets
        Qt5::Network
    )
endif()

# Instalação
install(TARGETS WYDBR WYDLauncher WYDStudio
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY
    config/
    data/
    docs/
    DESTINATION share/wydbr
)

# Documentação
if(BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()

# Testes
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Mensagem de configuração
message(STATUS "WYDBR 2.0 Game Logic Library")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Tests: ${BUILD_TESTS}")
message(STATUS "Build Docs: ${BUILD_DOCS}")
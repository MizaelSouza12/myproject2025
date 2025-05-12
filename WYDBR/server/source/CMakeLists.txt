cmake_minimum_required(VERSION 3.10)
project(WYDBR_Server VERSION 1.0.0 LANGUAGES CXX)

# Definição da versão do C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configurações de build
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")

# Opções de compilação
option(WYDBR_ENABLE_LOGGING "Habilitar logs detalhados" ON)
option(WYDBR_ENABLE_SECURE_MODE "Habilitar modo seguro com validações adicionais" ON)
option(WYDBR_ENABLE_PROFILING "Habilitar ferramentas de profiling" OFF)
option(WYDBR_ENABLE_TESTS "Compilar testes unitários" OFF)
option(WYDBR_BUILD_TMSRV "Compilar TMSrv" ON)
option(WYDBR_BUILD_DBSRV "Compilar DBSrv" ON)
option(WYDBR_BUILD_MSGSRV "Compilar MSGSrv" ON)
option(WYDBR_BUILD_GATESRV "Compilar GateServer" ON)

# Diretórios
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Flags de compilação
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Verificar sistema operacional
if(WIN32)
    # Flags específicas do Windows
    add_definitions(-D_WIN32_WINNT=0x0601 -DWIN32_LEAN_AND_MEAN -DNOMINMAX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /MP")
elseif(UNIX AND NOT APPLE)
    # Flags específicas do Linux
    add_definitions(-D_GNU_SOURCE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

# Adicionar definições baseadas nas opções
if(WYDBR_ENABLE_LOGGING)
    add_definitions(-DWYDBR_ENABLE_LOGGING)
endif()

if(WYDBR_ENABLE_SECURE_MODE)
    add_definitions(-DWYDBR_ENABLE_SECURE_MODE)
endif()

if(WYDBR_ENABLE_PROFILING)
    add_definitions(-DWYDBR_ENABLE_PROFILING)
endif()

# Encontrar dependências
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(MySQL REQUIRED)
find_package(Threads REQUIRED)

# Incluir diretórios
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs
    ${OPENSSL_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    ${MYSQL_INCLUDE_DIR}
)

# Conjuntos de arquivos fonte comuns
set(COMMON_SOURCES
    src/common/logger.cpp
    src/common/config.cpp
    src/common/utils.cpp
    src/common/timer.cpp
    src/common/crypt.cpp
    src/common/compression.cpp
    src/common/random.cpp
)

set(NETWORK_SOURCES
    src/network/network_adapter.cpp
    src/network/socket_manager.cpp
    src/network/packet_handler.cpp
    src/network/packet_factory.cpp
    src/network/session_manager.cpp
)

set(DATABASE_SOURCES
    src/database/database_client.cpp
    src/database/query_builder.cpp
    src/database/connection_pool.cpp
    src/database/transaction_manager.cpp
)

set(CORE_SOURCES
    src/core/event_bus.cpp
    src/core/item_manager.cpp
    src/core/mob_manager.cpp
    src/core/skill_manager.cpp
    src/core/map_manager.cpp
    src/core/player_manager.cpp
    src/core/guild_manager.cpp
    src/core/trade_manager.cpp
    src/core/battle_manager.cpp
    src/core/quest_manager.cpp
)

set(GAME_SOURCES
    src/game/mob.cpp
    src/game/item.cpp
    src/game/skill.cpp
    src/game/player.cpp
    src/game/guild.cpp
    src/game/map.cpp
    src/game/npc.cpp
    src/game/shop.cpp
    src/game/battle.cpp
    src/game/quest.cpp
)

# TMSrv (Trade & Message Server)
if(WYDBR_BUILD_TMSRV)
    set(TMSRV_SOURCES
        src/tmsrv/main.cpp
        src/tmsrv/TMServer.cpp
        src/tmsrv/config/TMConfig.cpp
        src/tmsrv/player/WYDPlayer.cpp
        src/tmsrv/world/World.cpp
        src/tmsrv/item/ItemEffectManager.cpp
        ${COMMON_SOURCES}
        ${NETWORK_SOURCES}
        ${DATABASE_SOURCES}
        ${CORE_SOURCES}
        ${GAME_SOURCES}
    )

    add_executable(TMSrv ${TMSRV_SOURCES})
    target_link_libraries(TMSrv ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES} ${MYSQL_LIBRARIES} Threads::Threads)
    
    # Instalar TMSrv
    install(TARGETS TMSrv DESTINATION bin)
endif()

# DBSrv (Database Server)
if(WYDBR_BUILD_DBSRV)
    set(DBSRV_SOURCES
        src/dbsrv/main.cpp
        src/dbsrv/dbsrv.cpp
        src/dbsrv/account/account_manager.cpp
        src/dbsrv/character/character_manager.cpp
        src/dbsrv/storage/storage_manager.cpp
        src/dbsrv/guild/guild_data_manager.cpp
        src/dbsrv/system/system_manager.cpp
        ${COMMON_SOURCES}
        ${NETWORK_SOURCES}
        ${DATABASE_SOURCES}
    )

    add_executable(DBSrv ${DBSRV_SOURCES})
    target_link_libraries(DBSrv ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES} ${MYSQL_LIBRARIES} Threads::Threads)
    
    # Instalar DBSrv
    install(TARGETS DBSrv DESTINATION bin)
endif()

# MSGSrv (Login Server)
if(WYDBR_BUILD_MSGSRV)
    set(MSGSRV_SOURCES
        src/msgsrv/main.cpp
        src/msgsrv/msgsrv.cpp
        src/msgsrv/login/login_manager.cpp
        src/msgsrv/login/auth_system.cpp
        src/msgsrv/character/character_selector.cpp
        src/msgsrv/channel/channel_manager.cpp
        ${COMMON_SOURCES}
        ${NETWORK_SOURCES}
        ${DATABASE_SOURCES}
    )

    add_executable(MSGSrv ${MSGSRV_SOURCES})
    target_link_libraries(MSGSrv ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES} ${MYSQL_LIBRARIES} Threads::Threads)
    
    # Instalar MSGSrv
    install(TARGETS MSGSrv DESTINATION bin)
endif()

# GateServer (Connection Server)
if(WYDBR_BUILD_GATESRV)
    set(GATESRV_SOURCES
        src/gatesrv/main.cpp
        src/gatesrv/gatesrv.cpp
        src/gatesrv/connection/connection_router.cpp
        src/gatesrv/connection/packet_router.cpp
        src/gatesrv/connection/session_handler.cpp
        src/gatesrv/security/security_manager.cpp
        ${COMMON_SOURCES}
        ${NETWORK_SOURCES}
    )

    add_executable(GateServer ${GATESRV_SOURCES})
    target_link_libraries(GateServer ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES} Threads::Threads)
    
    # Instalar GateServer
    install(TARGETS GateServer DESTINATION bin)
endif()

# Instalar arquivos de configuração
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/config/ DESTINATION ../config)

# Instalar scripts
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts/ 
        DESTINATION ../scripts
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# Regras de construção específicas
set_target_properties(TMSrv PROPERTIES OUTPUT_NAME "TMSrv")
if(WYDBR_BUILD_DBSRV)
    set_target_properties(DBSrv PROPERTIES OUTPUT_NAME "DBSrv")
endif()
if(WYDBR_BUILD_MSGSRV)
    set_target_properties(MSGSrv PROPERTIES OUTPUT_NAME "MSGSrv")
endif()
if(WYDBR_BUILD_GATESRV)
    set_target_properties(GateServer PROPERTIES OUTPUT_NAME "GateServer")
endif()

# Adicionar testes se habilitado
if(WYDBR_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Imprimir configuração
message(STATUS "==== WYDBR Server Build Configuration ====")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Logging: ${WYDBR_ENABLE_LOGGING}")
message(STATUS "Secure mode: ${WYDBR_ENABLE_SECURE_MODE}")
message(STATUS "Profiling: ${WYDBR_ENABLE_PROFILING}")
message(STATUS "Tests: ${WYDBR_ENABLE_TESTS}")
message(STATUS "Building TMSrv: ${WYDBR_BUILD_TMSRV}")
message(STATUS "Building DBSrv: ${WYDBR_BUILD_DBSRV}")
message(STATUS "Building MSGSrv: ${WYDBR_BUILD_MSGSRV}")
message(STATUS "Building GateServer: ${WYDBR_BUILD_GATESRV}")
message(STATUS "==========================================")
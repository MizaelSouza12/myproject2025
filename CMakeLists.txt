# CMakeLists.txt principal para WYDBR 2.0
cmake_minimum_required(VERSION 3.14)
project(WYDBR2 VERSION 2.0.0 LANGUAGES CXX)

# Configurações globais
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opções de compilação
option(WYDBR_BUILD_TESTS "Compilar testes" OFF)
option(WYDBR_ENABLE_VOIP "Habilitar suporte a VoIP" ON)
option(WYDBR_ENABLE_SCREEN_SHARE "Habilitar suporte a compartilhamento de tela" ON)

# Definições globais
add_definitions(-DWYDBR_VERSION="${PROJECT_VERSION}")

# Verificar sistema operacional para flags específicas
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
elseif(UNIX)
    add_definitions(-D_POSIX_C_SOURCE=200809L)
endif()

# Bibliotecas externas
find_package(Threads REQUIRED)

# Diretórios de cabeçalho
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Common
)

# Verificar dependências para VoIP
if(WYDBR_ENABLE_VOIP)
    # Adicionar dependências de áudio aqui
    # Por exemplo: PortAudio, OpenAL, etc.
    add_definitions(-DWYDBR_ENABLE_VOIP)
endif()

# Verificar dependências para compartilhamento de tela
if(WYDBR_ENABLE_SCREEN_SHARE)
    # Adicionar dependências de vídeo aqui
    # Por exemplo: FFmpeg, libvpx, etc.
    add_definitions(-DWYDBR_ENABLE_SCREEN_SHARE)
endif()

# Componentes comuns
add_subdirectory(src/common)

# TMSrv
add_subdirectory(src/tmsrv)

# DBSrv
add_subdirectory(src/dbsrv)

# WYDStudio (inclui TMSrv e DBSrv)
add_subdirectory(src/wydstudio)

# VoIP (opcional)
if(WYDBR_ENABLE_VOIP)
    add_subdirectory(src/voip)
endif()

# Screen Share (opcional)
if(WYDBR_ENABLE_SCREEN_SHARE)
    add_subdirectory(src/screenshare)
endif()

# Cliente
add_subdirectory(src/client)

# Testes (opcional)
if(WYDBR_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)

# Fase 7 - Módulos adicionais
add_subdirectory(src/tmsrv/auth)
add_subdirectory(src/tmsrv/economy)
add_subdirectory(src/tmsrv/movement)
add_subdirectory(src/security/crypto)
add_subdirectory(src/integration)
endif()

# Configurações de instalação
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Resumo de configuração
message(STATUS "========== WYDBR 2.0 Configuration ==========")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "VoIP support: ${WYDBR_ENABLE_VOIP}")
message(STATUS "Screen Share support: ${WYDBR_ENABLE_SCREEN_SHARE}")
message(STATUS "Tests: ${WYDBR_BUILD_TESTS}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "==============================================")
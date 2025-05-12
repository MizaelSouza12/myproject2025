cmake_minimum_required(VERSION 3.10)
project(WYDBR VERSION 1.0.0 LANGUAGES CXX)

# Configurações globais de compilação
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opções de build
option(BUILD_TESTS "Build test executables" ON)
option(BUILD_SERVER "Build server executables" ON)

# Configurações para debug/release
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Diretórios de inclusão global
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/server/source/include
)

# Adiciona diretórios com o código-fonte
if(BUILD_SERVER)
    add_subdirectory(server/source/src)
endif()

# Adiciona diretórios com testes
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(server/source/tests)
endif()
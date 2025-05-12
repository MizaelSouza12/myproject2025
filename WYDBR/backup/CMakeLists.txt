cmake_minimum_required(VERSION 3.10)

# Definição do projeto e linguagem
project(WYD_SERVER VERSION 1.0.0 LANGUAGES CXX)

# Definição do standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opções de compilação
option(WYD_BUILD_TESTS "Build test programs" OFF)
option(WYD_ENABLE_DEBUG "Enable debug mode" ON)
option(WYD_USE_ENCRYPTION "Enable network encryption" ON)

# Diretórios com headers
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/headers)

# Definições
if(WYD_ENABLE_DEBUG)
    add_definitions(-DWYD_DEBUG=1)
endif()

if(WYD_USE_ENCRYPTION)
    add_definitions(-DWYD_ENCRYPTION=1)
endif()

# Busca por todas as fontes do servidor
file(GLOB_RECURSE SERVER_SOURCES 
    "${CMAKE_SOURCE_DIR}/src/server/*.cpp"
)

# Bibliotecas externas necessárias
find_package(Threads REQUIRED)

# Definição do executável TMSrv
add_executable(tmsrv 
    src/server/tmsrv/main.cpp
    ${SERVER_SOURCES}
)

# Vincula as bibliotecas
target_link_libraries(tmsrv
    PRIVATE
    Threads::Threads
)

# Configurações específicas para cada plataforma
if(WIN32)
    target_link_libraries(tmsrv PRIVATE wsock32 ws2_32)
    target_compile_definitions(tmsrv PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
endif()

# Definição do executável DBSrv (quando tivermos fontes suficientes)
# add_executable(dbsrv 
#     src/server/dbsrv/main.cpp
#     ${SERVER_SOURCES}
# )

# Configurações para testes
if(WYD_BUILD_TESTS)
    add_subdirectory(tests)
endif()
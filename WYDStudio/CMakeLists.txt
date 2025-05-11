cmake_minimum_required(VERSION 3.12)
project(WYDStudio VERSION 1.0.0 LANGUAGES CXX)

# Configuração do C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opções de compilação
option(WYDSTUDIO_BUILD_TESTS "Compilar testes" OFF)
option(WYDSTUDIO_USE_QT "Usar Qt para interface gráfica" ON)
option(WYDSTUDIO_USE_IMGUI "Usar ImGui para interface gráfica" OFF)

# Definição de variáveis para caminhos de código-fonte
set(WYDSTUDIO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(WYDSTUDIO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(WYDSTUDIO_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(WYDSTUDIO_MODELS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/models)

# Encontrar dependências
find_package(nlohmann_json REQUIRED)

# Configuração da GUI
if(WYDSTUDIO_USE_QT)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)
    add_definitions(-DWYDSTUDIO_USE_QT)
elseif(WYDSTUDIO_USE_IMGUI)
    # Configuração do ImGui
    find_package(OpenGL REQUIRED)
    find_package(GLFW3 REQUIRED)
    add_definitions(-DWYDSTUDIO_USE_IMGUI)
endif()

# Listar arquivos de código-fonte
file(GLOB_RECURSE WYDSTUDIO_SOURCES 
    ${WYDSTUDIO_SRC_DIR}/*.cpp
)

file(GLOB_RECURSE WYDSTUDIO_HEADERS 
    ${WYDSTUDIO_INCLUDE_DIR}/*.h
    ${WYDSTUDIO_SRC_DIR}/*.h
)

# Criar executável
add_executable(${PROJECT_NAME} ${WYDSTUDIO_SOURCES} ${WYDSTUDIO_HEADERS})

# Incluir diretórios
target_include_directories(${PROJECT_NAME} PRIVATE
    ${WYDSTUDIO_INCLUDE_DIR}
    ${WYDSTUDIO_SRC_DIR}
)

# Vincular bibliotecas
target_link_libraries(${PROJECT_NAME} PRIVATE
    nlohmann_json::nlohmann_json
)

# Vincular bibliotecas de GUI
if(WYDSTUDIO_USE_QT)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Core
        Qt5::Widgets
        Qt5::Network
    )
elseif(WYDSTUDIO_USE_IMGUI)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenGL::GL
        glfw
    )
endif()

# Configuração de instalação
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY ${WYDSTUDIO_RESOURCES_DIR} DESTINATION share/${PROJECT_NAME})
install(DIRECTORY ${WYDSTUDIO_MODELS_DIR} DESTINATION share/${PROJECT_NAME})

# Configuração para geração de documentação
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Gerando documentação API com Doxygen"
        VERBATIM
    )
endif()

# Configuração para testes
if(WYDSTUDIO_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Configurações específicas de plataforma
if(WIN32)
    # Configurações específicas para Windows
    target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    # Ícone e recursos do Windows
    set(WYDSTUDIO_RC_FILE ${WYDSTUDIO_RESOURCES_DIR}/WYDStudio.rc)
    target_sources(${PROJECT_NAME} PRIVATE ${WYDSTUDIO_RC_FILE})
elseif(UNIX AND NOT APPLE)
    # Configurações específicas para Linux
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread dl)
elseif(APPLE)
    # Configurações específicas para macOS
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${WYDSTUDIO_RESOURCES_DIR}/Info.plist
    )
endif()

# Exibir resumo da configuração
message(STATUS "WYDStudio configurado com:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Interface gráfica: ${WYDSTUDIO_USE_QT ? 'Qt' : (WYDSTUDIO_USE_IMGUI ? 'ImGui' : 'Nenhuma')}")
message(STATUS "  Compilando testes: ${WYDSTUDIO_BUILD_TESTS}")
message(STATUS "  Caminho de recursos: ${WYDSTUDIO_RESOURCES_DIR}")
message(STATUS "  Caminho de modelos: ${WYDSTUDIO_MODELS_DIR}")
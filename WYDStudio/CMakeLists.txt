cmake_minimum_required(VERSION 3.16)
project(WYDStudio VERSION 1.0.0 LANGUAGES CXX)

# Configuração do C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opções de compilação
option(WYDSTUDIO_BUILD_TESTS "Compilar testes" OFF)
option(WYDSTUDIO_ENABLE_AI "Habilitar funcionalidades de IA" ON)

# Configuração do Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Encontrar o Qt
find_package(Qt6 COMPONENTS
    Core
    Gui
    Widgets
    Network
    Sql
    Charts
    REQUIRED
)

# Encontrar outras dependências
find_package(OpenSSL REQUIRED)
find_package(MySQL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

# Definir diretórios de código-fonte
set(WYDSTUDIO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(WYDSTUDIO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(WYDSTUDIO_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)

# Coletar arquivos fonte
file(GLOB_RECURSE WYDSTUDIO_SOURCES
    ${WYDSTUDIO_SRC_DIR}/*.cpp
    ${WYDSTUDIO_SRC_DIR}/*.h
)

# Recursos do Qt
set(WYDSTUDIO_RESOURCES
    ${WYDSTUDIO_RESOURCES_DIR}/icons.qrc
    ${WYDSTUDIO_RESOURCES_DIR}/styles.qrc
)

# Criar executável
add_executable(${PROJECT_NAME}
    ${WYDSTUDIO_SOURCES}
    ${WYDSTUDIO_RESOURCES}
)

# Incluir diretórios
target_include_directories(${PROJECT_NAME} PRIVATE
    ${WYDSTUDIO_SRC_DIR}
    ${WYDSTUDIO_INCLUDE_DIR}
    ${MySQL_INCLUDE_DIR}
)

# Vincular bibliotecas
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Sql
    Qt6::Charts
    OpenSSL::SSL
    OpenSSL::Crypto
    MySQL::MySQL
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    fmt::fmt
)

# Definições de compilação
target_compile_definitions(${PROJECT_NAME} PRIVATE
    QT_NO_KEYWORDS
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG
)

if(WYDSTUDIO_ENABLE_AI)
    target_compile_definitions(${PROJECT_NAME} PRIVATE WYDSTUDIO_ENABLE_AI)
endif()

# Configurações específicas de plataforma
if(WIN32)
    # Ícone do Windows
    set(APP_ICON_RESOURCE_WINDOWS "${WYDSTUDIO_RESOURCES_DIR}/wydstudio.rc")
    target_sources(${PROJECT_NAME} PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
    
    # Definições do Windows
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
endif()

# Instalação
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

install(DIRECTORY ${WYDSTUDIO_RESOURCES_DIR}/
    DESTINATION share/${PROJECT_NAME}
    FILES_MATCHING PATTERN "*.qss"
)

install(FILES ${WYDSTUDIO_RESOURCES_DIR}/config/default.json
    DESTINATION share/${PROJECT_NAME}/config
)

# Testes
if(WYDSTUDIO_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Configuração do pacote
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "WYDBR Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "WYDStudio - Ferramenta Avançada de Administração WYD")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_NSIS_DISPLAY_NAME ${PROJECT_NAME})
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_MUI_ICON "${WYDSTUDIO_RESOURCES_DIR}/wydstudio.ico")
    set(CPACK_NSIS_MUI_UNIICON "${WYDSTUDIO_RESOURCES_DIR}/wydstudio.ico")
else()
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "WYDBR Team <contato@wydbr.com>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "games")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6, libqt6gui6, libqt6widgets6, libqt6network6, libqt6sql6, libqt6charts6")
endif()

include(CPack)
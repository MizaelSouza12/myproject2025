# CMakeLists.txt para o servidor WYDBR 2.0
cmake_minimum_required(VERSION 3.12)

# Definir arquivos fonte do servidor
set(SERVER_SOURCES
    ../src/server/main.cpp
    ../src/network/NetworkManager.cpp
    ../src/network/CryptoManager.cpp
    ../src/network/PacketHandler.cpp
    ../src/Security/SecurityManager.cpp
)

# Adicionar definições específicas para o servidor
add_definitions(-DSERVER_BUILD)

# Verificar sistema operacional para dependências específicas
if(WIN32)
    # Windows - adicionar bibliotecas de socket
    set(PLATFORM_LIBS ws2_32)
else()
    # Linux/Unix - bibliotecas de thread e socket
    set(PLATFORM_LIBS pthread)
    
    # Verificar se precisa de link explícito com rt (real-time) ou dl (dynamic loading)
    include(CheckLibraryExists)
    check_library_exists(rt clock_gettime "" NEED_RT)
    if(NEED_RT)
        list(APPEND PLATFORM_LIBS rt)
    endif()
    
    check_library_exists(dl dlopen "" NEED_DL)
    if(NEED_DL)
        list(APPEND PLATFORM_LIBS dl)
    endif()
endif()

# Incluir diretórios de header
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../headers
)

# Criar executável do servidor
add_executable(TMSrv ${SERVER_SOURCES})

# Vincular bibliotecas
target_link_libraries(TMSrv PRIVATE ${PLATFORM_LIBS})

# Configurar flags do compilador para segurança
if(MSVC)
    # Flags para Visual Studio
    target_compile_options(TMSrv PRIVATE 
        /W4 /WX /GS /DYNAMICBASE /NXCOMPAT /D_CRT_SECURE_NO_WARNINGS
    )
else()
    # Flags para GCC/Clang
    target_compile_options(TMSrv PRIVATE 
        -Wall -Wextra -Wpedantic -Werror -fstack-protector-strong -D_FORTIFY_SOURCE=2
        -Wformat -Wformat-security -fPIE
    )
    
    if(NOT APPLE)
        # Excluir estas flags no macOS, que pode não suportá-las
        target_link_options(TMSrv PRIVATE 
            -z relro -z now
        )
    endif()
endif()

# Instalar
install(TARGETS TMSrv DESTINATION bin)
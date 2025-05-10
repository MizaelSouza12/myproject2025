# CMakeLists.txt para o diretório src
cmake_minimum_required(VERSION 3.12)

# Adicionar subdiretórios
add_subdirectory(common)

if(WYDBR_BUILD_SERVER)
    add_subdirectory(server)
endif()

if(WYDBR_BUILD_STUDIO)
    add_subdirectory(admin)
endif()

if(WYDBR_BUILD_CLIENT)
    add_subdirectory(client)
endif()

if(WYDBR_BUILD_CONFIG)
    add_subdirectory(config)
endif()

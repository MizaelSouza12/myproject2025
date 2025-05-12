# CMakeLists.txt para código-fonte
cmake_minimum_required(VERSION 3.10)

# Compila o código-fonte como uma biblioteca
add_library(WYDBRCore
    database/DatabaseManager.cpp
    # Adicione outros arquivos .cpp aqui conforme necessário
)

# Configurações para a biblioteca
target_include_directories(WYDBRCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Vincula bibliotecas externas
target_link_libraries(WYDBRCore
    pthread
    stdc++fs
)

# Configura opções de compilação
set_target_properties(WYDBRCore PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
# CMakeLists.txt para os testes de banco de dados
cmake_minimum_required(VERSION 3.10)

# Adiciona o executável de teste
add_executable(DatabaseManagerTest
    DatabaseManagerTest.cpp
    ../../src/database/DatabaseManager.cpp
)

# Adiciona diretórios de inclusão
target_include_directories(DatabaseManagerTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
)

# Vincula bibliotecas necessárias
target_link_libraries(DatabaseManagerTest
    pthread
    stdc++fs
)

# Configura opções de compilação
set_target_properties(DatabaseManagerTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Adiciona definições
target_compile_definitions(DatabaseManagerTest PRIVATE
    _DEBUG
    _TEST
)
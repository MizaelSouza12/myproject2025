cmake_minimum_required(VERSION 3.10)
project(WYDBRCompressionTest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Diretórios de inclusão
include_directories(${CMAKE_SOURCE_DIR}/../../source/include)

# Arquivos de origem
set(SOURCES
    compression_error_test.cpp
    ${CMAKE_SOURCE_DIR}/../../source/src/compression/error-handler.cpp
)

# Executável do teste
add_executable(compression_error_test ${SOURCES})

# Exibir mensagem no final
add_custom_command(TARGET compression_error_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Build concluído. Execute ./compression_error_test para rodar os testes."
)
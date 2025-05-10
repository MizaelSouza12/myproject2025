# CMakeLists.txt para testes
enable_testing()

# Find GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Testes de integração
add_subdirectory(integration)

# Testes de segurança
add_subdirectory(security)

# Testes de rede
add_subdirectory(network)

# Testes de armazenamento
add_subdirectory(storage)

# Testes de lógica de jogo
add_subdirectory(gamelogic)

# Teste principal que executa todos os testes
add_executable(all_tests main_test.cpp)
target_link_libraries(all_tests
    integration_lib
    security_lib
    network_lib
    storage_lib
    gamelogic_lib
    common_lib
    ${GTEST_BOTH_LIBRARIES}
    Threads::Threads
)

add_test(NAME all_tests COMMAND all_tests)

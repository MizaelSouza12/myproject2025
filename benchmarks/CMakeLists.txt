# CMakeLists.txt para benchmarks

# Find Google Benchmark
find_package(benchmark REQUIRED)

# Benchmark para desempenho da rede
add_executable(network_benchmark network_benchmark.cpp)
target_link_libraries(network_benchmark
    network_lib
    security_lib
    integration_lib
    common_lib
    benchmark::benchmark
    Threads::Threads
)

# Benchmark para desempenho da segurança
add_executable(security_benchmark security_benchmark.cpp)
target_link_libraries(security_benchmark
    security_lib
    common_lib
    benchmark::benchmark
    Threads::Threads
)

# Benchmark para lógica de jogo
add_executable(gamelogic_benchmark gamelogic_benchmark.cpp)
target_link_libraries(gamelogic_benchmark
    gamelogic_lib
    common_lib
    benchmark::benchmark
    Threads::Threads
)

# Benchmark para carga do sistema
add_executable(system_load_benchmark system_load_benchmark.cpp)
target_link_libraries(system_load_benchmark
    integration_lib
    security_lib
    network_lib
    gamelogic_lib
    common_lib
    benchmark::benchmark
    Threads::Threads
)

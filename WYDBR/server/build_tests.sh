#!/bin/bash
# Script para compilar testes unitários, testes de integração e benchmarks

# Definir diretórios
TESTS_DIR="./tests"
INTEGRATION_DIR="./integration"
BENCHMARKS_DIR="./benchmarks"
INCLUDE_DIR="./source/include"
SRC_DIR="./source/src"
BUILD_DIR="./build"

# Criar diretório de build se não existir
mkdir -p "$BUILD_DIR"
mkdir -p "$BUILD_DIR/tests"
mkdir -p "$BUILD_DIR/integration"
mkdir -p "$BUILD_DIR/benchmarks"

# Compilar testes unitários
echo "Compilando testes unitários..."
for test_file in $(find "$TESTS_DIR" -name "*_test.cpp"); do
    basename=$(basename "$test_file" .cpp)
    echo "  Compilando $basename..."
    g++ -std=c++17 -Wall -Wextra -I"$INCLUDE_DIR" "$test_file" -o "$BUILD_DIR/tests/$basename"
done

# Compilar testes de integração
echo "Compilando testes de integração..."
for test_file in $(find "$INTEGRATION_DIR" -name "*_integration_test.cpp"); do
    basename=$(basename "$test_file" .cpp)
    echo "  Compilando $basename..."
    g++ -std=c++17 -Wall -Wextra -I"$INCLUDE_DIR" "$test_file" -o "$BUILD_DIR/integration/$basename"
done

# Compilar benchmarks
echo "Compilando benchmarks..."
for benchmark_file in $(find "$BENCHMARKS_DIR" -name "*_benchmark.cpp"); do
    basename=$(basename "$benchmark_file" .cpp)
    echo "  Compilando $basename..."
    g++ -std=c++17 -Wall -Wextra -O3 -I"$INCLUDE_DIR" "$benchmark_file" -o "$BUILD_DIR/benchmarks/$basename"
done

echo "Compilação concluída!"
echo "Os executáveis estão disponíveis em:"
echo "  - Testes unitários: $BUILD_DIR/tests/"
echo "  - Testes de integração: $BUILD_DIR/integration/"
echo "  - Benchmarks: $BUILD_DIR/benchmarks/"

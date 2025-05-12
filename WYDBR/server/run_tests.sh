#!/bin/bash
# Script para executar testes unitários, testes de integração e benchmarks

# Definir diretórios
BUILD_DIR="./build"
RESULTS_DIR="./results"

# Criar diretório de resultados se não existir
mkdir -p "$RESULTS_DIR"

# Função para registrar o resultado
log_result() {
    test_name=$1
    result=$2
    timestamp=$(date +"%Y-%m-%d %H:%M:%S")
    
    echo "$timestamp | $test_name | $result" >> "$RESULTS_DIR/test_results.log"
    
    if [ "$result" == "PASSED" ]; then
        echo -e "\e[32m✓ $test_name: $result\e[0m"
    else
        echo -e "\e[31m✗ $test_name: $result\e[0m"
    fi
}

# Executar testes unitários
echo "Executando testes unitários..."
for test_exec in $(find "$BUILD_DIR/tests" -type f -executable); do
    test_name=$(basename "$test_exec")
    echo "  Executando $test_name..."
    
    "$test_exec" > "$RESULTS_DIR/${test_name}.log" 2>&1
    
    if [ $? -eq 0 ]; then
        log_result "$test_name" "PASSED"
    else
        log_result "$test_name" "FAILED"
    fi
done

# Executar testes de integração
echo "Executando testes de integração..."
for test_exec in $(find "$BUILD_DIR/integration" -type f -executable); do
    test_name=$(basename "$test_exec")
    echo "  Executando $test_name..."
    
    "$test_exec" > "$RESULTS_DIR/${test_name}.log" 2>&1
    
    if [ $? -eq 0 ]; then
        log_result "$test_name" "PASSED"
    else
        log_result "$test_name" "FAILED"
    fi
done

# Executar benchmarks
echo "Executando benchmarks..."
for benchmark_exec in $(find "$BUILD_DIR/benchmarks" -type f -executable); do
    benchmark_name=$(basename "$benchmark_exec")
    echo "  Executando $benchmark_name..."
    
    "$benchmark_exec" > "$RESULTS_DIR/${benchmark_name}.log" 2>&1
    
    if [ $? -eq 0 ]; then
        echo -e "\e[32m✓ $benchmark_name: Completado\e[0m"
    else
        echo -e "\e[31m✗ $benchmark_name: Falha\e[0m"
    fi
done

# Gerar relatório de resultados
report_file="$RESULTS_DIR/summary_report_$(date +"%Y%m%d_%H%M%S").md"

echo "# Relatório de Testes e Benchmarks" > "$report_file"
echo "Data: $(date)" >> "$report_file"
echo "" >> "$report_file"

echo "## Resumo" >> "$report_file"
total_tests=$(find "$BUILD_DIR/tests" "$BUILD_DIR/integration" -type f -executable | wc -l)
passed_tests=$(grep "PASSED" "$RESULTS_DIR/test_results.log" | wc -l)
failed_tests=$(grep "FAILED" "$RESULTS_DIR/test_results.log" | wc -l)

echo "- Total de testes: $total_tests" >> "$report_file"
echo "- Testes passados: $passed_tests" >> "$report_file"
echo "- Testes falhos: $failed_tests" >> "$report_file"
echo "- Taxa de sucesso: $(echo "scale=2; $passed_tests * 100 / $total_tests" | bc)%" >> "$report_file"
echo "" >> "$report_file"

echo "## Detalhes dos Testes" >> "$report_file"
echo "### Testes Unitários" >> "$report_file"
for test_exec in $(find "$BUILD_DIR/tests" -type f -executable); do
    test_name=$(basename "$test_exec")
    result=$(grep "$test_name" "$RESULTS_DIR/test_results.log" | cut -d'|' -f3)
    echo "- $test_name: $result" >> "$report_file"
done
echo "" >> "$report_file"

echo "### Testes de Integração" >> "$report_file"
for test_exec in $(find "$BUILD_DIR/integration" -type f -executable); do
    test_name=$(basename "$test_exec")
    result=$(grep "$test_name" "$RESULTS_DIR/test_results.log" | cut -d'|' -f3)
    echo "- $test_name: $result" >> "$report_file"
done
echo "" >> "$report_file"

echo "### Benchmarks" >> "$report_file"
for benchmark_exec in $(find "$BUILD_DIR/benchmarks" -type f -executable); do
    benchmark_name=$(basename "$benchmark_exec")
    echo "- $benchmark_name: Completado" >> "$report_file"
done
echo "" >> "$report_file"

echo "Relatório gerado: $report_file"

echo "Execução de testes e benchmarks concluída!"

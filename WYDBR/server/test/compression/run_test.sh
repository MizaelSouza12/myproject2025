#!/bin/bash

echo "===== Compilando teste do sistema de exceções de compressão ====="

# Navegar para o diretório de testes
cd "$(dirname "$0")"

# Definir variáveis
INCLUDE_DIR="../../source/include"
SOURCE_DIR="../../source/src/compression"
TEST_FILE="compression_error_test.cpp"
OUTPUT_BIN="compression_error_test"

# Criar o diretório source/src/compression se não existir
mkdir -p ../../source/src/compression

# Verificar se os arquivos de cabeçalho existem
if [ ! -f "$INCLUDE_DIR/compression/compression-exceptions.h" ] || [ ! -f "$INCLUDE_DIR/compression/error-handler.h" ]; then
    echo "ERRO: Arquivos de cabeçalho necessários não encontrados!"
    exit 1
fi

# Verificar se os arquivos de implementação existem
if [ ! -f "$SOURCE_DIR/error-handler.cpp" ]; then
    echo "ERRO: Arquivo $SOURCE_DIR/error-handler.cpp não encontrado!"
    exit 1
fi

# Verificar se o arquivo de teste existe
if [ ! -f "$TEST_FILE" ]; then
    echo "ERRO: Arquivo de teste não encontrado!"
    exit 1
fi

# Compilar o teste
echo "Compilando..."
g++ -std=c++11 -I"$INCLUDE_DIR" "$TEST_FILE" "$SOURCE_DIR/error-handler.cpp" -o "$OUTPUT_BIN"

if [ $? -ne 0 ]; then
    echo "ERRO: Compilação falhou!"
    exit 1
fi

# Executar o teste
echo -e "\n===== Executando teste ====="
chmod +x "./$OUTPUT_BIN"
"./$OUTPUT_BIN"

# Verificar resultado
if [ $? -eq 0 ]; then
    echo -e "\n===== Teste concluído com sucesso! ====="
    exit 0
else
    echo -e "\n===== Teste falhou! ====="
    exit 1
fi
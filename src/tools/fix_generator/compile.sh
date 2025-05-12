#!/bin/bash

# Verifica se o diretório de saída existe, senão cria
mkdir -p build

# Compila o gerador de correções
echo "Compilando gerador de correções WYDBR 2.0..."
g++ -std=c++17 main.cpp -o build/wydbr_fix_generator -pthread -lstdc++fs

if [ $? -eq 0 ]; then
    echo "Compilação concluída com sucesso!"
    echo "Execute o gerador com: ./build/wydbr_fix_generator [caminho_de_saída]"
else
    echo "Erro na compilação!"
    exit 1
fi 
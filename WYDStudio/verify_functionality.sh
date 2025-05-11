#!/bin/bash

# Script para verificar a funcionalidade do WYDStudio
# Este script testa a estrutura, compilação e funções principais

# Definir cores para saída
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${BLUE}===============================================${NC}"
echo -e "${BLUE}    WYD MODERN 2025 STUDIO - Verificação      ${NC}"
echo -e "${BLUE}===============================================${NC}"

# Função para mostrar status de verificação
check_status() {
    if [ $1 -eq 0 ]; then
        echo -e "[${GREEN}✓${NC}] $2"
    else
        echo -e "[${RED}✗${NC}] $2"
        all_passed=false
    fi
}

# Variável global para status 
all_passed=true

echo -e "\n${YELLOW}1. Verificando estrutura de diretórios...${NC}"

# Verificar diretórios essenciais
directories=(
    "./include"
    "./src"
    "./examples"
    "./docs"
    "./data"
    "./bin"
    "./build"
)

for dir in "${directories[@]}"; do
    if [ -d "$dir" ]; then
        check_status 0 "Diretório $dir existe"
    else
        check_status 1 "Diretório $dir não encontrado"
        mkdir -p "$dir"
        echo -e "   ${YELLOW}Diretório '$dir' criado${NC}"
    fi
done

echo -e "\n${YELLOW}2. Verificando arquivos essenciais...${NC}"

# Verificar arquivos importantes
files=(
    "./include/NaturalLanguageProcessor.h"
    "./include/AdvancedAdminAgent.h"
    "./include/LanguageTermsLoader.h"
    "./src/NaturalLanguageProcessor.cpp"
    "./src/AdvancedAdminAgent.cpp"
    "./src/LanguageTermsLoader.cpp"
    "./examples/AdvancedNLPAgentExample.cpp"
    "./examples/EnhancedLanguageExample.cpp"
    "./data/expanded_language_terms.json"
    "./Makefile"
    "./compile.sh"
)

for file in "${files[@]}"; do
    if [ -f "$file" ]; then
        check_status 0 "Arquivo $file existe"
    else
        check_status 1 "Arquivo $file não encontrado"
    fi
done

echo -e "\n${YELLOW}3. Verificando permissões de execução...${NC}"

# Verificar permissões de scripts
executable_files=(
    "./compile.sh"
)

for file in "${executable_files[@]}"; do
    if [ -x "$file" ]; then
        check_status 0 "Arquivo $file tem permissão de execução"
    else
        check_status 1 "Arquivo $file sem permissão de execução"
        chmod +x "$file"
        echo -e "   ${YELLOW}Permissão de execução adicionada a '$file'${NC}"
    fi
done

echo -e "\n${YELLOW}4. Verificando ferramentas de compilação...${NC}"

# Verificar ferramentas necessárias
tools=("g++" "make" "mkdir" "cp")

for tool in "${tools[@]}"; do
    if command -v $tool &> /dev/null; then
        check_status 0 "Ferramenta $tool disponível"
    else
        check_status 1 "Ferramenta $tool não encontrada"
    fi
done

echo -e "\n${YELLOW}5. Verificando dependências...${NC}"

# Verificar biblioteca JSON
json_header="./include/nlohmann/json.hpp"
if [ -f "$json_header" ]; then
    check_status 0 "Biblioteca JSON disponível"
else
    echo -e "   ${YELLOW}Biblioteca JSON não encontrada, baixando...${NC}"
    mkdir -p "./include/nlohmann"
    curl -s https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp -o "./include/nlohmann/json.hpp"
    
    if [ -f "$json_header" ]; then
        check_status 0 "Biblioteca JSON baixada com sucesso"
    else
        check_status 1 "Falha ao baixar biblioteca JSON"
    fi
fi

echo -e "\n${YELLOW}6. Verificando capacidade de edição...${NC}"

# Testar edição de arquivo
test_file="./test_edit_capability.tmp"
echo "Este é um arquivo de teste para verificar capacidade de edição." > "$test_file"

if [ -f "$test_file" ]; then
    check_status 0 "Capacidade de criação de arquivo OK"
    
    # Testar modificação
    echo "Esta linha foi adicionada para testar modificação." >> "$test_file"
    if grep -q "modificação" "$test_file"; then
        check_status 0 "Capacidade de modificação de arquivo OK"
    else
        check_status 1 "Falha na modificação de arquivo"
    fi
    
    # Limpar
    rm "$test_file"
    if [ ! -f "$test_file" ]; then
        check_status 0 "Capacidade de remoção de arquivo OK"
    else
        check_status 1 "Falha na remoção de arquivo"
    fi
else
    check_status 1 "Falha na criação de arquivo"
fi

echo -e "\n${YELLOW}7. Tentativa de compilação básica...${NC}"

# Criar um arquivo de teste simples
test_cpp="./test_compile.cpp"
cat > "$test_cpp" << EOL
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::cout << "Teste de compilação bem-sucedido!" << std::endl;
    
    // Testar funções básicas de STL
    std::vector<std::string> vec = {"Teste", "de", "funcionalidade", "STL"};
    
    for (const auto& str : vec) {
        std::cout << str << " ";
    }
    std::cout << std::endl;
    
    return 0;
}
EOL

# Tentar compilar
g++ -std=c++17 -o test_compile "$test_cpp" 2>/dev/null

if [ -x "./test_compile" ]; then
    check_status 0 "Compilação básica bem-sucedida"
    
    # Executar para verificar
    output=$(./test_compile)
    if [[ "$output" == *"Teste de compilação bem-sucedido!"* ]]; then
        check_status 0 "Execução do programa compilado OK"
    else
        check_status 1 "Falha na execução do programa compilado"
    fi
    
    # Limpar
    rm "./test_compile" "$test_cpp"
else
    check_status 1 "Falha na compilação básica"
    rm "$test_cpp"
fi

echo -e "\n${YELLOW}8. Verificando banco de dados de termos...${NC}"

# Verificar estrutura do JSON
if [ -f "./data/expanded_language_terms.json" ]; then
    output=$(grep -o '"terms"' "./data/expanded_language_terms.json" | wc -l)
    if [ "$output" -gt 0 ]; then
        check_status 0 "Estrutura de termos no JSON verificada"
        
        # Contar categorias
        cat_count=$(grep -o '"category"' "./data/expanded_language_terms.json" | wc -l)
        echo -e "   ${BLUE}→ $cat_count categorias encontradas no banco de termos${NC}"
    else
        check_status 1 "Estrutura de termos no JSON inválida"
    fi
else
    check_status 1 "Arquivo de banco de termos não encontrado"
fi

# Resultado final
echo -e "\n${BLUE}===============================================${NC}"
if [ "$all_passed" = true ]; then
    echo -e "${GREEN}Todas as verificações passaram com sucesso!${NC}"
    echo -e "${GREEN}O WYDStudio está configurado corretamente.${NC}"
else
    echo -e "${YELLOW}Algumas verificações falharam.${NC}"
    echo -e "${YELLOW}Verifique os erros acima e corrija-os antes de prosseguir.${NC}"
fi
echo -e "${BLUE}===============================================${NC}"

# Sugestões finais
echo -e "\n${BLUE}Próximos passos recomendados:${NC}"
echo -e "1. Execute ${YELLOW}./compile.sh${NC} para compilar o projeto"
echo -e "2. Execute ${YELLOW}./bin/EnhancedLanguageExample${NC} para testar a integração de linguagem"
echo -e "3. Consulte ${YELLOW}./docs/EXPANDED_LANGUAGE_INTEGRATION.md${NC} para mais informações"
echo ""
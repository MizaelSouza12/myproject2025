#!/bin/bash

# Script de compilação para o projeto WYDBR
# Esse script compila os diferentes componentes do projeto

# Cores para saída
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Diretórios principais
SRC_DIR="./server/source/src"
INCLUDE_DIR="./server/source/include"
BUILD_DIR="./build"
BIN_DIR="./bin"

# Parâmetros de compilação
CXX="g++"
CXXFLAGS="-std=c++17 -Wall -Wextra -I${INCLUDE_DIR}"
LDFLAGS="-pthread -lstdc++fs"

# Verifica se está executando em ambiente de debug
if [ "$1" = "debug" ]; then
    CXXFLAGS="$CXXFLAGS -g -DDEBUG"
    echo -e "${BLUE}Compilando em modo DEBUG${NC}"
else
    CXXFLAGS="$CXXFLAGS -O2 -DNDEBUG"
    echo -e "${BLUE}Compilando em modo RELEASE${NC}"
fi

# Cria diretórios necessários
mkdir -p ${BUILD_DIR}
mkdir -p ${BIN_DIR}

# Função para compilar um executável
compile_executable() {
    local name=$1
    local sources=$2
    local output=$3
    
    echo -e "${YELLOW}Compilando $name...${NC}"
    
    $CXX $CXXFLAGS $sources -o $output $LDFLAGS
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}$name compilado com sucesso: $output${NC}"
        return 0
    else
        echo -e "${RED}Erro ao compilar $name${NC}"
        return 1
    fi
}

# Função para compilar e executar testes
compile_and_run_tests() {
    local name=$1
    local sources=$2
    local output=$3
    
    compile_executable "$name" "$sources" "$output"
    
    if [ $? -eq 0 ]; then
        echo -e "${YELLOW}Executando $name...${NC}"
        $output
        
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}$name executado com sucesso${NC}"
            return 0
        else
            echo -e "${RED}Erro ao executar $name${NC}"
            return 1
        fi
    fi
    
    return 1
}

# Função para limpar a compilação
clean_build() {
    echo -e "${YELLOW}Limpando diretórios de build...${NC}"
    rm -rf ${BUILD_DIR}/*
    rm -rf ${BIN_DIR}/*
    echo -e "${GREEN}Limpeza concluída${NC}"
}

# Menu de opções
if [ "$1" = "clean" ]; then
    clean_build
    exit 0
fi

if [ "$1" = "test-db" ] || [ "$1" = "all" ]; then
    # Compila e executa o teste de banco de dados
    mkdir -p test_db_simple
    compile_and_run_tests "Teste de Banco de Dados" "${SRC_DIR}/database/minimal_test.cpp ${SRC_DIR}/database/DatabaseManager.cpp" "${BIN_DIR}/db_test"
fi

if [ "$1" = "test-player" ] || [ "$1" = "all" ]; then
    # Compila e executa o teste do gerenciador de jogadores
    mkdir -p player_data
    compile_and_run_tests "Teste do PlayerManager" "${SRC_DIR}/tmsrv/PlayerManager_test.cpp ${SRC_DIR}/tmsrv/PlayerManager.cpp" "${BIN_DIR}/player_test"
fi

if [ "$1" = "test-map" ] || [ "$1" = "all" ]; then
    # Compila e executa o teste do gerenciador de mapas
    mkdir -p maps
    compile_and_run_tests "Teste do MapManager" "${SRC_DIR}/world/MapManager.cpp" "${BIN_DIR}/map_test"
fi

if [ "$1" = "help" ] || [ "$#" -eq 0 ]; then
    echo "Uso: $0 [opção]"
    echo "Opções:"
    echo "  clean     - Limpa os diretórios de build"
    echo "  debug     - Compila em modo debug (com símbolos)"
    echo "  test-db   - Compila e executa o teste de banco de dados"
    echo "  test-player - Compila e executa o teste do gerenciador de jogadores"
    echo "  test-map  - Compila e executa o teste do gerenciador de mapas"
    echo "  all       - Compila tudo"
    echo "  help      - Exibe esta ajuda"
    exit 0
fi

echo -e "${GREEN}Compilação concluída${NC}"
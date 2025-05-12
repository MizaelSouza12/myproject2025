#!/bin/bash

# Cores para saída
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${BLUE}===============================================${NC}"
echo -e "${BLUE}    WYD MODERN 2025 STUDIO - Compilação       ${NC}"
echo -e "${BLUE}===============================================${NC}"

# Verificar se o compilador está disponível
if ! command -v g++ &> /dev/null; then
    echo -e "${RED}Erro: g++ não encontrado! Instale o compilador C++ antes de continuar.${NC}"
    exit 1
fi

# Verificar se make está disponível
if ! command -v make &> /dev/null; then
    echo -e "${RED}Erro: make não encontrado! Instale o utilitário make antes de continuar.${NC}"
    exit 1
fi

# Verificar parâmetros
BUILD_TYPE="release"
BUILD_EXAMPLES=true
RUN_TESTS=false
RUN_UI_TESTS=false

for arg in "$@"; do
    case $arg in
        --debug)
            BUILD_TYPE="debug"
            shift
            ;;
        --no-examples)
            BUILD_EXAMPLES=false
            shift
            ;;
        --test)
            RUN_TESTS=true
            shift
            ;;
        --ui-test)
            RUN_UI_TESTS=true
            shift
            ;;
        --help)
            echo -e "${YELLOW}Uso: ./compile.sh [opções]${NC}"
            echo "Opções:"
            echo "  --debug       Compilar em modo de depuração"
            echo "  --no-examples Não compilar exemplos"
            echo "  --test        Executar testes após compilação"
            echo "  --ui-test     Executar testes de UI após compilação"
            echo "  --help        Mostrar esta mensagem de ajuda"
            exit 0
            ;;
    esac
done

echo -e "${YELLOW}Configuração:${NC}"
echo -e "  Tipo de build: ${GREEN}$BUILD_TYPE${NC}"
echo -e "  Compilar exemplos: ${GREEN}$BUILD_EXAMPLES${NC}"
echo -e "  Executar testes: ${GREEN}$RUN_TESTS${NC}"
echo -e "  Executar testes de UI: ${GREEN}$RUN_UI_TESTS${NC}"
echo ""

# Preparar flags de compilação com base no tipo de build
MAKE_FLAGS=""
if [ "$BUILD_TYPE" = "debug" ]; then
    MAKE_FLAGS="CXXFLAGS='-std=c++17 -Wall -Wextra -pedantic -g -O0 -DDEBUG'"
fi

# Criar diretórios necessários
mkdir -p build bin

# Compilar a biblioteca principal
echo -e "${YELLOW}Compilando biblioteca WYDStudio...${NC}"
if [ "$BUILD_EXAMPLES" = true ]; then
    eval make $MAKE_FLAGS
else
    eval make $MAKE_FLAGS lib
fi

# Verificar status da compilação
if [ $? -ne 0 ]; then
    echo -e "${RED}Erro: Falha na compilação da biblioteca!${NC}"
    exit 1
else
    echo -e "${GREEN}Biblioteca compilada com sucesso!${NC}"
fi

# Compilar e executar testes de UI se solicitado
if [ "$RUN_UI_TESTS" = true ]; then
    echo -e "\n${YELLOW}Compilando e executando testes de UI...${NC}"
    cd ui_test
    make
    if [ $? -ne 0 ]; then
        echo -e "${RED}Erro: Falha na compilação dos testes de UI!${NC}"
        exit 1
    fi
    
    ./UITestApp
    if [ $? -ne 0 ]; then
        echo -e "${RED}Erro: Falha na execução dos testes de UI!${NC}"
        exit 1
    else
        echo -e "${GREEN}Testes de UI executados com sucesso!${NC}"
    fi
    cd ..
fi

# Executar testes se solicitado
if [ "$RUN_TESTS" = true ]; then
    echo -e "\n${YELLOW}Executando testes...${NC}"
    if ls bin/EnhancedLanguageExample* 1> /dev/null 2>&1; then
        bin/EnhancedLanguageExample
        if [ $? -ne 0 ]; then
            echo -e "${RED}Erro: Falha nos testes de processamento de linguagem!${NC}"
            exit 1
        else
            echo -e "${GREEN}Testes de processamento de linguagem concluídos com sucesso!${NC}"
        fi
    else
        echo -e "${RED}Aviso: Exemplo de processamento de linguagem não encontrado!${NC}"
    fi
    
    # Aqui pode adicionar mais testes conforme necessário
fi

echo -e "\n${BLUE}===============================================${NC}"
echo -e "${GREEN}Compilação concluída com sucesso!${NC}"

if [ "$BUILD_EXAMPLES" = true ]; then
    echo -e "${YELLOW}Executáveis de exemplo disponíveis no diretório 'bin'.${NC}"
fi

echo -e "${BLUE}===============================================${NC}"
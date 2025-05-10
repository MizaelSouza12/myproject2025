#!/bin/bash
# Script de compilação para Linux - WYDBR 2.0

# Cores
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}==============================================${NC}"
echo -e "${BLUE}      WYDBR 2.0 - Script de Compilação      ${NC}"
echo -e "${BLUE}==============================================${NC}"

# Verificar requisitos
echo -e "${YELLOW}Verificando requisitos...${NC}"

# Verificar CMake
if ! command -v cmake &> /dev/null
then
    echo -e "${RED}CMake não encontrado! Por favor, instale o CMake.${NC}"
    exit 1
fi
echo -e "${GREEN}✓ CMake encontrado${NC}"

# Verificar compilador C++
if ! command -v g++ &> /dev/null
then
    echo -e "${RED}G++ não encontrado! Por favor, instale o G++.${NC}"
    exit 1
fi
echo -e "${GREEN}✓ G++ encontrado${NC}"

# Verificar make
if ! command -v make &> /dev/null
then
    echo -e "${RED}Make não encontrado! Por favor, instale o Make.${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Make encontrado${NC}"

# Criar diretório de build se não existir
echo -e "${YELLOW}Criando diretório de build...${NC}"
mkdir -p build
cd build

# Configurar CMake
echo -e "${YELLOW}Configurando CMake...${NC}"
cmake .. -DCMAKE_BUILD_TYPE=Release

# Compilar
echo -e "${YELLOW}Compilando...${NC}"
make -j$(nproc)

# Verificar resultado da compilação
if [ $? -eq 0 ]; then
    echo -e "${GREEN}Compilação concluída com sucesso!${NC}"
    echo -e "${YELLOW}Os binários estão disponíveis em:${NC} ${BLUE}$(pwd)/bin${NC}"
else
    echo -e "${RED}Erro durante a compilação!${NC}"
    exit 1
fi

# Voltar para o diretório original
cd ..

echo -e "${GREEN}Processo concluído!${NC}"
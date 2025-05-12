#!/bin/bash
# Script para gerar documentação usando Doxygen

# Verificar se o Doxygen está instalado
if ! command -v doxygen &> /dev/null; then
    echo "Doxygen não está instalado. Por favor, instale o Doxygen primeiro."
    exit 1
fi

# Definir diretórios
DOCS_DIR="./docs"
SOURCE_DIR="./source"

# Criar diretório de documentação se não existir
mkdir -p "$DOCS_DIR"

# Criar arquivo de configuração do Doxygen
DOXYFILE="$DOCS_DIR/Doxyfile"

# Gerar Doxyfile padrão e customizar
doxygen -g "$DOXYFILE" > /dev/null

# Customizar configurações do Doxygen
sed -i 's/PROJECT_NAME.*=.*/PROJECT_NAME = "WYDBR - Sistemas Avançados"/' "$DOXYFILE"
sed -i 's/PROJECT_BRIEF.*=.*/PROJECT_BRIEF = "Documentação dos sistemas avançados do WYDBR"/' "$DOXYFILE"
sed -i 's/OUTPUT_DIRECTORY.*=.*/OUTPUT_DIRECTORY = "$DOCS_DIR"/' "$DOXYFILE"
sed -i 's/INPUT.*=.*/INPUT = "$SOURCE_DIR"/' "$DOXYFILE"
sed -i 's/RECURSIVE.*=.*/RECURSIVE = YES/' "$DOXYFILE"
sed -i 's/EXTRACT_ALL.*=.*/EXTRACT_ALL = YES/' "$DOXYFILE"
sed -i 's/EXTRACT_PRIVATE.*=.*/EXTRACT_PRIVATE = YES/' "$DOXYFILE"
sed -i 's/EXTRACT_STATIC.*=.*/EXTRACT_STATIC = YES/' "$DOXYFILE"
sed -i 's/HAVE_DOT.*=.*/HAVE_DOT = YES/' "$DOXYFILE"
sed -i 's/UML_LOOK.*=.*/UML_LOOK = YES/' "$DOXYFILE"
sed -i 's/CALL_GRAPH.*=.*/CALL_GRAPH = YES/' "$DOXYFILE"
sed -i 's/CALLER_GRAPH.*=.*/CALLER_GRAPH = YES/' "$DOXYFILE"
sed -i 's/DOT_IMAGE_FORMAT.*=.*/DOT_IMAGE_FORMAT = svg/' "$DOXYFILE"
sed -i 's/GENERATE_HTML.*=.*/GENERATE_HTML = YES/' "$DOXYFILE"
sed -i 's/GENERATE_LATEX.*=.*/GENERATE_LATEX = NO/' "$DOXYFILE"
sed -i 's/HTML_OUTPUT.*=.*/HTML_OUTPUT = html/' "$DOXYFILE"
sed -i 's/JAVADOC_AUTOBRIEF.*=.*/JAVADOC_AUTOBRIEF = YES/' "$DOXYFILE"
sed -i 's/SORT_MEMBER_DOCS.*=.*/SORT_MEMBER_DOCS = YES/' "$DOXYFILE"
sed -i 's/GENERATE_TREEVIEW.*=.*/GENERATE_TREEVIEW = YES/' "$DOXYFILE"

# Gerar documentação
echo "Gerando documentação..."
doxygen "$DOXYFILE"

echo "Documentação gerada em $DOCS_DIR/html/"
echo "Abra $DOCS_DIR/html/index.html no navegador para visualizar."

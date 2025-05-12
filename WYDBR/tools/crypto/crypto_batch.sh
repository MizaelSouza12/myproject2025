#!/bin/bash

echo "===== WYDBR Config Encryptor ====="
echo "Criptografando arquivos sensíveis..."

TARGET_DIR="$1"
if [ -z "$TARGET_DIR" ]; then
  echo "Uso: $0 <diretório_alvo>"
  exit 1
fi

if [ ! -d "$TARGET_DIR" ]; then
  echo "Erro: Diretório alvo não existe: $TARGET_DIR"
  exit 1
fi

# Criar diretório para arquivos criptografados
mkdir -p "$TARGET_DIR/encrypted"

# Encontrar arquivos de texto/configuração
TEXT_FILES=$(find "$TARGET_DIR" -name "*.json" -o -name "*.ini" -o -name "*.txt" -o -name "*.cfg" -o -name "*.xml" -o -name "*.csv" -o -name "*.md")

if [ -n "$TEXT_FILES" ]; then
  echo "Criptografando arquivos..."
  node encrypt_config.js $TEXT_FILES
else
  echo "Nenhum arquivo de texto/configuração encontrado."
fi

echo "Criptografia concluída!"

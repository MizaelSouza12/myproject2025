#!/bin/bash

echo "===== WYDBR Binary Obfuscator ====="
echo "Ofuscando arquivos binários para dificultar engenharia reversa..."

TARGET_DIR="$1"
if [ -z "$TARGET_DIR" ]; then
  echo "Uso: $0 <diretório_alvo>"
  exit 1
fi

if [ ! -d "$TARGET_DIR" ]; then
  echo "Erro: Diretório alvo não existe: $TARGET_DIR"
  exit 1
fi

# Criar diretório para backups de arquivos ofuscados
mkdir -p "$TARGET_DIR/obfuscated"

# Encontrar arquivos binários
BIN_FILES=$(find "$TARGET_DIR" -name "*.bin" -o -name "*.dat" -o -name "*.wyd")

if [ -n "$BIN_FILES" ]; then
  echo "Ofuscando arquivos binários..."
  node obfuscate_bin.js $BIN_FILES
else
  echo "Nenhum arquivo binário encontrado."
fi

echo "Ofuscação concluída!"

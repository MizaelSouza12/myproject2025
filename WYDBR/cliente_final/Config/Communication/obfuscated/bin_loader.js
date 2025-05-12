/**
 * WYDBR Binary Loader
 * 
 * Este arquivo é usado pelo sistema para carregar binários ofuscados.
 * Não modifique este arquivo!
 */

const fs = require('fs');
const crypto = require('crypto');
const path = require('path');

// Função interna para desofuscar arquivos binários
function deobfuscateBinary(filePath) {
  // Verificar se o arquivo existe
  if (!fs.existsSync(filePath)) {
    throw new Error(`Arquivo binário não encontrado: ${filePath}`);
  }
  
  // Ler o arquivo ofuscado
  const obfuscatedData = fs.readFileSync(filePath);
  
  // Extrair partes do arquivo
  const fakeHeader = obfuscatedData.slice(0, 64);
  const padding = obfuscatedData.slice(64, 320);
  
  // Verificar assinatura falsa (validação adicional)
  const signature = fakeHeader.slice(0, 6).toString();
  if (signature !== 'MSFILE') {
    throw new Error(`Formato de arquivo inválido: ${filePath}`);
  }
  
  // Extrair tamanho original
  const originalSize = fakeHeader.readUInt32LE(42);
  
  // Calcular posição do trailer
  const trailerPos = 320 + originalSize;
  const trailer = obfuscatedData.slice(trailerPos, trailerPos + 64);
  
  // Extrair dados XOR
  const xorData = obfuscatedData.slice(320, trailerPos);
  
  // Gerar a mesma chave que o ofuscador
  const key = crypto.createHash('sha256').update('WYDBINPROTECT' + path.basename(filePath)).digest();
  
  // Reverter o XOR
  const originalData = Buffer.alloc(xorData.length);
  for (let i = 0; i < xorData.length; i++) {
    originalData[i] = xorData[i] ^ key[i % key.length];
  }
  
  // Extrair hash original do trailer
  const originalHash = trailer.slice(16, 48);
  
  // Verificar o hash para garantir integridade
  const calculatedHash = crypto.createHash('sha256').update(originalData).digest();
  if (!calculatedHash.equals(originalHash)) {
    throw new Error(`Verificação de integridade falhou: ${filePath}`);
  }
  
  return originalData;
}

// Exportar apenas a função pública
module.exports = {
  loadBinary: function(filePath) {
    try {
      return deobfuscateBinary(filePath);
    } catch (error) {
      console.error(`Erro ao carregar binário: ${error.message}`);
      return null;
    }
  }
};

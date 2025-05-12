/**
 * WYDBR Binary Obfuscator
 * 
 * Este script adiciona camadas extras de ofuscação para arquivos binários
 * (.bin, .dat, etc.) a fim de dificultar a engenharia reversa.
 */

const fs = require('fs');
const path = require('path');
const crypto = require('crypto');

// Função para ofuscar um arquivo binário
function obfuscateBinary(filePath, outputPath = null) {
  if (!outputPath) {
    outputPath = filePath + '.wob'; // WYD OBfuscated
  }
  
  console.log(`Ofuscando arquivo binário: ${filePath} -> ${outputPath}`);
  
  // Ler o arquivo original
  const originalData = fs.readFileSync(filePath);
  
  // Calcular hash para verificação
  const originalHash = crypto.createHash('sha256').update(originalData).digest();
  
  // Gerar chave baseada no nome do arquivo para consistência
  const key = crypto.createHash('sha256').update('WYDBINPROTECT' + path.basename(filePath)).digest();
  
  // Aplicar transformações de ofuscação:
  
  // 1. Adicionar dados "falsos" para confundir analisadores
  const padding = Buffer.alloc(256);
  crypto.randomFillSync(padding);
  
  // 2. XOR os dados com a chave
  const xorData = Buffer.alloc(originalData.length);
  for (let i = 0; i < originalData.length; i++) {
    xorData[i] = originalData[i] ^ key[i % key.length];
  }
  
  // 3. Adicionar um cabeçalho com informações falsas
  const fakeHeader = Buffer.alloc(64);
  fakeHeader.write('MSFILE', 0); // Falso identificador para confundir
  crypto.randomFillSync(fakeHeader, 6, 26); // Dados aleatórios
  
  // Adicionar o tamanho real escondido dentro dos dados aleatórios
  fakeHeader.writeUInt32LE(originalData.length, 42);
  
  // 4. Esconder o hash original no final
  const trailer = Buffer.alloc(64);
  originalHash.copy(trailer, 16);
  crypto.randomFillSync(trailer, 0, 16);
  crypto.randomFillSync(trailer, 48, 16);
  
  // 5. Concatenar tudo em ordem específica
  const obfuscatedData = Buffer.concat([
    fakeHeader,     // Cabeçalho falso com metadados escondidos
    padding,        // Dados falsos para confundir
    xorData,        // Dados originais com XOR aplicado
    trailer         // Trailer com hash escondido
  ]);
  
  // Salvar o arquivo ofuscado
  fs.writeFileSync(outputPath, obfuscatedData);
  
  return outputPath;
}

// Função para criar um loader de binários
function createBinaryLoader(outputDir) {
  const loaderPath = path.join(outputDir, 'bin_loader.js');
  
  const loaderCode = `/**
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
    throw new Error(\`Arquivo binário não encontrado: \${filePath}\`);
  }
  
  // Ler o arquivo ofuscado
  const obfuscatedData = fs.readFileSync(filePath);
  
  // Extrair partes do arquivo
  const fakeHeader = obfuscatedData.slice(0, 64);
  const padding = obfuscatedData.slice(64, 320);
  
  // Verificar assinatura falsa (validação adicional)
  const signature = fakeHeader.slice(0, 6).toString();
  if (signature !== 'MSFILE') {
    throw new Error(\`Formato de arquivo inválido: \${filePath}\`);
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
    throw new Error(\`Verificação de integridade falhou: \${filePath}\`);
  }
  
  return originalData;
}

// Exportar apenas a função pública
module.exports = {
  loadBinary: function(filePath) {
    try {
      return deobfuscateBinary(filePath);
    } catch (error) {
      console.error(\`Erro ao carregar binário: \${error.message}\`);
      return null;
    }
  }
};
`;
  
  // Salvar o loader
  fs.writeFileSync(loaderPath, loaderCode);
  console.log(`Loader de binários criado em: ${loaderPath}`);
}

// Processar argumentos da linha de comando
const args = process.argv.slice(2);
if (args.length === 0) {
  console.log('Uso: node obfuscate_bin.js arquivo1.bin [arquivo2.bin ...]');
  process.exit(1);
}

// Criar um diretório para arquivos ofuscados
const obfuscatedDir = path.join(path.dirname(args[0]), 'obfuscated');
if (!fs.existsSync(obfuscatedDir)) {
  fs.mkdirSync(obfuscatedDir);
}

// Processar cada arquivo
let obfuscatedFiles = 0;
args.forEach(file => {
  if (fs.existsSync(file)) {
    // Determinar o caminho de saída
    const fileName = path.basename(file);
    const outputPath = path.join(obfuscatedDir, fileName + '.wob');
    
    // Ofuscar o arquivo
    obfuscateBinary(file, outputPath);
    obfuscatedFiles++;
    
    // Substituir o arquivo original
    fs.copyFileSync(outputPath, file);
    console.log(`Arquivo original substituído: ${file}`);
  } else {
    console.log(`❌ Arquivo não encontrado: ${file}`);
  }
});

// Criar o loader
createBinaryLoader(obfuscatedDir);

console.log(`\nOfuscação concluída! ${obfuscatedFiles} arquivo(s) processado(s).`);
console.log(`Backups dos arquivos ofuscados estão em: ${obfuscatedDir}`);

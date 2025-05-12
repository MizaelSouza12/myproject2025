/**
 * WYDBR Config Encryptor
 * 
 * Este script criptografa arquivos de configuração e outros arquivos de texto
 * para proteger informações sensíveis e dificultar o acesso de curiosos.
 */

const fs = require('fs');
const path = require('path');
const crypto = require('crypto');

// Função para criptografar um arquivo
function encryptFile(filePath, outputPath = null) {
  if (!outputPath) {
    outputPath = filePath + '.wenc'; // WYD ENCrypted
  }
  
  console.log(`Criptografando: ${filePath} -> ${outputPath}`);
  
  // Ler o arquivo original
  const plaintext = fs.readFileSync(filePath, 'utf8');
  
  // Gerar uma chave e IV aleatórios (mas usar um seed fixo para consistência)
  const hash = crypto.createHash('sha256').update('WYDBR2025SECRETKEY' + path.basename(filePath)).digest();
  const key = hash.slice(0, 32); // 256 bits para AES-256
  const iv = hash.slice(32, 48); // 128 bits para o IV (16 bytes)
  
  // Criptografar o conteúdo
  const cipher = crypto.createCipheriv('aes-256-cbc', key, iv);
  let encrypted = cipher.update(plaintext, 'utf8', 'binary');
  encrypted += cipher.final('binary');
  
  // Criar um cabeçalho com informações de identificação
  const header = Buffer.concat([
    Buffer.from('WYDCFG\0\0', 'ascii'), // Assinatura
    Buffer.from([0x01, 0x00, 0x00, 0x00]), // Versão
    Buffer.from([0x00, 0x00, 0x00, 0x00]), // Flags
    Buffer.from([plaintext.length & 0xFF, (plaintext.length >> 8) & 0xFF, 
                (plaintext.length >> 16) & 0xFF, (plaintext.length >> 24) & 0xFF]), // Tamanho original
    crypto.createHash('md5').update(plaintext).digest(), // Checksum MD5 do conteúdo original
    Buffer.alloc(16) // Espaço reservado
  ]);
  
  // Combinar cabeçalho com conteúdo criptografado
  const finalBuffer = Buffer.concat([
    header,
    Buffer.from(encrypted, 'binary')
  ]);
  
  // Salvar o arquivo criptografado
  fs.writeFileSync(outputPath, finalBuffer);
  
  return outputPath;
}

// Função para criar um loader de configuração
function createConfigLoader(outputDir) {
  const loaderPath = path.join(outputDir, 'config_loader.js');
  
  const loaderCode = `/**
 * WYDBR Config Loader
 * 
 * Este arquivo é usado pelo sistema para carregar configurações criptografadas.
 * Não modifique este arquivo!
 */

const fs = require('fs');
const crypto = require('crypto');

// Função interna para decodificar arquivos criptografados
function decryptConfig(filePath, keyBase) {
  // Verificar se o arquivo existe
  if (!fs.existsSync(filePath)) {
    throw new Error(\`Arquivo de configuração não encontrado: \${filePath}\`);
  }
  
  // Ler o arquivo criptografado
  const encryptedData = fs.readFileSync(filePath);
  
  // Verificar assinatura
  const signature = encryptedData.slice(0, 8).toString('ascii');
  if (signature !== 'WYDCFG\\0\\0') {
    throw new Error(\`Formato de arquivo inválido: \${filePath}\`);
  }
  
  // Extrair informações do cabeçalho
  const version = encryptedData.readUInt32LE(8);
  const originalSize = encryptedData.readUInt32LE(16);
  const originalChecksum = encryptedData.slice(20, 36);
  
  // Dados criptografados começam após o cabeçalho (64 bytes)
  const encryptedContent = encryptedData.slice(64);
  
  // Gerar chave e IV da mesma forma que o encriptador
  const hash = crypto.createHash('sha256').update(keyBase + path.basename(filePath)).digest();
  const key = hash.slice(0, 32);
  const iv = hash.slice(32, 48);
  
  // Descriptografar
  const decipher = crypto.createDecipheriv('aes-256-cbc', key, iv);
  let decrypted = decipher.update(encryptedContent);
  decrypted = Buffer.concat([decrypted, decipher.final()]);
  
  // Verificar o tamanho
  if (decrypted.length !== originalSize) {
    throw new Error(\`Tamanho inválido após descriptografia: \${filePath}\`);
  }
  
  // Verificar o checksum
  const checksum = crypto.createHash('md5').update(decrypted).digest();
  if (!checksum.equals(originalChecksum)) {
    throw new Error(\`Checksum inválido após descriptografia: \${filePath}\`);
  }
  
  return decrypted.toString('utf8');
}

// Exportar apenas a função públicada
module.exports = {
  loadConfig: function(filePath) {
    try {
      // A chave real estaria incorporada no executável ou em um módulo nativo
      return decryptConfig(filePath, 'WYDBR2025SECRETKEY');
    } catch (error) {
      console.error(\`Erro ao carregar configuração: \${error.message}\`);
      return null;
    }
  },
  
  // Esta função retorna o objeto parseado, assumindo que o arquivo é JSON
  loadJsonConfig: function(filePath) {
    const content = this.loadConfig(filePath);
    if (content) {
      try {
        return JSON.parse(content);
      } catch (error) {
        console.error(\`Erro ao parsear JSON: \${error.message}\`);
        return null;
      }
    }
    return null;
  }
};
`;
  
  // Salvar o loader
  fs.writeFileSync(loaderPath, loaderCode);
  console.log(`Loader de configuração criado em: ${loaderPath}`);
}

// Processar argumentos da linha de comando
const args = process.argv.slice(2);
if (args.length === 0) {
  console.log('Uso: node encrypt_config.js arquivo1 [arquivo2 ...]');
  process.exit(1);
}

// Criar um diretório para arquivos criptografados
const encryptedDir = path.join(path.dirname(args[0]), 'encrypted');
if (!fs.existsSync(encryptedDir)) {
  fs.mkdirSync(encryptedDir);
}

// Processar cada arquivo
let encryptedFiles = 0;
args.forEach(file => {
  if (fs.existsSync(file)) {
    // Determinar o caminho de saída
    const fileName = path.basename(file);
    const outputPath = path.join(encryptedDir, fileName + '.wenc');
    
    // Criptografar o arquivo
    encryptFile(file, outputPath);
    encryptedFiles++;
    
    // Substituir o arquivo original por um aviso
    const warningContent = `Este arquivo foi movido para um formato criptografado para segurança.\nData: ${new Date().toISOString()}\n`;
    fs.writeFileSync(file, warningContent);
  } else {
    console.log(`❌ Arquivo não encontrado: ${file}`);
  }
});

// Criar o loader
createConfigLoader(encryptedDir);

console.log(`\nCriptografia concluída! ${encryptedFiles} arquivo(s) processado(s).`);
console.log(`Os arquivos criptografados estão em: ${encryptedDir}`);

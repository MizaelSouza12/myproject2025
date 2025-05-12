/**
 * WYDBR - Graphics Engine Patcher
 * 
 * Este script adiciona suporte aos novos motores gráficos (DirectX 11, Vulkan, OpenGL)
 * no executável WYD.exe sem modificar sua estrutura original.
 * 
 * Ele insere uma tabela de compatibilidade no final do arquivo que será lida
 * pelos novos arquivos DLL durante a inicialização.
 */

const fs = require('fs');
const path = require('path');

const WYD_PATH = path.join(__dirname, '..', '..', 'cliente', 'WYD.exe');

// Verifica se o arquivo existe
if (!fs.existsSync(WYD_PATH)) {
  console.error('ERRO: WYD.exe não encontrado!');
  process.exit(1);
}

// Lê o arquivo original
const wydData = fs.readFileSync(WYD_PATH);

// Verifica se o arquivo já está patcheado
const signature = Buffer.from('WYDBR_GRAPHICS_PATCH_V1');
if (wydData.includes(signature)) {
  console.log('WYD.exe já está patcheado. Nenhuma ação necessária.');
  process.exit(0);
}

// Estrutura da tabela de compatibilidade
const compatTable = Buffer.concat([
  signature,
  Buffer.from([0x01, 0x00, 0x00, 0x00]),  // Versão
  Buffer.from([0x03, 0x00, 0x00, 0x00]),  // Número de APIs suportadas
  
  // DirectX 11
  Buffer.from([0x01, 0x00, 0x00, 0x00]),  // ID
  Buffer.from('DirectX11\0\0\0\0\0\0\0\0', 'ascii'),  // Nome (padding para 16 bytes)
  Buffer.from([0x01, 0x00, 0x00, 0x00]),  // Prioridade
  Buffer.from([0x01, 0x00, 0x00, 0x00]),  // Habilitado
  
  // Vulkan
  Buffer.from([0x02, 0x00, 0x00, 0x00]),  // ID
  Buffer.from('Vulkan\0\0\0\0\0\0\0\0\0\0', 'ascii'),  // Nome (padding para 16 bytes)
  Buffer.from([0x02, 0x00, 0x00, 0x00]),  // Prioridade
  Buffer.from([0x01, 0x00, 0x00, 0x00]),  // Habilitado
  
  // OpenGL
  Buffer.from([0x03, 0x00, 0x00, 0x00]),  // ID
  Buffer.from('OpenGL\0\0\0\0\0\0\0\0\0\0', 'ascii'),  // Nome (padding para 16 bytes)
  Buffer.from([0x03, 0x00, 0x00, 0x00]),  // Prioridade
  Buffer.from([0x01, 0x00, 0x00, 0x00]),  // Habilitado
  
  // Fim da tabela
  Buffer.from([0x00, 0x00, 0x00, 0x00])
]);

// Adiciona a tabela ao final do arquivo
const patchedData = Buffer.concat([wydData, compatTable]);

// Backup do arquivo original
const backupPath = WYD_PATH + '.backup';
fs.copyFileSync(WYD_PATH, backupPath);
console.log(`Backup criado em: ${backupPath}`);

// Salva o arquivo patcheado
fs.writeFileSync(WYD_PATH, patchedData);
console.log('WYD.exe patcheado com sucesso!');
console.log(`Tamanho original: ${wydData.length} bytes`);
console.log(`Tamanho após patch: ${patchedData.length} bytes`);
console.log(`Tabela de compatibilidade: ${compatTable.length} bytes`);

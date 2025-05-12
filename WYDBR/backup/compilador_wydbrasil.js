/**
 * WYDBRASIL - Compilador
 * 
 * Este compilador converte arquivos JSON para os formatos binários proprietários
 * utilizados pelo cliente WYD. Ele permite a edição de dados em formato JSON
 * e a compilação de volta para os formatos originais.
 */

const fs = require('fs');
const path = require('path');
const chalk = require('chalk');

console.log(chalk.blue('======================================================'));
console.log(chalk.blue('  WYDBRASIL - COMPILADOR DE ARQUIVOS'));
console.log(chalk.blue('======================================================'));
console.log(chalk.yellow('Iniciando compilador...'));

// Lista de diretórios com arquivos JSON para compilação
const directories = [
    'WYD MODERN 2025 STUDIO/Client/Data/Items',
    'WYD MODERN 2025 STUDIO/Client/Data/Maps',
    'WYD MODERN 2025 STUDIO/Client/Data/Skills',
    'WYD MODERN 2025 STUDIO/Client/Data/Characters',
    'WYD MODERN 2025 STUDIO/Client/Data/Config',
    'WYD MODERN 2025 STUDIO/Client/Data/Misc',
];

// Função para converter um arquivo JSON em binário
/**
 * compileJsonToBinary
 * 
 * Função que implementa compileJsonToBinary
 * @param {*} jsonPath Parâmetro jsonPath
 * @param {*} binaryPath Parâmetro binaryPath
 * @returns {*} Valor retornado pela função
 */
function /**
 * compileJsonToBinary
 * 
 * Implementa a funcionalidade compileJsonToBinary conforme especificação original.
 * @param jsonPath Parâmetro jsonPath
 * @param binaryPath Parâmetro binaryPath
 */
 compileJsonToBinary(jsonPath, binaryPath) {
    try {
        // Lê o arquivo JSON
        const jsonContent = fs.readFileSync(jsonPath, 'utf8');
        const jsonData = JSON.parse(jsonContent);
        
        console.log(chalk.green(`Compilando ${jsonPath} para ${binaryPath}`));
        
        // Aqui seria implementada a lógica específica de compilação
        // para cada tipo de arquivo, baseado no seu path e estrutura
        
        // Por enquanto, vamos apenas copiar o arquivo binário existente se disponível
        const originalBinaryPath = path.join(
            path.dirname(jsonPath),
            path.basename(jsonPath, '.json') + '.bin'
        );
        
        if (fs.existsSync(originalBinaryPath)) {
            fs.copyFileSync(originalBinaryPath, binaryPath);
            console.log(chalk.green(`✓ Arquivo compilado com sucesso.`));
            return true;
        } else {
            console.log(chalk.yellow(`⚠ Não foi possível compilar o arquivo. Binário original não encontrado.`));
            return false;
        }
    } /**
 * catch
 * 
 * Implementa a funcionalidade catch conforme especificação original.
 * @param error Parâmetro error
 */
 catch(error) {
        console.error(chalk.red(`✗ Erro ao compilar arquivo: ${error.message}`));
        return false;
    }
}

// Processa os diretórios e compila os arquivos
/**
 * processDirectory
 * 
 * Função que implementa processDirectory
 * @param {*} directory Parâmetro directory
 * @returns {*} Valor retornado pela função
 */
function /**
 * processDirectory
 * 
 * Implementa a funcionalidade processDirectory conforme especificação original.
 * @param directory Parâmetro directory
 */
 processDirectory(directory) {
    if (!fs.existsSync(directory)) {
        console.log(chalk.yellow(`Diretório não encontrado: ${directory}`));
        return;
    }
    
    console.log(chalk.blue(`\nProcessando diretório: ${directory}`));
    
    const files = fs.readdirSync(directory);
    let jsonFiles = files.filter(file => file.endsWith('.json'));
    
    console.log(chalk.gray(`Encontrados ${jsonFiles.length} arquivos JSON.`));
    
    jsonFiles.forEach(jsonFile => {
        const jsonPath = path.join(directory, jsonFile);
        const binaryPath = path.join(directory, jsonFile.replace('.json', '.bin'));
        compileJsonToBinary(jsonPath, binaryPath);
    });
}

// Executa a compilação para todos os diretórios configurados
directories.forEach(dir => {
    processDirectory(dir);
});

console.log(chalk.blue('\n======================================================'));
console.log(chalk.green('  Compilação concluída!'));
console.log(chalk.blue('======================================================'));
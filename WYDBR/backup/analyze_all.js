/**
 * WYD Full Analysis Suite
 * 
 * Ferramenta abrangente que executa todas as análises disponíveis em um binário WYD,
 * gerando um relatório completo que combina os resultados de todas as ferramentas.
 */

const fs = require('fs');
const path = require('path');
const { execSync } = require('child_process');

// Importa todas as ferramentas de análise
const WYDDeobfuscator = require('./crypto/wyd_deobfuscator');
const JProtectAnalyzer = require('./crypto/jprotect_analyzer');
const WYDBinaryAnalyzer = require('./wyd_bin_analyzer');
const WYDBinaryConverter = require('./binary_converter');
const { WYDProtocol } = require('./network/wyd_protocol');

/**
 * Conjunto completo de ferramentas de análise para WYD
 */
/**
 * Classe WYDAnalysisSuite
 * 
 * Esta classe implementa a funcionalidade WYDAnalysisSuite conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDAnalysisSuite {
    /**
 * constructor
 * 
 * Implementa a funcionalidade constructor conforme especificação original.
 * @param options = {} Parâmetro options = {}
 */

    constructor(options = {}) {
        this.options = {
            debug: true,
            outputDir: './analysis_results',
            ...options
        };
        
        // Inicializa ferramentas individuais
        this.deobfuscator = new WYDDeobfuscator({ debug: this.options.debug });
        this.jprotectAnalyzer = new JProtectAnalyzer({ debug: this.options.debug });
        this.binaryAnalyzer = new WYDBinaryAnalyzer();
        this.binaryConverter = new WYDBinaryConverter({ debug: this.options.debug });
        this.protocol = new WYDProtocol({ debug: this.options.debug });
        
        // Garante que o diretório de saída existe
        if (!fs.existsSync(this.options.outputDir)) {
            fs.mkdirSync(this.options.outputDir, { recursive: true });
        }
    }
    
    /**
     * Executa uma análise completa de um binário WYD
     * @param {string} targetPath Caminho para o arquivo alvo
     * @param {Object} context Contexto adicional (caminhos para arquivos relacionados)
     */
    async /**
 * analyzeTarget
 * 
 * Implementa a funcionalidade analyzeTarget conforme especificação original.
 * @param targetPath Parâmetro targetPath
 * @param context = {} Parâmetro context = {}
 */
 analyzeTarget(targetPath, context = {}) {
        if (!fs.existsSync(targetPath)) {
            throw new Error(`Arquivo não encontrado: ${targetPath}`);
        }
        
        console.log(`\n=========================================`);
        console.log(`   INICIANDO ANÁLISE COMPLETA DO WYD`);
        console.log(`=========================================`);
        console.log(`\nAlvo principal: ${targetPath}`);
        
        // Identifica o tipo de arquivo baseado na extensão e nome
        const fileName = path.basename(targetPath);
        const fileExt = path.extname(targetPath).toLowerCase();
        
        const startTime = Date.now();
        const analysisId = `analysis_${startTime}`;
        
        // Cria diretório para esta análise específica
        const analysisDir = path.join(this.options.outputDir, analysisId);
        fs.mkdirSync(analysisDir, { recursive: true });
        
        // Resultado final combinado
        const result = {
            targetFile: fileName,
            targetPath,
            analysisTime: new Date().toISOString(),
            fileInfo: {
                size: fs.statSync(targetPath).size,
                extension: fileExt,
                type: this._detectFileType(targetPath)
            },
            context,
            analysisResults: {}
        };
        
        // Executa análises baseadas no tipo de arquivo
        try {
            console.log('\n1. Analisando informações básicas do arquivo...');
            result.fileInfo.md5 = this._calculateMD5(targetPath);
            result.fileInfo.sha256 = this._calculateSHA256(targetPath);
            
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param fileExt === '.exe' || fileExt === '.dll' Parâmetro fileExt === '.exe' || fileExt === '.dll'
 */

            
            if(fileExt === '.exe' || fileExt === '.dll') {
                // Análise de executável/DLL
                await this._analyzeExecutable(targetPath, result, analysisDir);
            } else /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param fileExt === '.bin' Parâmetro fileExt === '.bin'
 */
 if(fileExt === '.bin') {
                // Análise de arquivo de dados binário
                await this._analyzeBinaryData(targetPath, result, analysisDir);
            } else {
                // Análise genérica
                await this._analyzeGenericFile(targetPath, result, analysisDir);
            }
            
            // Salva o resultado combinado
            const resultPath = path.join(analysisDir, 'combined_analysis.json');
            fs.writeFileSync(resultPath, JSON.stringify(result, null, 2));
            
            const endTime = Date.now();
            const duration = (endTime - startTime) / 1000;
            
            console.log(`\n=========================================`);
            console.log(`   ANÁLISE CONCLUÍDA EM ${duration.toFixed(2)} SEGUNDOS`);
            console.log(`=========================================`);
            console.log(`\nResultados salvos em: ${resultPath}`);
            
            return { result, outputDir: analysisDir };
            
        } /**
 * catch
 * 
 * Implementa a funcionalidade catch conforme especificação original.
 * @param error Parâmetro error
 */
 catch(error) {
            console.error(`\nERRO: ${error.message}`);
            console.error(error.stack);
            
            // Salva o resultado parcial em caso de erro
            result.error = { message: error.message, stack: error.stack };
            const resultPath = path.join(analysisDir, 'partial_analysis.json');
            fs.writeFileSync(resultPath, JSON.stringify(result, null, 2));
            
            console.log(`\nAnálise interrompida. Resultados parciais salvos em: ${resultPath}`);
            
            throw error;
        }
    }
    
    /**
     * Analisa um arquivo executável (EXE/DLL)
     */
    async /**
 * _analyzeExecutable
 * 
 * Implementa a funcionalidade _analyzeExecutable conforme especificação original.
 * @param filePath Parâmetro filePath
 * @param result Parâmetro result
 * @param outputDir Parâmetro outputDir
 */
 _analyzeExecutable(filePath, result, outputDir) {
        console.log(`Executando análise de arquivo executável: ${path.basename(filePath)}`);
        
        // 1. Analisa o formato PE
        console.log('\n2. Analisando formato PE...');
        const peAnalysisPath = path.join(outputDir, 'pe_analysis.json');
        const exeResult = this._analyzeExeFile(filePath, peAnalysisPath);
        result.analysisResults.peAnalysis = exeResult;
        
        // 2. Se for JProtectDll.dll, faz análise específica
        if (path.basename(filePath).toLowerCase() === 'jprotectdll.dll') {
            console.log('\n3. Detectado JProtectDll.dll, executando análise especializada...');
            const jprotectPath = path.join(outputDir, 'jprotect_analysis.json');
            
            // Procura WYD.exe no mesmo diretório, se não encontrar usa apenas o DLL
            const dirPath = path.dirname(filePath);
            const wydExePath = path.join(dirPath, 'WYD.exe');
            const exePath = fs.existsSync(wydExePath) ? wydExePath : null;
            
            const jprotectResult = this.jprotectAnalyzer.analyzeProtection(filePath, exePath);
            fs.writeFileSync(jprotectPath, JSON.stringify(jprotectResult, null, 2));
            
            result.analysisResults.jprotectAnalysis = jprotectResult;
        } else if (path.basename(filePath).toLowerCase() === 'wyd.exe') {
            // 3. Se for WYD.exe, busca por JProtectDll.dll e faz análise cruzada
            console.log('\n3. Detectado WYD.exe, buscando JProtectDll.dll para análise cruzada...');
            
            const dirPath = path.dirname(filePath);
            const jprotectPath = path.join(dirPath, 'JProtectDll.dll');
            
            if (fs.existsSync(jprotectPath)) {
                console.log('   JProtectDll.dll encontrado, executando análise especializada...');
                
                const jprotectOutputPath = path.join(outputDir, 'jprotect_analysis.json');
                const jprotectResult = this.jprotectAnalyzer.analyzeProtection(jprotectPath, filePath);
                fs.writeFileSync(jprotectOutputPath, JSON.stringify(jprotectResult, null, 2));
                
                result.analysisResults.jprotectAnalysis = jprotectResult;
            } else {
                console.log('   JProtectDll.dll não encontrado no mesmo diretório.');
            }
        }
        
        // 4. Extrai strings do executável
        console.log('\n4. Extraindo strings...');
        const stringsPath = path.join(outputDir, 'strings.txt');
        this._extractStrings(filePath, stringsPath);
        
        console.log(`   Strings extraídas para: ${stringsPath}`);
        
        // 5. Análise de imports/exports
        console.log('\n5. Analisando importações e exportações...');
        // Esta já está na análise PE, apenas destacando aqui
        
        return result;
    }
    
    /**
     * Analisa um arquivo de dados binário (.bin)
     */
    async /**
 * _analyzeBinaryData
 * 
 * Implementa a funcionalidade _analyzeBinaryData conforme especificação original.
 * @param filePath Parâmetro filePath
 * @param result Parâmetro result
 * @param outputDir Parâmetro outputDir
 */
 _analyzeBinaryData(filePath, result, outputDir) {
        const fileName = path.basename(filePath);
        console.log(`Executando análise de arquivo de dados: ${fileName}`);
        
        // 1. Tenta desofuscar o arquivo
        console.log('\n2. Tentando desofuscar o arquivo...');
        const deobfuscatedPath = path.join(outputDir, `${fileName}.deobfuscated`);
        
        const deobfResult = this.deobfuscator.deobfuscateFile(filePath, deobfuscatedPath);
        result.analysisResults.deobfuscation = deobfResult;
        
        // 2. Usa o BinConverter para converter para JSON
        console.log('\n3. Convertendo para formato JSON...');
        const jsonPath = path.join(outputDir, `${fileName}.json`);
        
        try {
            // Usa o arquivo desofuscado se disponível, senão usa o original
            const sourceFile = deobfResult.success ? deobfuscatedPath : filePath;
            const conversionResult = this.binaryConverter.binToJson(sourceFile, jsonPath);
            result.analysisResults.binToJsonConversion = {
                success: true,
                totalItems: conversionResult.item_count,
                validItems: conversionResult.valid_items
            };
            
            console.log(`   Convertido para JSON: ${jsonPath}`);
            console.log(`   Items válidos: ${conversionResult.valid_items} de ${conversionResult.item_count}`);
        } /**
 * catch
 * 
 * Implementa a funcionalidade catch conforme especificação original.
 * @param error Parâmetro error
 */
 catch(error) {
            console.log(`   Erro na conversão para JSON: ${error.message}`);
            result.analysisResults.binToJsonConversion = {
                success: false,
                error: error.message
            };
        }
        
        // 3. Analisa estrutura do arquivo binário
        console.log('\n4. Analisando estrutura binária...');
        const binAnalysisPath = path.join(outputDir, `${fileName}.analysis.json`);
        
        try {
            this.binaryAnalyzer.analyzeFile(filePath, binAnalysisPath);
            console.log(`   Análise binária salva em: ${binAnalysisPath}`);
            
            // Lê o resultado para incluir no relatório combinado
            const binAnalysis = JSON.parse(fs.readFileSync(binAnalysisPath, 'utf8'));
            result.analysisResults.binaryAnalysis = binAnalysis;
        } /**
 * catch
 * 
 * Implementa a funcionalidade catch conforme especificação original.
 * @param error Parâmetro error
 */
 catch(error) {
            console.log(`   Erro na análise binária: ${error.message}`);
            result.analysisResults.binaryAnalysis = {
                error: error.message
            };
        }
        
        return result;
    }
    
    /**
     * Analisa um arquivo genérico
     */
    async /**
 * _analyzeGenericFile
 * 
 * Implementa a funcionalidade _analyzeGenericFile conforme especificação original.
 * @param filePath Parâmetro filePath
 * @param result Parâmetro result
 * @param outputDir Parâmetro outputDir
 */
 _analyzeGenericFile(filePath, result, outputDir) {
        const fileName = path.basename(filePath);
        console.log(`Executando análise genérica para arquivo: ${fileName}`);
        
        // 1. Analisa estrutura binária
        console.log('\n2. Analisando estrutura binária...');
        const binAnalysisPath = path.join(outputDir, `${fileName}.analysis.json`);
        
        try {
            this.binaryAnalyzer.analyzeFile(filePath, binAnalysisPath);
            console.log(`   Análise binária salva em: ${binAnalysisPath}`);
            
            // Lê o resultado para incluir no relatório combinado
            const binAnalysis = JSON.parse(fs.readFileSync(binAnalysisPath, 'utf8'));
            result.analysisResults.binaryAnalysis = binAnalysis;
        } /**
 * catch
 * 
 * Implementa a funcionalidade catch conforme especificação original.
 * @param error Parâmetro error
 */
 catch(error) {
            console.log(`   Erro na análise binária: ${error.message}`);
            result.analysisResults.binaryAnalysis = {
                error: error.message
            };
        }
        
        // 2. Extrai strings do arquivo
        console.log('\n3. Extraindo strings...');
        const stringsPath = path.join(outputDir, 'strings.txt');
        this._extractStrings(filePath, stringsPath);
        
        console.log(`   Strings extraídas para: ${stringsPath}`);
        
        // 3. Tenta desofuscar o arquivo
        console.log('\n4. Tentando desofuscar o arquivo...');
        const deobfuscatedPath = path.join(outputDir, `${fileName}.deobfuscated`);
        
        const deobfResult = this.deobfuscator.deobfuscateFile(filePath, deobfuscatedPath);
        result.analysisResults.deobfuscation = deobfResult;
        
        return result;
    }
    
    /**
     * Analisa um arquivo executável PE usando nossa ferramenta própria
     */
    _analyzeExeFile(filePath, outputPath) {
        try {
            console.log(`   Analisando informações do executável PE: ${path.basename(filePath)}`);
            
            // Cria objeto resultado (simplificado)
            const result = {
                filePath: filePath,
                fileName: path.basename(filePath),
                fileSize: fs.statSync(filePath).size,
                md5: this._calculateMD5(filePath),
                sections: []
            };
            
            // Usa nossa ferramenta de análise de executáveis (avançada)
            try {
                console.log('   Executando análise detalhada do PE...');
                const exeAnalyzer = require('./analyze_wyd_exe');
                const analyzer = new exeAnalyzer();
                
                // Executa a análise e obtém resultado
                const analyzerResult = analyzer.analyzeExecutable(filePath, outputPath);
                console.log(`   Análise de executável concluída.`);
                
                // Retorna o resultado combinado
                return {
                    success: true,
                    ...analyzerResult
                };
            } /**
 * catch
 * 
 * Implementa a funcionalidade catch conforme especificação original.
 * @param error Parâmetro error
 */
 catch(error) {
                console.error(`   Erro na análise avançada de PE: ${error.message}`);
                
                // Continua com análise básica
                result.analyzerError = error.message;
            }
            
            // Se não conseguimos usar a análise avançada, volta para análise básica
            console.log('   Executando análise básica de PE...');
            
            // Salva o resultado
            fs.writeFileSync(outputPath, JSON.stringify(result, null, 2));
            
            return result;
            
        } /**
 * catch
 * 
 * Implementa a funcionalidade catch conforme especificação original.
 * @param error Parâmetro error
 */
 catch(error) {
            console.error(`   Erro na análise de PE: ${error.message}`);
            return { error: error.message };
        }
    }
    
    /**
     * Extrai strings de um arquivo binário
     */
    _extractStrings(filePath, outputPath) {
        try {
            // Para sistemas Unix, usa o comando strings
            let result;
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param process.platform !== 'win32' Parâmetro process.platform !== 'win32'
 */

            if(process.platform !== 'win32') {
                // No Unix, usamos o comando strings
                result = execSync(`strings "${filePath}"`, { maxBuffer: 10 * 1024 * 1024 }).toString();
            } else {
                // No Windows, usamos uma extração básica de strings em JavaScript
                result = this._extractStringsJS(filePath);
            }
            
            fs.writeFileSync(outputPath, result);
            return { success: true, count: result.split('\n').length };
        } /**
 * catch
 * 
 * Implementa a funcionalidade catch conforme especificação original.
 * @param error Parâmetro error
 */
 catch(error) {
            console.error(`Erro ao extrair strings: ${error.message}`);
            return { success: false, error: error.message };
        }
    }
    
    /**
     * Extrai strings em JavaScript (para Windows ou quando o comando strings não está disponível)
     */
    _extractStringsJS(filePath) {
        const data = fs.readFileSync(filePath);
        const minLength = 4; // Tamanho mínimo para considerar como string
        const strings = [];
        
        // Strings ASCII
        let currentString = '';
        /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let i = 0; i < data.length; i++ Parâmetro let i = 0; i < data.length; i++
 */

        for(let i = 0; i < data.length; i++) {
            const byte = data[i];
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param byte >= 32 && byte <= 126 Parâmetro byte >= 32 && byte <= 126
 */

            if(byte >= 32 && byte <= 126) { // Caracteres ASCII imprimíveis
                currentString += String.fromCharCode(byte);
            } else {
                /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param currentString.length >= minLength Parâmetro currentString.length >= minLength
 */

                if(currentString.length >= minLength) {
                    strings.push(currentString);
                }
                currentString = '';
            }
        }
        
        // Strings UTF-16 (caracteres ASCII com zero entre eles)
        let utf16String = '';
        /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let i = 0; i < data.length - 1; i += 2 Parâmetro let i = 0; i < data.length - 1; i += 2
 */

        for(let i = 0; i < data.length - 1; i += 2) {
            const byte1 = data[i];
            const byte2 = data[i + 1];
            
            // Apenas ASCII básico com zero byte
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param byte1 >= 32 && byte1 <= 126 && byte2 === 0 Parâmetro byte1 >= 32 && byte1 <= 126 && byte2 === 0
 */

            if(byte1 >= 32 && byte1 <= 126 && byte2 === 0) {
                utf16String += String.fromCharCode(byte1);
            } else {
                /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param utf16String.length >= minLength Parâmetro utf16String.length >= minLength
 */

                if(utf16String.length >= minLength) {
                    strings.push(utf16String);
                }
                utf16String = '';
            }
        }
        
        return strings.join('\n');
    }
    
    /**
     * Calcula o hash MD5 de um arquivo
     */
    _calculateMD5(filePath) {
        const data = fs.readFileSync(filePath);
        return require('crypto').createHash('md5').update(data).digest('hex');
    }
    
    /**
     * Calcula o hash SHA256 de um arquivo
     */
    _calculateSHA256(filePath) {
        const data = fs.readFileSync(filePath);
        return require('crypto').createHash('sha256').update(data).digest('hex');
    }
    
    /**
     * Detecta o tipo de arquivo baseado em nome e extensão
     */
    _detectFileType(filePath) {
        const fileName = path.basename(filePath).toLowerCase();
        const ext = path.extname(filePath).toLowerCase();
        
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param ext === '.exe' Parâmetro ext === '.exe'
 */

        
        if(ext === '.exe') {
            return 'Executable';
        } else /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param ext === '.dll' Parâmetro ext === '.dll'
 */
 if(ext === '.dll') {
            return 'Dynamic Link Library';
        } else /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param ext === '.bin' Parâmetro ext === '.bin'
 */
 if(ext === '.bin') {
            // Determina o tipo de arquivo .bin
            if (fileName.includes('item')) {
                return 'Item Data File';
            } else if (fileName.includes('mob')) {
                return 'Mob Data File';
            } else if (fileName.includes('skill')) {
                return 'Skill Data File';
            } else {
                return 'Generic Binary Data';
            }
        } else /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param ext === '.wys' Parâmetro ext === '.wys'
 */
 if(ext === '.wys') {
            return 'WYD Map File';
        } else /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param ext === '.wyt' Parâmetro ext === '.wyt'
 */
 if(ext === '.wyt') {
            return 'WYD Texture File';
        } else /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param ext === '.wyp' Parâmetro ext === '.wyp'
 */
 if(ext === '.wyp') {
            return 'WYD Model File';
        } else {
            return 'Unknown File Type';
        }
    }
}

/**
 * Função principal de execução
 */
async /**
 * main
 * 
 * Função que implementa main
 * @returns {*} Valor retornado pela função
 */
function /**
 * main
 * 
 * Implementa a funcionalidade main conforme especificação original.
 */
 main() {
    const args = process.argv.slice(2);
    
    /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param args.length === 0 Parâmetro args.length === 0
 */

    
    if(args.length === 0) {
        console.log('Uso: node analyze_all.js <caminho_do_arquivo_alvo>');
        console.log('Exemplo: node analyze_all.js "WYD MODERN 2025 STUDIO/WYD.exe"');
        return;
    }
    
    const targetPath = args[0];
    const outputDir = args[1] || './analysis_results';
    
    const suite = new WYDAnalysisSuite({ outputDir });
    
    try {
        await suite.analyzeTarget(targetPath);
    } /**
 * catch
 * 
 * Implementa a funcionalidade catch conforme especificação original.
 * @param error Parâmetro error
 */
 catch(error) {
        console.error(`Falha na análise: ${error.message}`);
        process.exit(1);
    }
}

// Executa se for chamado diretamente
/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param require.main === module Parâmetro require.main === module
 */

if(require.main === module) {
    main().catch(error => {
        console.error(error);
        process.exit(1);
    });
}

module.exports = WYDAnalysisSuite;
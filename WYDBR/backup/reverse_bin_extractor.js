/**
 * WYD Binary File Extractor (Reversing Tool)
 * 
 * Ferramenta especializada para extrair estruturas de dados dos arquivos binários do WYD.
 * Foca na extração e conversão dos arquivos .bin para formatos analisáveis.
 * 
 * Uso: node reverse_bin_extractor.js <arquivo.bin> [--format=json|csv|headers] [--output=arquivo_saida]
 */

const fs = require('fs');
const path = require('path');
const crypto = require('crypto');

// Constantes
const KNOWN_STRUCTURES = {
    ITEM: { size: 72, name: 'ItemList.bin' },
    MOB: { size: 368, name: 'MobList.bin' },
    SKILL: { size: 32, name: 'SkillData.bin' },
    DROP: { size: 24, name: 'DropList.bin' }
};

/**
 * Extrator de arquivos binários do WYD
 */
/**
 * Classe WYDBinaryExtractor
 * 
 * Esta classe implementa a funcionalidade WYDBinaryExtractor conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDBinaryExtractor {
    /**
 * constructor
 * 
 * Implementa a funcionalidade constructor conforme especificação original.
 * @param options = {} Parâmetro options = {}
 */

    constructor(options = {}) {
        this.options = {
            debug: true,
            decryptionKey: null, // Possível chave de criptografia
            outputFormat: 'json',
            preserveRaw: true,
            ...options
        };
        
        // Mapeamento de tipos de arquivo e suas estratégias de extração
        this.extractors = {
            'ItemList.bin': this._extractItemList.bind(this),
            'MobList.bin': this._extractMobList.bind(this),
            'SkillData.bin': this._extractSkillData.bind(this),
            'DropList.bin': this._extractDropList.bind(this)
        };
    }
    
    /**
     * Extrai dados de um arquivo binário
     * @param {string} filePath Caminho para o arquivo binário
     * @param {string} outputPath Caminho para salvar o resultado (opcional)
     * @returns {Object} Dados extraídos
     */
    extractData(filePath, outputPath = null) {
        try {
            console.log(`[INFO] Analisando arquivo: ${filePath}`);
            
            // Verifica se o arquivo existe
            if (!fs.existsSync(filePath)) {
                throw new Error(`Arquivo não encontrado: ${filePath}`);
            }
            
            // Lê o arquivo binário
            const fileData = fs.readFileSync(filePath);
            const fileName = path.basename(filePath);
            
            console.log(`[INFO] Arquivo carregado: ${fileData.length} bytes`);
            console.log(`[INFO] Analisando assinatura...`);
            
            // Primeiros 16 bytes podem conter uma assinatura ou informações de formato
            const signature = fileData.slice(0, 16);
            console.log(`[INFO] Assinatura hex: ${this._bytesToHex(signature)}`);
            
            // Detecta o tipo de arquivo e extrai dados
            let extractedData = {};
            const fileType = this._detectFileType(fileName, signature);
            
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param fileType && this.extractors[fileType] Parâmetro fileType && this.extractors[fileType]
 */

            
            if(fileType && this.extractors[fileType]) {
                console.log(`[INFO] Tipo de arquivo detectado: ${fileType}`);
                extractedData = this.extractors[fileType](fileData, fileName);
            } else {
                console.log(`[AVISO] Tipo de arquivo não reconhecido. Tentando extração genérica...`);
                extractedData = this._extractGenericBinary(fileData, fileName);
            }
            
            // Salva o resultado, se especificado
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param outputPath Parâmetro outputPath
 */

            if(outputPath) {
                this._saveOutput(extractedData, outputPath);
                console.log(`[INFO] Dados extraídos salvos em: ${outputPath}`);
            }
            
            return extractedData;
            
        } /**
 * catch
 * 
 * Implementa a funcionalidade catch conforme especificação original.
 * @param error Parâmetro error
 */
 catch(error) {
            console.error(`[ERRO] Falha na extração: ${error.message}`);
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param this.options.debug Parâmetro this.options.debug
 */

            if(this.options.debug) {
                console.error(error.stack);
            }
            return { error: error.message };
        }
    }
    
    /**
     * Detecta o tipo de arquivo com base no nome e assinatura
     */
    _detectFileType(fileName, signature) {
        // Tenta identificar pelo nome do arquivo primeiro
        const knownFiles = Object.keys(this.extractors);
        /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param const knownFile of knownFiles Parâmetro const knownFile of knownFiles
 */

        for(const knownFile of knownFiles) {
            if (fileName.toLowerCase().includes(knownFile.toLowerCase())) {
                return knownFile;
            }
        }
        
        // Se não conseguiu pelo nome, tenta pela assinatura ou tamanho
        // Implementação simplificada, expandir conforme necessário
        
        return null;
    }
    
    /**
     * Extrai dados do arquivo ItemList.bin
     */
    _extractItemList(fileData, fileName) {
        console.log('[INFO] Extraindo dados de ItemList.bin...');
        
        const itemSize = KNOWN_STRUCTURES.ITEM.size;
        const itemCount = Math.floor(fileData.length / itemSize);
        
        console.log(`[INFO] Tamanho de cada item: ${itemSize} bytes`);
        console.log(`[INFO] Número total de itens possíveis: ${itemCount}`);
        
        // Identifica possível offset onde os dados reais começam
        let dataOffset = 0;
        // Procura por padrões como repetição de bytes (0x5A, 0xA4)
        const patterns = [
            { value: 0x5A, name: 'Byte 0x5A', threshold: 10 },
            { value: 0xA4, name: 'Byte 0xA4', threshold: 10 }
        ];
        
        /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param const pattern of patterns Parâmetro const pattern of patterns
 */

        
        for(const pattern of patterns) {
            let count = 0;
            /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let i = 0; i < 100 && i < fileData.length; i++ Parâmetro let i = 0; i < 100 && i < fileData.length; i++
 */

            for(let i = 0; i < 100 && i < fileData.length; i++) {
                if (fileData[i] === pattern.value) count++;
            }
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param count >= pattern.threshold Parâmetro count >= pattern.threshold
 */

            if(count >= pattern.threshold) {
                console.log(`[INFO] Detectado padrão de ofuscação: ${pattern.name} repetido ${count} vezes nos primeiros 100 bytes`);
            }
        }
        
        // Extrai cada item
        const items = [];
        let validItemCount = 0;
        
        /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let i = 0; i < itemCount; i++ Parâmetro let i = 0; i < itemCount; i++
 */

        
        for(let i = 0; i < itemCount; i++) {
            const offset = dataOffset + (i * itemSize);
            if (offset + itemSize > fileData.length) break;
            
            const itemData = fileData.slice(offset, offset + itemSize);
            
            // Detecta se este parece ser um item válido (não é todo zeros)
            const isValid = !this._isEmptyBlock(itemData);
            
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param isValid Parâmetro isValid
 */

            
            if(isValid) {
                const item = this._parseItemData(itemData, i);
                items.push(item);
                validItemCount++;
            }
        }
        
        console.log(`[INFO] Itens válidos encontrados: ${validItemCount}`);
        
        // Gera resultado
        const result = {
            fileName,
            fileSize: fileData.length,
            format: 'item_list',
            itemSize,
            totalItems: itemCount,
            validItems: validItemCount,
            items
        };
        
        // Se definido para preservar dados brutos
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param this.options.preserveRaw Parâmetro this.options.preserveRaw
 */

        if(this.options.preserveRaw) {
            // Não incluímos todo o buffer, apenas um hash para verificação
            result.rawDataHash = crypto.createHash('sha256').update(fileData).digest('hex');
        }
        
        return result;
    }
    
    /**
     * Extrai dados do arquivo MobList.bin
     */
    _extractMobList(fileData, fileName) {
        console.log('[INFO] Extraindo dados de MobList.bin...');
        
        const mobSize = KNOWN_STRUCTURES.MOB.size;
        const mobCount = Math.floor(fileData.length / mobSize);
        
        console.log(`[INFO] Tamanho de cada mob: ${mobSize} bytes`);
        console.log(`[INFO] Número total de mobs possíveis: ${mobCount}`);
        
        // Extrai cada mob
        const mobs = [];
        let validMobCount = 0;
        
        /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let i = 0; i < mobCount; i++ Parâmetro let i = 0; i < mobCount; i++
 */

        
        for(let i = 0; i < mobCount; i++) {
            const offset = i * mobSize;
            if (offset + mobSize > fileData.length) break;
            
            const mobData = fileData.slice(offset, offset + mobSize);
            
            // Detecta se este parece ser um mob válido
            const isValid = !this._isEmptyBlock(mobData);
            
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param isValid Parâmetro isValid
 */

            
            if(isValid) {
                const mob = this._parseMobData(mobData, i);
                mobs.push(mob);
                validMobCount++;
            }
        }
        
        console.log(`[INFO] Mobs válidos encontrados: ${validMobCount}`);
        
        // Gera resultado
        return {
            fileName,
            fileSize: fileData.length,
            format: 'mob_list',
            mobSize,
            totalMobs: mobCount,
            validMobs: validMobCount,
            mobs
        };
    }
    
    /**
     * Extrai dados do arquivo SkillData.bin
     */
    _extractSkillData(fileData, fileName) {
        console.log('[INFO] Extraindo dados de SkillData.bin...');
        
        const skillSize = KNOWN_STRUCTURES.SKILL.size;
        const skillCount = Math.floor(fileData.length / skillSize);
        
        console.log(`[INFO] Tamanho de cada skill: ${skillSize} bytes`);
        console.log(`[INFO] Número total de skills possíveis: ${skillCount}`);
        
        // Função para extrair skills
        const skills = [];
        let validSkillCount = 0;
        
        /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let i = 0; i < skillCount; i++ Parâmetro let i = 0; i < skillCount; i++
 */

        
        for(let i = 0; i < skillCount; i++) {
            const offset = i * skillSize;
            if (offset + skillSize > fileData.length) break;
            
            const skillData = fileData.slice(offset, offset + skillSize);
            
            // Detecta se esta skill parece válida
            const isValid = !this._isEmptyBlock(skillData);
            
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param isValid Parâmetro isValid
 */

            
            if(isValid) {
                const skill = this._parseSkillData(skillData, i);
                skills.push(skill);
                validSkillCount++;
            }
        }
        
        console.log(`[INFO] Skills válidas encontradas: ${validSkillCount}`);
        
        // Gera resultado
        return {
            fileName,
            fileSize: fileData.length,
            format: 'skill_data',
            skillSize,
            totalSkills: skillCount,
            validSkills: validSkillCount,
            skills
        };
    }
    
    /**
     * Extrai dados do arquivo DropList.bin
     */
    _extractDropList(fileData, fileName) {
        console.log('[INFO] Extraindo dados de DropList.bin...');
        
        // Implementação para DropList.bin
        // TODO: Implementar extração específica de dados do DropList.bin
        
        return this._extractGenericBinary(fileData, fileName);
    }
    
    /**
     * Extração genérica para arquivos não reconhecidos
     */
    _extractGenericBinary(fileData, fileName) {
        console.log('[INFO] Executando extração genérica...');
        
        // Para arquivos desconhecidos, tentamos detectar estruturas repetitivas
        const possibleSizes = [8, 16, 24, 32, 64, 72, 96, 128, 256, 368, 512];
        let bestSize = 0;
        let bestCount = 0;
        
        // Estatísticas básicas
        const stats = {
            fileSize: fileData.length,
            byteFrequency: {},
            patternInfo: {}
        };
        
        // Calculamos a frequência de bytes
        /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let i = 0; i < fileData.length; i++ Parâmetro let i = 0; i < fileData.length; i++
 */

        for(let i = 0; i < fileData.length; i++) {
            const byte = fileData[i];
            stats.byteFrequency[byte] = (stats.byteFrequency[byte] || 0) + 1;
        }
        
        // Ordenamos os bytes mais frequentes
        const sortedBytes = Object.entries(stats.byteFrequency)
            .sort((a, b) => b[1] - a[1])
            .slice(0, 10)
            .map(([byte, count]) => ({ 
                byte: parseInt(byte), 
                count, 
                percentage: (count / fileData.length * 100).toFixed(2) + '%'
            }));
        
        // Para cada tamanho possível, verificamos se o arquivo é divisível
        /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param const size of possibleSizes Parâmetro const size of possibleSizes
 */

        for(const size of possibleSizes) {
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param fileData.length % size === 0 Parâmetro fileData.length % size === 0
 */

            if(fileData.length % size === 0) {
                const count = fileData.length / size;
                
                // Se encontramos pelo menos 5 registros com este tamanho
                /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param count >= 5 Parâmetro count >= 5
 */

                if(count >= 5) {
                    console.log(`[INFO] Possível estrutura encontrada: ${count} registros de ${size} bytes cada`);
                    
                    /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param count > bestCount Parâmetro count > bestCount
 */

                    
                    if(count > bestCount) {
                        bestCount = count;
                        bestSize = size;
                    }
                    
                    // Analisamos o primeiro registro como exemplo
                    const firstRecord = fileData.slice(0, size);
                    stats.patternInfo[size] = {
                        recordCount: count,
                        firstRecordSample: this._bytesToHex(firstRecord.slice(0, Math.min(16, size)))
                    };
                }
            }
        }
        
        // Se encontramos um tamanho provável, extraímos os registros
        const records = [];
        
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param bestSize > 0 Parâmetro bestSize > 0
 */

        
        if(bestSize > 0) {
            console.log(`[INFO] Melhor estimativa: ${bestCount} registros de ${bestSize} bytes cada`);
            
            /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let i = 0; i < bestCount; i++ Parâmetro let i = 0; i < bestCount; i++
 */

            
            for(let i = 0; i < bestCount; i++) {
                const offset = i * bestSize;
                const recordData = fileData.slice(offset, offset + bestSize);
                
                if (!this._isEmptyBlock(recordData)) {
                    // Para registros genéricos, extraímos apenas informações básicas
                    records.push({
                        index: i,
                        offset,
                        // Alguns campos básicos comuns em muitos formatos
                        id: recordData.readUInt16LE(0), // Assumindo que o primeiro campo é um ID de 16 bits
                        hexSample: this._bytesToHex(recordData.slice(0, Math.min(32, bestSize))),
                        // Poderia incluir outros campos específicos se soubermos mais sobre o formato
                    });
                }
            }
        }
        
        return {
            fileName,
            fileSize: fileData.length,
            format: 'unknown',
            estimatedRecordSize: bestSize,
            estimatedRecordCount: bestCount,
            recordsExtracted: records.length,
            frequentBytes: sortedBytes,
            patternInfo: stats.patternInfo,
            records: records.length > 0 ? records : undefined
        };
    }
    
    /**
     * Analisa os dados de um item extraído
     */
    _parseItemData(data, index) {
        // Esta função mapeia os bytes do buffer para campos de estrutura
        // Baseado na estrutura definida em item_struct.h
        
        try {
            return {
                index,
                // Cabeçalho do item
                id: data.readUInt16LE(0),
                itemId: data.readUInt16LE(2),
                type: data.readUInt8(4),
                subType: data.readUInt8(5),
                level: data.readUInt8(6),
                
                // Atributos básicos
                refinement: data.readUInt8(7),
                quality: data.readUInt8(8),
                durability: data.readUInt8(9),
                maxDurability: data.readUInt8(10),
                
                // Atributos adicionais
                attack: data.readUInt16LE(11),
                defense: data.readUInt16LE(13),
                reqLevel: data.readUInt16LE(15),
                reqStr: data.readUInt16LE(17),
                reqDex: data.readUInt16LE(19),
                reqInt: data.readUInt16LE(21),
                reqCon: data.readUInt16LE(23),
                
                // Efeitos especiais (apenas os bytes brutos, a interpretação varia)
                effects: Array.from(data.slice(25, 33)),
                
                // Sockets
                sockets: [
                    data.readUInt16LE(33),
                    data.readUInt16LE(35),
                    data.readUInt16LE(37),
                    data.readUInt16LE(39)
                ],
                
                // Tempo e flags
                creationTime: data.readUInt32LE(41),
                expirationTime: data.readUInt32LE(45),
                flags: data.readUInt32LE(49),
                
                // Dados brutos para referência
                rawHex: this._bytesToHex(data)
            };
        } /**
 * catch
 * 
 * Implementa a funcionalidade catch conforme especificação original.
 * @param error Parâmetro error
 */
 catch(error) {
            console.error(`[ERRO] Falha ao analisar item #${index}: ${error.message}`);
            return {
                index,
                error: error.message,
                rawHex: this._bytesToHex(data)
            };
        }
    }
    
    /**
     * Analisa os dados de um mob extraído
     */
    _parseMobData(data, index) {
        // Esta função mapeia os bytes do buffer para campos de estrutura
        // Baseado na estrutura definida em mob_struct.h
        
        try {
            // Extrai o nome (string terminada em null, máximo 16 bytes)
            const nameBuffer = data.slice(0, 16);
            let nameLength = 0;
            /**
 * while
 * 
 * Implementa a funcionalidade while conforme especificação original.
 * @param nameLength < nameBuffer.length && nameBuffer[nameLength] !== 0 Parâmetro nameLength < nameBuffer.length && nameBuffer[nameLength] !== 0
 */

            while(nameLength < nameBuffer.length && nameBuffer[nameLength] !== 0) {
                nameLength++;
            }
            const name = nameBuffer.slice(0, nameLength).toString('utf8');
            
            return {
                index,
                // Informações básicas
                name,
                mobId: data.readUInt16LE(16),
                merchant: data.readUInt8(18),
                class: data.readUInt8(19),
                // AffectInfo omitido por brevidade
                
                // Level e experiência
                level: data.readUInt16LE(52),
                // Outros campos omitidos por brevidade
                
                // Apenas algumas informações chave para exemplificar
                strength: data.readUInt16LE(60),
                dexterity: data.readUInt16LE(62),
                intelligence: data.readUInt16LE(64),
                constitution: data.readUInt16LE(66),
                
                // Coordenadas
                posX: data.readUInt16LE(96),
                posY: data.readUInt16LE(98),
                
                // Dados brutos para referência
                rawHex: this._bytesToHex(data.slice(0, 100)) // Primeiros 100 bytes apenas, para não ficar grande demais
            };
        } /**
 * catch
 * 
 * Implementa a funcionalidade catch conforme especificação original.
 * @param error Parâmetro error
 */
 catch(error) {
            console.error(`[ERRO] Falha ao analisar mob #${index}: ${error.message}`);
            return {
                index,
                error: error.message,
                rawHex: this._bytesToHex(data.slice(0, 64))
            };
        }
    }
    
    /**
     * Analisa os dados de uma skill extraída
     */
    _parseSkillData(data, index) {
        // Implementação para extração de skills
        // TODO: Implementar conforme a estrutura real de SkillData.bin
        
        return {
            index,
            // Alguns campos básicos de exemplo
            id: data.readUInt16LE(0),
            level: data.readUInt8(2),
            type: data.readUInt8(3),
            // Outros campos omitidos
            
            // Dados brutos para referência
            rawHex: this._bytesToHex(data)
        };
    }
    
    /**
     * Verifica se um bloco de dados está vazio (todos zeros ou valores repetidos)
     */
    _isEmptyBlock(data) {
        // Se todos os bytes são zero, é vazio
        if (data.every(byte => byte === 0)) {
            return true;
        }
        
        // Se 95% dos bytes são iguais, provavelmente é um bloco de padding
        const byteCount = {};
        /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param const byte of data Parâmetro const byte of data
 */

        for(const byte of data) {
            byteCount[byte] = (byteCount[byte] || 0) + 1;
        }
        
        const mostFrequent = Object.entries(byteCount).sort((a, b) => b[1] - a[1])[0];
        const [byte, count] = mostFrequent;
        
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param count / data.length > 0.95 Parâmetro count / data.length > 0.95
 */

        
        if(count / data.length > 0.95) {
            return true;
        }
        
        return false;
    }
    
    /**
     * Salva dados extraídos em um arquivo
     */
    _saveOutput(data, outputPath) {
        const format = this.options.outputFormat.toLowerCase();
        
        /**
 * switch
 * 
 * Implementa a funcionalidade switch conforme especificação original.
 * @param format Parâmetro format
 */

        
        switch(format) {
            case 'json':
                fs.writeFileSync(outputPath, JSON.stringify(data, null, 2));
                break;
            case 'csv':
                this._saveAsCsv(data, outputPath);
                break;
            case 'headers':
                this._saveAsHeaders(data, outputPath);
                break;
            default:
                console.log(`[AVISO] Formato de saída desconhecido: ${format}, usando JSON.`);
                fs.writeFileSync(outputPath, JSON.stringify(data, null, 2));
        }
    }
    
    /**
     * Salva dados em formato CSV
     */
    _saveAsCsv(data, outputPath) {
        // Implementação simplificada para CSV
        // Apenas para itens e mobs por enquanto
        
        let csvContent = '';
        let headers = [];
        let records = [];
        
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param data.format === 'item_list' && data.items && data.items.length > 0 Parâmetro data.format === 'item_list' && data.items && data.items.length > 0
 */

        
        if(data.format === 'item_list' && data.items && data.items.length > 0) {
            records = data.items;
            headers = Object.keys(records[0]).filter(key => key !== 'rawHex' && key !== 'effects');
        } else /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param data.format === 'mob_list' && data.mobs && data.mobs.length > 0 Parâmetro data.format === 'mob_list' && data.mobs && data.mobs.length > 0
 */
 if(data.format === 'mob_list' && data.mobs && data.mobs.length > 0) {
            records = data.mobs;
            headers = Object.keys(records[0]).filter(key => key !== 'rawHex');
        } else {
            console.log('[AVISO] Formato não suportado para CSV, salvando como JSON');
            fs.writeFileSync(outputPath, JSON.stringify(data, null, 2));
            return;
        }
        
        // Gera cabeçalho
        csvContent += headers.join(',') + '\n';
        
        // Gera linhas de dados
        /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param const record of records Parâmetro const record of records
 */

        for(const record of records) {
            const line = headers.map(header => {
                const value = record[header];
                if (value === null || value === undefined) return '';
                if (typeof value === 'string') return `"${value.replace(/"/g, '""')}"`;
                if (Array.isArray(value)) return `"[${value.join(',')}]"`;
                return value;
            }).join(',');
            
            csvContent += line + '\n';
        }
        
        fs.writeFileSync(outputPath, csvContent);
    }
    
    /**
     * Salva dados em formato de headers C/C++
     */
    _saveAsHeaders(data, outputPath) {
        let headerContent = '';
        
        const guardName = path.basename(outputPath).toUpperCase().replace('.', '_');
        
        headerContent += `#ifndef ${guardName}\n`;
        headerContent += `#define ${guardName}\n\n`;
        headerContent += `/**\n`;
        headerContent += ` * Auto-generated header definitions from WYD binary files\n`;
        headerContent += ` * Generated on: ${new Date().toISOString()}\n`;
        headerContent += ` * Source file: ${data.fileName}\n`;
        headerContent += ` */\n\n`;
        headerContent += `#include <stdint.h>\n\n`;
        headerContent += `#pragma pack(push, 1)\n\n`;
        
        // Gera definições baseadas no tipo de arquivo
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param data.format === 'item_list' Parâmetro data.format === 'item_list'
 */

        if(data.format === 'item_list') {
            headerContent += this._generateItemHeaders(data);
        } else /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param data.format === 'mob_list' Parâmetro data.format === 'mob_list'
 */
 if(data.format === 'mob_list') {
            headerContent += this._generateMobHeaders(data);
        } else /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param data.format === 'skill_data' Parâmetro data.format === 'skill_data'
 */
 if(data.format === 'skill_data') {
            headerContent += this._generateSkillHeaders(data);
        } else {
            headerContent += `// Unknown format: ${data.format}\n`;
            headerContent += `// Unable to generate specific headers for this format\n\n`;
        }
        
        headerContent += `#pragma pack(pop)\n\n`;
        headerContent += `#endif // ${guardName}\n`;
        
        fs.writeFileSync(outputPath, headerContent);
    }
    
    /**
     * Gera headers C/C++ para estrutura de itens
     */
    _generateItemHeaders(data) {
        let content = '';
        
        content += `/**\n`;
        content += ` * Structure definition for WYD items\n`;
        content += ` * Record size: ${data.itemSize} bytes\n`;
        content += ` * Valid records: ${data.validItems} / ${data.totalItems}\n`;
        content += ` */\n\n`;
        
        content += `typedef struct _STRUCT_ITEM {\n`;
        content += `    uint16_t Index;              // ID do item\n`;
        content += `    uint16_t ItemId;             // Subtipo do item\n`;
        content += `    uint8_t  Type;               // Tipo principal do item (arma, armadura, etc)\n`;
        content += `    uint8_t  SubType;            // Subtipo secundário\n`;
        content += `    uint8_t  Level;              // Nível base do item\n`;
        content += `    uint8_t  Refinement;         // Nível de refinamento (+0 a +9)\n`;
        content += `    uint8_t  Quality;            // Qualidade (normal, superior, raro, etc)\n`;
        content += `    uint8_t  Durability;         // Durabilidade atual\n`;
        content += `    uint8_t  MaxDurability;      // Durabilidade máxima\n`;
        content += `    uint16_t Attack;             // Ataque (para armas)\n`;
        content += `    uint16_t Defense;            // Defesa (para armaduras)\n`;
        content += `    uint16_t ReqLevel;           // Nível requerido\n`;
        content += `    uint16_t ReqStr;             // Força requerida\n`;
        content += `    uint16_t ReqDex;             // Destreza requerida\n`;
        content += `    uint16_t ReqInt;             // Inteligência requerida\n`;
        content += `    uint16_t ReqCon;             // Constituição requerida\n`;
        content += `    uint8_t  Effects[8];         // Efeitos especiais\n`;
        content += `    uint16_t Sockets[4];         // Slots para gemas/aprimoramentos\n`;
        content += `    uint32_t CreationTime;       // Timestamp de criação\n`;
        content += `    uint32_t ExpirationTime;     // Timestamp de expiração (0 = sem expiração)\n`;
        content += `    uint32_t Flags;              // Flags diversas (tradeable, sellable, etc)\n`;
        content += `    uint8_t  Reserved[8];        // Bytes reservados para expansão futura\n`;
        content += `} STRUCT_ITEM;\n\n`;
        
        // Adiciona enums para tipos conhecidos
        content += `// Tipos de itens\n`;
        content += `enum ITEM_TYPE {\n`;
        content += `    ITEM_TYPE_WEAPON     = 0,    // Armas\n`;
        content += `    ITEM_TYPE_ARMOR      = 1,    // Armaduras\n`;
        content += `    ITEM_TYPE_CONSUMABLE = 2,    // Consumíveis\n`;
        content += `    ITEM_TYPE_MATERIAL   = 3,    // Materiais de crafting\n`;
        content += `    ITEM_TYPE_MISC       = 4,    // Itens diversos\n`;
        content += `    ITEM_TYPE_QUEST      = 5     // Itens de quest\n`;
        content += `};\n\n`;
        
        // Se temos uma boa amostra de itens, geramos algumas constantes de exemplo
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param data.items && data.items.length > 10 Parâmetro data.items && data.items.length > 10
 */

        if(data.items && data.items.length > 10) {
            content += `// Exemplos de IDs de itens encontrados no arquivo\n`;
            content += `enum ITEM_ID {\n`;
            
            const sampleItems = data.items.slice(0, 20); // Limitamos a 20 exemplos
            /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let i = 0; i < sampleItems.length; i++ Parâmetro let i = 0; i < sampleItems.length; i++
 */

            for(let i = 0; i < sampleItems.length; i++) {
                const item = sampleItems[i];
                /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param item.id Parâmetro item.id
 */

                if(item.id) {
                    content += `    ITEM_ID_${i} = ${item.id},${i < sampleItems.length - 1 ? '\n' : ''}`; 
                }
            }
            
            content += `\n};\n\n`;
        }
        
        return content;
    }
    
    /**
     * Gera headers C/C++ para estrutura de mobs
     */
    _generateMobHeaders(data) {
        // Similar à _generateItemHeaders mas para estrutura de mobs
        // TODO: Implementar conforme os campos reais em mob_struct.h
        
        let content = '';
        
        content += `/**\n`;
        content += ` * Structure definition for WYD mobs/characters\n`;
        content += ` * Record size: ${data.mobSize} bytes\n`;
        content += ` * Valid records: ${data.validMobs} / ${data.totalMobs}\n`;
        content += ` */\n\n`;
        
        content += `typedef struct _STRUCT_MOB {\n`;
        content += `    // Todo: Implement full MOB structure\n`;
        content += `    char Name[16];              // Nome do personagem/mob\n`;
        content += `    uint16_t MobId;             // ID único do mob/npc (0 para players)\n`;
        content += `    uint8_t  Class;             // Classe do personagem\n`;
        content += `    // ... outros campos ...\n`;
        content += `} STRUCT_MOB;\n\n`;
        
        return content;
    }
    
    /**
     * Gera headers C/C++ para estrutura de skills
     */
    _generateSkillHeaders(data) {
        // Similar à _generateItemHeaders mas para estrutura de skills
        // TODO: Implementar conforme os campos reais de SkillData.bin
        
        let content = '';
        
        content += `/**\n`;
        content += ` * Structure definition for WYD skills\n`;
        content += ` * Record size: ${data.skillSize} bytes\n`;
        content += ` * Valid records: ${data.validSkills} / ${data.totalSkills}\n`;
        content += ` */\n\n`;
        
        content += `typedef struct _STRUCT_SKILL {\n`;
        content += `    // Todo: Implement full SKILL structure\n`;
        content += `    uint16_t SkillId;           // ID da habilidade\n`;
        content += `    uint8_t  Level;             // Nível da habilidade\n`;
        content += `    uint8_t  Type;              // Tipo de habilidade\n`;
        content += `    // ... outros campos ...\n`;
        content += `} STRUCT_SKILL;\n\n`;
        
        return content;
    }
    
    /**
     * Converte buffer para representação hexadecimal formatada
     */
    _bytesToHex(buffer) {
        return Array.from(buffer).map(byte => byte.toString(16).padStart(2, '0')).join(' ');
    }
}

/**
 * Função principal de execução
 */
/**
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
        console.log('Uso: node reverse_bin_extractor.js <arquivo.bin> [--format=json|csv|headers] [--output=arquivo_saida]');
        console.log('Exemplo: node reverse_bin_extractor.js "WYD MODERN 2025 STUDIO/ItemList.bin" --format=json --output=itemlist.json');
        return;
    }
    
    // Parse de argumentos básico
    const filePath = args[0];
    let outputFormat = 'json';
    let outputPath = null;
    
    /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let i = 1; i < args.length; i++ Parâmetro let i = 1; i < args.length; i++
 */

    
    for(let i = 1; i < args.length; i++) {
        const arg = args[i];
        
        if (arg.startsWith('--format=')) {
            outputFormat = arg.split('=')[1];
        } else if (arg.startsWith('--output=')) {
            outputPath = arg.split('=')[1];
        }
    }
    
    // Se não foi especificado um caminho de saída, gera um com base no arquivo de entrada
    /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !outputPath Parâmetro !outputPath
 */

    if(!outputPath) {
        const ext = outputFormat === 'headers' ? 'h' : outputFormat;
        outputPath = `${filePath}.extracted.${ext}`;
    }
    
    console.log(`[INFO] Iniciando extração de ${filePath}`);
    console.log(`[INFO] Formato de saída: ${outputFormat}`);
    console.log(`[INFO] Arquivo de saída: ${outputPath}`);
    
    const extractor = new WYDBinaryExtractor({ outputFormat });
    extractor.extractData(filePath, outputPath);
}

// Executa se for chamado diretamente
/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param require.main === module Parâmetro require.main === module
 */

if(require.main === module) {
    main();
}

module.exports = WYDBinaryExtractor;
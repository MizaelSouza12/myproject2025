/**
 * WYD Protocol Analyzer
 * 
 * Ferramenta avançada para análise e decodificação do protocolo de rede do WYD.
 * Permite analisar tráfego capturado ou simular pacotes de rede para entender o protocolo.
 */

const fs = require('fs');
const crypto = require('crypto');

// Estrutura padrão de pacote (baseada no nosso packet_struct.h)
const PACKET_HEADER_SIZE = 12; // bytes
const MAX_PACKET_SIZE = 8192;  // tamanho máximo de um pacote (8KB)

// Tipos de pacotes conhecidos
const PACKET_TYPES = {
    0x101: 'LOGIN',
    0x102: 'LOGIN_RESPONSE',
    0x110: 'CREATE_CHARACTER',
    0x111: 'DELETE_CHARACTER',
    0x120: 'ENTER_WORLD',
    0x210: 'MOVEMENT',
    0x301: 'CHAT',
    0x401: 'STATS_UPDATE',
    0x501: 'ITEM_TRANSACTION',
    // Adicione outros tipos de pacotes conhecidos aqui
};

/**
 * Analisador de protocolo de rede do WYD
 */
/**
 * Classe WYDProtocolAnalyzer
 * 
 * Esta classe implementa a funcionalidade WYDProtocolAnalyzer conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDProtocolAnalyzer {
    /**
 * constructor
 * 
 * Implementa a funcionalidade constructor conforme especificação original.
 */

    constructor() {
        this.DEBUG = true;
        this.packetParsers = {
            'LOGIN': this._parseLoginPacket.bind(this),
            'LOGIN_RESPONSE': this._parseLoginResponsePacket.bind(this),
            'MOVEMENT': this._parseMovementPacket.bind(this),
            'CHAT': this._parseChatPacket.bind(this),
            'STATS_UPDATE': this._parseStatsUpdatePacket.bind(this),
            'ITEM_TRANSACTION': this._parseItemTransactionPacket.bind(this),
            // Adicione outros parsers específicos aqui
        };
    }

    /**
     * Analisa um arquivo de captura de pacotes ou buffer de dados
     * @param {Buffer|string} data Buffer ou caminho para arquivo de dados
     * @param {Object} options Opções de análise
     * @returns {Array} Array com pacotes analisados
     */
    analyzePackets(data, options = {}) {
        let buffer;
        
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param typeof data === 'string' Parâmetro typeof data === 'string'
 */

        
        if(typeof data === 'string') {
            // Se é um caminho de arquivo, lê o arquivo
            console.log(`[INFO] Lendo arquivo de pacotes: ${data}`);
            buffer = fs.readFileSync(data);
        } else if (Buffer.isBuffer(data)) {
            // Se já é um buffer, usa diretamente
            buffer = data;
        } else {
            throw new Error('Dados inválidos. Forneça um caminho de arquivo ou um buffer.');
        }
        
        console.log(`[INFO] Analisando ${buffer.length} bytes de dados de pacotes`);
        
        // Extrai os pacotes dos dados
        const packets = this._extractPackets(buffer, options);
        console.log(`[INFO] ${packets.length} pacotes extraídos`);
        
        // Analisa cada pacote individualmente
        const parsedPackets = [];
        /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let i = 0; i < packets.length; i++ Parâmetro let i = 0; i < packets.length; i++
 */

        for(let i = 0; i < packets.length; i++) {
            const packetInfo = this._parsePacket(packets[i]);
            parsedPackets.push(packetInfo);
            
            // Exibe informações sobre o pacote
            const packetType = packetInfo.type ? 
                (PACKET_TYPES[packetInfo.header.type] || `DESCONHECIDO_0x${packetInfo.header.type.toString(16)}`) : 
                'INVÁLIDO';
                
            console.log(`\n[INFO] Pacote #${i+1}:`);
            console.log(`  Tipo: ${packetType} (0x${packetInfo.header.type?.toString(16) || 'N/A'})`);
            console.log(`  Tamanho: ${packetInfo.header.size || 'N/A'} bytes`);
            console.log(`  Client ID: ${packetInfo.header.clientId || 'N/A'}`);
            
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param packetInfo.parsedData Parâmetro packetInfo.parsedData
 */

            
            if(packetInfo.parsedData) {
                console.log('  Dados decodificados:');
                this._printObject(packetInfo.parsedData, '    ');
            }
        }
        
        // Estatísticas básicas
        this._analyzeProtocolStats(parsedPackets);
        
        return parsedPackets;
    }
    
    /**
     * Gera pacotes de simulação para testar compatibilidade com o cliente
     * @param {string} packetType Tipo de pacote a gerar
     * @param {Object} packetData Dados para o pacote
     * @returns {Buffer} Pacote gerado
     */
    generatePacket(packetType, packetData) {
        console.log(`[INFO] Gerando pacote do tipo ${packetType}`);
        
        const packetTypeCode = Object.keys(PACKET_TYPES).find(key => PACKET_TYPES[key] === packetType);
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !packetTypeCode Parâmetro !packetTypeCode
 */

        if(!packetTypeCode) {
            throw new Error(`Tipo de pacote desconhecido: ${packetType}`);
        }
        
        // Gera o pacote específico com base no tipo
        let packetBuffer;
        /**
 * switch
 * 
 * Implementa a funcionalidade switch conforme especificação original.
 * @param packetType Parâmetro packetType
 */

        switch(packetType) {
            case 'LOGIN':
                packetBuffer = this._generateLoginPacket(packetData);
                break;
            case 'MOVEMENT':
                packetBuffer = this._generateMovementPacket(packetData);
                break;
            // Adicione outros geradores de pacotes específicos
            default:
                throw new Error(`Geração de pacote do tipo ${packetType} não implementada`);
        }
        
        console.log(`[INFO] Pacote gerado com sucesso (${packetBuffer.length} bytes)`);
        return packetBuffer;
    }
    
    /**
     * Extrai pacotes individuais de um stream de dados
     */
    _extractPackets(buffer, options) {
        const packets = [];
        let offset = 0;
        
        // Opções de extração
        const useChecksum = options.useChecksum !== false;
        const validateSize = options.validateSize !== false;
        
        /**
 * while
 * 
 * Implementa a funcionalidade while conforme especificação original.
 * @param offset + PACKET_HEADER_SIZE <= buffer.length Parâmetro offset + PACKET_HEADER_SIZE <= buffer.length
 */

        
        while(offset + PACKET_HEADER_SIZE <= buffer.length) {
            // Lê o cabeçalho do pacote
            const packetSize = buffer.readUInt16LE(offset);
            
            // Validação básica de tamanho
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param packetSize < PACKET_HEADER_SIZE || packetSize > MAX_PACKET_SIZE Parâmetro packetSize < PACKET_HEADER_SIZE || packetSize > MAX_PACKET_SIZE
 */

            if(packetSize < PACKET_HEADER_SIZE || packetSize > MAX_PACKET_SIZE) {
                console.log(`[AVISO] Tamanho de pacote inválido em offset ${offset}: ${packetSize}`);
                offset += 2; // Avança apenas 2 bytes e tenta novamente
                continue;
            }
            
            // Verifica se o pacote completo está disponível
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param offset + packetSize > buffer.length Parâmetro offset + packetSize > buffer.length
 */

            if(offset + packetSize > buffer.length) {
                console.log(`[AVISO] Pacote incompleto em offset ${offset}: esperado ${packetSize}, disponível ${buffer.length - offset}`);
                break;
            }
            
            // Extrai o pacote completo
            const packet = buffer.slice(offset, offset + packetSize);
            packets.push(packet);
            
            // Avança para o próximo pacote
            offset += packetSize;
        }
        
        return packets;
    }
    
    /**
     * Analisa um pacote individual
     */
    _parsePacket(packet) {
        // Verifica tamanho mínimo
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param packet.length < PACKET_HEADER_SIZE Parâmetro packet.length < PACKET_HEADER_SIZE
 */

        if(packet.length < PACKET_HEADER_SIZE) {
            return { 
                valid: false, 
                error: `Pacote muito pequeno: ${packet.length} bytes`
            };
        }
        
        // Extrai cabeçalho
        const header = {
            size: packet.readUInt16LE(0),
            type: packet.readUInt16LE(2),
            checksum: packet.readUInt32LE(4),
            clientId: packet.readUInt32LE(8)
        };
        
        // Validação básica
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param header.size !== packet.length Parâmetro header.size !== packet.length
 */

        if(header.size !== packet.length) {
            return {
                valid: false,
                header,
                error: `Tamanho de pacote inconsistente: declarado ${header.size}, real ${packet.length}`
            };
        }
        
        // Extrai dados do pacote (após o cabeçalho)
        const data = packet.slice(PACKET_HEADER_SIZE);
        
        // Tenta decodificar o pacote com base no tipo
        const packetType = PACKET_TYPES[header.type];
        let parsedData = null;
        
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param packetType && this.packetParsers[packetType] Parâmetro packetType && this.packetParsers[packetType]
 */

        
        if(packetType && this.packetParsers[packetType]) {
            try {
                parsedData = this.packetParsers[packetType](data, header);
            } /**
 * catch
 * 
 * Implementa a funcionalidade catch conforme especificação original.
 * @param error Parâmetro error
 */
 catch(error) {
                console.log(`[ERRO] Falha ao decodificar pacote ${packetType}: ${error.message}`);
            }
        }
        
        return {
            valid: true,
            type: packetType,
            header,
            rawData: data,
            parsedData
        };
    }
    
    /**
     * Analisador para pacote de LOGIN
     */
    _parseLoginPacket(data, header) {
        // Exemplo de estrutura do pacote de LOGIN (conforme packet_struct.h)
        // char Username[16];
        // char Password[32];
        // uint32_t Version;
        // uint8_t MacAddress[6];
        // uint16_t Padding;
        
        const username = this._readNullTerminatedString(data, 0, 16);
        const password = this._readNullTerminatedString(data, 16, 32);
        const version = data.readUInt32LE(48);
        const macAddress = Array.from(data.slice(52, 58)).map(b => b.toString(16).padStart(2, '0')).join(':');
        
        return {
            username,
            password: '***REDACTED***', // Por segurança, não mostramos a senha
            version,
            macAddress,
            rawPassword: password // Apenas para debug, remover em produção
        };
    }
    
    /**
     * Analisador para pacote de resposta de LOGIN
     */
    _parseLoginResponsePacket(data, header) {
        // Exemplo de estrutura do pacote de LOGIN_RESPONSE
        // uint16_t Result;
        // uint32_t AccountId;
        // uint8_t AccessLevel;
        // uint8_t Reserved[11];
        
        const result = data.readUInt16LE(0);
        const accountId = data.readUInt32LE(2);
        const accessLevel = data.readUInt8(6);
        
        // Códigos de resultado conhecidos
        const resultCodes = {
            0: 'SUCCESS',
            1: 'INVALID_USERNAME',
            2: 'INVALID_PASSWORD',
            3: 'ACCOUNT_BANNED',
            4: 'ALREADY_LOGGED',
            5: 'SERVER_FULL',
            // Outros códigos
        };
        
        return {
            result,
            resultText: resultCodes[result] || `UNKNOWN_RESULT_${result}`,
            accountId,
            accessLevel
        };
    }
    
    /**
     * Analisador para pacote de MOVEMENT
     */
    _parseMovementPacket(data, header) {
        // Exemplo de estrutura do pacote de MOVEMENT
        // uint16_t PosX;
        // uint16_t PosY;
        // uint8_t Direction;
        // uint8_t Reserved[7];
        
        const posX = data.readUInt16LE(0);
        const posY = data.readUInt16LE(2);
        const direction = data.readUInt8(4);
        
        // Direções conhecidas
        const directions = {
            0: 'NORTH',
            1: 'NORTHEAST',
            2: 'EAST',
            3: 'SOUTHEAST',
            4: 'SOUTH',
            5: 'SOUTHWEST',
            6: 'WEST',
            7: 'NORTHWEST'
        };
        
        return {
            posX,
            posY,
            direction,
            directionText: directions[direction] || `UNKNOWN_DIRECTION_${direction}`
        };
    }
    
    /**
     * Analisador para pacote de CHAT
     */
    _parseChatPacket(data, header) {
        // Exemplo de estrutura do pacote de CHAT
        // uint8_t ChatType;
        // char ToName[16];
        // char Message[100];
        
        const chatType = data.readUInt8(0);
        const toName = this._readNullTerminatedString(data, 1, 16);
        const message = this._readNullTerminatedString(data, 17, 100);
        
        // Tipos de chat conhecidos
        const chatTypes = {
            0: 'ALL',
            1: 'WHISPER',
            2: 'GUILD',
            3: 'PARTY',
            4: 'SHOUT',
            5: 'SYSTEM',
            // Outros tipos
        };
        
        return {
            chatType,
            chatTypeText: chatTypes[chatType] || `UNKNOWN_CHAT_TYPE_${chatType}`,
            toName: chatType === 1 ? toName : null, // apenas mensagens privadas têm destinatário
            message
        };
    }
    
    /**
     * Analisador para pacote de STATS_UPDATE
     */
    _parseStatsUpdatePacket(data, header) {
        // Estrutura detalhada em packet_struct.h
        return {
            level: data.readUInt16LE(0),
            experience: data.readUInt32LE(2),
            strength: data.readUInt16LE(6),
            dexterity: data.readUInt16LE(8),
            intelligence: data.readUInt16LE(10),
            constitution: data.readUInt16LE(12),
            hp: data.readUInt32LE(14),
            maxHP: data.readUInt32LE(18),
            mp: data.readUInt32LE(22),
            maxMP: data.readUInt32LE(26),
            attack: data.readUInt16LE(30),
            defense: data.readUInt16LE(32),
            resistance: data.readUInt16LE(34),
            statPoints: data.readUInt16LE(36),
            skillPoints: data.readUInt16LE(38)
        };
    }
    
    /**
     * Analisador para pacote de ITEM_TRANSACTION
     */
    _parseItemTransactionPacket(data, header) {
        // Estrutura detalhada em packet_struct.h
        // Apenas parte inicial para simplificar
        const transactionType = data.readUInt8(0);
        const sourceSlot = data.readUInt16LE(1);
        const destSlot = data.readUInt16LE(3);
        const quantity = data.readUInt16LE(5);
        
        // A estrutura do item é mais complexa e está em item_struct.h
        // Aqui extraímos apenas alguns campos básicos
        const itemData = data.slice(7);
        const itemInfo = {
            index: itemData.readUInt16LE(0),
            itemId: itemData.readUInt16LE(2),
            type: itemData.readUInt8(4),
            subType: itemData.readUInt8(5),
            level: itemData.readUInt8(6)
        };
        
        // Tipos de transação conhecidos
        const transactionTypes = {
            0: 'DROP',
            1: 'PICKUP',
            2: 'MOVE',
            3: 'TRADE',
            4: 'BUY',
            5: 'SELL',
            // Outros tipos
        };
        
        return {
            transactionType,
            transactionTypeText: transactionTypes[transactionType] || `UNKNOWN_TRANSACTION_${transactionType}`,
            sourceSlot,
            destSlot,
            quantity,
            item: itemInfo
        };
    }
    
    /**
     * Gerador de pacote de LOGIN
     */
    _generateLoginPacket(data) {
        // Valida dados necessários
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !data.username || !data.password Parâmetro !data.username || !data.password
 */

        if(!data.username || !data.password) {
            throw new Error('Username e password são obrigatórios para pacote de LOGIN');
        }
        
        // Calcula tamanho total do pacote
        const packetSize = PACKET_HEADER_SIZE + 16 + 32 + 4 + 6 + 2; // cabeçalho + username + password + version + mac + padding
        
        // Cria buffer para o pacote
        const packet = Buffer.alloc(packetSize);
        
        // Preenche cabeçalho
        packet.writeUInt16LE(packetSize, 0); // size
        packet.writeUInt16LE(0x101, 2);      // type (LOGIN)
        packet.writeUInt32LE(0, 4);          // checksum (será calculado depois)
        packet.writeUInt32LE(data.clientId || 0, 8); // clientId
        
        // Preenche dados do pacote
        let offset = PACKET_HEADER_SIZE;
        
        // Username (16 bytes, terminado com NULL)
        this._writeNullTerminatedString(packet, data.username, offset, 16);
        offset += 16;
        
        // Password (32 bytes, terminado com NULL)
        this._writeNullTerminatedString(packet, data.password, offset, 32);
        offset += 32;
        
        // Version
        packet.writeUInt32LE(data.version || 100, offset);
        offset += 4;
        
        // MAC Address (6 bytes)
        const mac = data.macAddress || '00:00:00:00:00:00';
        const macBytes = mac.split(':').map(hex => parseInt(hex, 16));
        /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let i = 0; i < 6; i++ Parâmetro let i = 0; i < 6; i++
 */

        for(let i = 0; i < 6; i++) {
            packet.writeUInt8(macBytes[i] || 0, offset + i);
        }
        offset += 6;
        
        // Padding (2 bytes)
        packet.writeUInt16LE(0, offset);
        
        // Calcula checksum
        const checksum = this._calculateChecksum(packet);
        packet.writeUInt32LE(checksum, 4);
        
        return packet;
    }
    
    /**
     * Gerador de pacote de MOVEMENT
     */
    _generateMovementPacket(data) {
        // Valida dados necessários
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param data.posX === undefined || data.posY === undefined Parâmetro data.posX === undefined || data.posY === undefined
 */

        if(data.posX === undefined || data.posY === undefined) {
            throw new Error('posX e posY são obrigatórios para pacote de MOVEMENT');
        }
        
        // Calcula tamanho total do pacote
        const packetSize = PACKET_HEADER_SIZE + 2 + 2 + 1 + 7; // cabeçalho + posX + posY + direction + reserved
        
        // Cria buffer para o pacote
        const packet = Buffer.alloc(packetSize);
        
        // Preenche cabeçalho
        packet.writeUInt16LE(packetSize, 0); // size
        packet.writeUInt16LE(0x210, 2);      // type (MOVEMENT)
        packet.writeUInt32LE(0, 4);          // checksum (será calculado depois)
        packet.writeUInt32LE(data.clientId || 0, 8); // clientId
        
        // Preenche dados do pacote
        let offset = PACKET_HEADER_SIZE;
        
        // PosX (2 bytes)
        packet.writeUInt16LE(data.posX, offset);
        offset += 2;
        
        // PosY (2 bytes)
        packet.writeUInt16LE(data.posY, offset);
        offset += 2;
        
        // Direction (1 byte)
        packet.writeUInt8(data.direction || 0, offset);
        offset += 1;
        
        // Reserved (7 bytes)
        packet.fill(0, offset, offset + 7);
        
        // Calcula checksum
        const checksum = this._calculateChecksum(packet);
        packet.writeUInt32LE(checksum, 4);
        
        return packet;
    }
    
    /**
     * Calcula o checksum para um pacote
     * Nota: Este é um método de exemplo, o algoritmo real pode ser diferente
     */
    _calculateChecksum(packet) {
        // Faz backup do valor atual de checksum e zera para o cálculo
        const currentChecksum = packet.readUInt32LE(4);
        packet.writeUInt32LE(0, 4);
        
        // Algoritmo de exemplo - soma simples de todos os bytes
        let checksum = 0;
        /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let i = 0; i < packet.length; i++ Parâmetro let i = 0; i < packet.length; i++
 */

        for(let i = 0; i < packet.length; i++) {
            checksum = (checksum + packet[i]) % 0xFFFFFFFF;
        }
        
        // Restaura o valor original de checksum
        packet.writeUInt32LE(currentChecksum, 4);
        
        return checksum;
    }
    
    /**
     * Lê uma string terminada em NULL de um buffer
     */
    _readNullTerminatedString(buffer, offset, maxLength) {
        let end = offset;
        /**
 * while
 * 
 * Implementa a funcionalidade while conforme especificação original.
 * @param end < offset + maxLength && buffer[end] !== 0 Parâmetro end < offset + maxLength && buffer[end] !== 0
 */

        while(end < offset + maxLength && buffer[end] !== 0) {
            end++;
        }
        return buffer.slice(offset, end).toString('utf8');
    }
    
    /**
     * Escreve uma string terminada em NULL em um buffer
     */
    _writeNullTerminatedString(buffer, str, offset, maxLength) {
        const bytes = Buffer.from(str, 'utf8');
        const length = Math.min(bytes.length, maxLength - 1);
        bytes.copy(buffer, offset, 0, length);
        buffer.writeUInt8(0, offset + length); // Termina com NULL
    }
    
    /**
     * Analisa estatísticas do protocolo com base nos pacotes interpretados
     */
    _analyzeProtocolStats(parsedPackets) {
        console.log('\n[INFO] Estatísticas do protocolo:');
        
        // Conta os tipos de pacotes
        const typeCounts = {};
        let totalSize = 0;
        
        parsedPackets./**
 * forEach
 * 
 * Implementa a funcionalidade forEach conforme especificação original.
 * @param packet => {
            if (packet.valid Parâmetro packet => {
            if (packet.valid
 */
forEach(packet => {
            if (packet.valid) {
                const type = packet.type || 'UNKNOWN';
                typeCounts[type] = (typeCounts[type] || 0) + 1;
                totalSize += packet.header.size || 0;
            } else {
                typeCounts['INVALID'] = (typeCounts['INVALID'] || 0) + 1;
            }
        });
        
        // Exibe contagens por tipo
        console.log('  Distribuição de pacotes:');
        Object.keys(typeCounts).sort().forEach(type => {
            const count = typeCounts[type];
            const percent = ((count / parsedPackets.length) * 100).toFixed(1);
            console.log(`    ${type}: ${count} (${percent}%)`);
        });
        
        // Outras estatísticas úteis
        console.log(`  Total de pacotes: ${parsedPackets.length}`);
        console.log(`  Tamanho médio de pacote: ${(totalSize / parsedPackets.length).toFixed(1)} bytes`);
    }
    
    /**
     * Imprime objeto de forma formatada com indentação
     */
    _printObject(obj, indent = '') {
        for (const [key, value] of Object.entries(obj)) {
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param typeof value === 'object' && value !== null Parâmetro typeof value === 'object' && value !== null
 */

            if(typeof value === 'object' && value !== null) {
                console.log(`${indent}${key}:`);
                this._printObject(value, indent + '  ');
            } else {
                console.log(`${indent}${key}: ${value}`);
            }
        }
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
        console.log('Uso: node wyd_protocol_analyzer.js <caminho_do_arquivo>');
        console.log('Exemplo: node wyd_protocol_analyzer.js captured_packets.bin');
        return;
    }
    
    const filePath = args[0];
    const analyzer = new WYDProtocolAnalyzer();
    const parsedPackets = analyzer.analyzePackets(filePath);
    
    // Salva o resultado em um arquivo JSON
    const resultPath = `${filePath}.analysis.json`;
    fs.writeFileSync(resultPath, JSON.stringify(parsedPackets, null, 2));
    console.log(`\n[INFO] Análise completa. Resultados salvos em: ${resultPath}`);
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

module.exports = WYDProtocolAnalyzer;
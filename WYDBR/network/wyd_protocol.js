/**
 * WYD Protocol Implementation
 * 
 * Uma implementação completa do protocolo de rede WYD que pode ser usada
 * para analisar pacotes capturados e também criar pacotes válidos para
 * comunicação com o cliente e o servidor.
 */

const crypto = require('crypto');
const EventEmitter = require('events');
const net = require('net');

// Constantes do protocolo
const PACKET_HEADER_SIZE = 12;
const MAX_PACKET_SIZE = 8192;

// Tipos de pacotes conhecidos
const PACKET_TYPES = {
    // Autenticação
    LOGIN: 0x101,
    LOGIN_RESPONSE: 0x102,
    CREATE_CHAR: 0x110,
    DELETE_CHAR: 0x111,
    SELECT_CHAR: 0x112,
    CHARACTER_INFO: 0x113,
    
    // Em jogo
    ENTER_WORLD: 0x120,
    LEAVE_WORLD: 0x121,
    TELEPORT: 0x122,
    
    // Movimentação
    MOVEMENT: 0x210,
    MOVEMENT_WARP: 0x211,
    
    // Comunicação
    CHAT: 0x301,
    SHOUT: 0x302,
    WHISPER: 0x303,
    PARTY_CHAT: 0x304,
    GUILD_CHAT: 0x305,
    
    // Estatísticas
    STATS_UPDATE: 0x401,
    LEVEL_UP: 0x402,
    ATTRIBUTE_POINT: 0x403,
    SKILL_POINT: 0x404,
    
    // Inventário
    ITEM_TRANSACTION: 0x501,
    ITEM_USE: 0x502,
    ITEM_DROP: 0x503,
    ITEM_PICK: 0x504,
    ITEM_SELL: 0x505,
    ITEM_BUY: 0x506,
    ITEM_EQUIP: 0x507,
    ITEM_UNEQUIP: 0x508,
    
    // Combate
    ATTACK: 0x601,
    SKILL_USE: 0x602,
    DAMAGE: 0x603,
    HEAL: 0x604,
    BUFF: 0x605,
    DEBUFF: 0x606,
    
    // Grupo e guilda
    PARTY_INVITE: 0x701,
    PARTY_LEAVE: 0x702,
    PARTY_KICK: 0x703,
    GUILD_INVITE: 0x704,
    GUILD_LEAVE: 0x705,
    GUILD_KICK: 0x706,
    
    // Sistema
    SERVER_INFO: 0x901,
    SERVER_TIME: 0x902,
    SERVER_NOTICE: 0x903,
    PING: 0x904,
    PONG: 0x905,
    
    // Administração
    ADMIN_COMMAND: 0xA01,
    ADMIN_TELEPORT: 0xA02,
    ADMIN_SUMMON: 0xA03,
    ADMIN_KICK: 0xA04
};

// Inverte PACKET_TYPES para obter o nome a partir do código
const PACKET_NAMES = {};
for (const [name, code] of Object.entries(PACKET_TYPES)) {
    PACKET_NAMES[code] = name;
}

/**
 * Implementação do protocolo de rede WYD
 */
/**
 * Classe WYDProtocol
 * 
 * Esta classe implementa a funcionalidade WYDProtocol conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDProtocol extends EventEmitter {
    /**
 * constructor
 * 
 * Implementa a funcionalidade constructor conforme especificação original.
 * @param options = {} Parâmetro options = {}
 */

    constructor(options = {}) {
        super();
        
        this.options = {
            debug: false,
            version: 100,
            encryptionEnabled: true,
            encryptionKey: 'WYD_DEFAULT_KEY', // Chave de criptografia padrão
            useCRC32: true,
            useCompression: true,
            compressionThreshold: 1024, // Bytes acima dos quais comprime
            ...options
        };
        
        this.lastClientId = 0;
        this.connections = new Map();
        this.server = null;
        
        // Mapeamento de tipo de pacote para manipulador
        this._packetHandlers = {};
        
        // Registra manipuladores padrão
        this._registerDefaultHandlers();
    }
    
    /**
     * Inicia um servidor WYD na porta especificada
     * @param {number} port Porta para escutar
     */
    startServer(port) {
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param this.server Parâmetro this.server
 */

        if(this.server) {
            throw new Error('Servidor já está em execução');
        }
        
        this.server = net.createServer((socket) => {
            this._handleClientConnection(socket);
        });
        
        this.server.listen(port, () => {
            this._debug(`Servidor WYD iniciado na porta ${port}`);
            this.emit('server_started', { port });
        });
        
        this.server.on('error', (error) => {
            this._debug(`Erro no servidor: ${error.message}`);
            this.emit('server_error', { error });
        });
        
        return this.server;
    }
    
    /**
     * Conecta a um servidor WYD
     * @param {string} host Endereço do servidor
     * @param {number} port Porta do servidor
     * @returns {net.Socket} Socket conectado
     */
    connectToServer(host, port) {
        const socket = new net.Socket();
        
        socket.connect(port, host, () => {
            this._debug(`Conectado ao servidor WYD em ${host}:${port}`);
            this._handleClientConnection(socket);
            this.emit('connected', { host, port, socket });
        });
        
        socket.on('error', (error) => {
            this._debug(`Erro de conexão: ${error.message}`);
            this.emit('connection_error', { error });
        });
        
        return socket;
    }
    
    /**
     * Para o servidor WYD
     */
    stopServer() {
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param this.server Parâmetro this.server
 */

        if(this.server) {
            this.server.close(() => {
                this._debug('Servidor WYD parado');
                this.emit('server_stopped');
            });
            
            // Fecha todas as conexões
            for (const conn of this.connections.values()) {
                conn.socket.destroy();
            }
            
            this.connections.clear();
            this.server = null;
        }
    }
    
    /**
     * Envia um pacote para um cliente específico
     * @param {number|string} clientId ID do cliente ou socket
     * @param {number} packetType Tipo do pacote
     * @param {Object} data Dados do pacote
     */
    sendPacket(clientId, packetType, data = {}) {
        const connection = typeof clientId === 'object' ? 
            { socket: clientId, clientId: 0 } : this.connections.get(clientId);
            
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !connection Parâmetro !connection
 */

            
        if(!connection) {
            throw new Error(`Cliente não encontrado: ${clientId}`);
        }
        
        const { socket } = connection;
        
        // Cria o pacote
        const packet = this.createPacket(packetType, data, clientId);
        
        // Envia o pacote
        socket.write(packet);
        
        this._debug(`Pacote enviado para cliente ${clientId}: ${PACKET_NAMES[packetType] || packetType}`);
    }
    
    /**
     * Registra um manipulador de pacotes
     * @param {number} packetType Tipo do pacote
     * @param {Function} handler Função manipuladora
     */
    registerPacketHandler(packetType, handler) {
        this._packetHandlers[packetType] = handler;
        this._debug(`Manipulador registrado para pacote ${PACKET_NAMES[packetType] || packetType}`);
    }
    
    /**
     * Cria um pacote WYD
     * @param {number} packetType Tipo do pacote
     * @param {Object} data Dados do pacote
     * @param {number} clientId ID do cliente (opcional)
     * @returns {Buffer} Pacote serializado
     */
    createPacket(packetType, data = {}, clientId = 0) {
        // Seleciona o construtor de pacote baseado no tipo
        const packetBuilder = this._getPacketBuilder(packetType);
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !packetBuilder Parâmetro !packetBuilder
 */

        if(!packetBuilder) {
            throw new Error(`Tipo de pacote desconhecido: ${packetType}`);
        }
        
        // Constrói o payload do pacote
        const payload = packetBuilder(data);
        
        // Tamanho total do pacote: cabeçalho + payload
        const packetSize = PACKET_HEADER_SIZE + payload.length;
        
        // Cria o buffer para o pacote
        const packet = Buffer.alloc(packetSize);
        
        // Escreve o cabeçalho
        packet.writeUInt16LE(packetSize, 0); // Size
        packet.writeUInt16LE(packetType, 2); // Type
        packet.writeUInt32LE(0, 4); // Checksum (preenchido depois)
        packet.writeUInt32LE(clientId, 8); // ClientId
        
        // Copia o payload para o pacote
        payload.copy(packet, PACKET_HEADER_SIZE);
        
        // Calcula o checksum se necessário
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param this.options.useCRC32 Parâmetro this.options.useCRC32
 */

        if(this.options.useCRC32) {
            const checksum = this._calculateChecksum(packet);
            packet.writeUInt32LE(checksum, 4);
        }
        
        // Aplica criptografia se necessário
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param this.options.encryptionEnabled Parâmetro this.options.encryptionEnabled
 */

        if(this.options.encryptionEnabled) {
            return this._encryptPacket(packet);
        }
        
        return packet;
    }
    
    /**
     * Analisa um pacote binário
     * @param {Buffer} packet Pacote a ser analisado
     * @returns {Object} Pacote decodificado
     */
    parsePacket(packet) {
        // Se a criptografia estiver habilitada, descriptografa primeiro
        const decryptedPacket = this.options.encryptionEnabled ? 
            this._decryptPacket(packet) : packet;
        
        // Verifica tamanho mínimo
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param decryptedPacket.length < PACKET_HEADER_SIZE Parâmetro decryptedPacket.length < PACKET_HEADER_SIZE
 */

        if(decryptedPacket.length < PACKET_HEADER_SIZE) {
            throw new Error(`Pacote muito pequeno: ${decryptedPacket.length} bytes`);
        }
        
        // Lê o cabeçalho
        const size = decryptedPacket.readUInt16LE(0);
        const type = decryptedPacket.readUInt16LE(2);
        const checksum = decryptedPacket.readUInt32LE(4);
        const clientId = decryptedPacket.readUInt32LE(8);
        
        // Verifica tamanho
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param size !== decryptedPacket.length Parâmetro size !== decryptedPacket.length
 */

        if(size !== decryptedPacket.length) {
            throw new Error(`Tamanho de pacote inválido: esperado ${size}, recebido ${decryptedPacket.length}`);
        }
        
        // Verifica checksum se necessário
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param this.options.useCRC32 Parâmetro this.options.useCRC32
 */

        if(this.options.useCRC32) {
            const calculatedChecksum = this._calculateChecksum(decryptedPacket);
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param checksum !== calculatedChecksum Parâmetro checksum !== calculatedChecksum
 */

            if(checksum !== calculatedChecksum) {
                throw new Error(`Checksum inválido: esperado ${checksum}, calculado ${calculatedChecksum}`);
            }
        }
        
        // Dados do pacote (após o cabeçalho)
        const payload = decryptedPacket.slice(PACKET_HEADER_SIZE);
        
        // Tenta analisar o payload baseado no tipo de pacote
        const parser = this._getPacketParser(type);
        
        // Objeto de pacote básico
        const result = {
            header: {
                size,
                type,
                checksum,
                clientId
            },
            typeName: PACKET_NAMES[type] || `UNKNOWN_${type.toString(16)}`,
            rawPayload: payload
        };
        
        // Se temos um parser, analisa o payload
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param parser Parâmetro parser
 */

        if(parser) {
            try {
                result.data = parser(payload);
            } /**
 * catch
 * 
 * Implementa a funcionalidade catch conforme especificação original.
 * @param error Parâmetro error
 */
 catch(error) {
                this._debug(`Erro ao analisar pacote ${result.typeName}: ${error.message}`);
                result.parseError = error.message;
            }
        } else {
            this._debug(`Nenhum parser disponível para pacote tipo ${result.typeName}`);
        }
        
        return result;
    }
    
    /**
     * Manipula uma nova conexão de cliente
     */
    _handleClientConnection(socket) {
        // Gera um ID para o cliente
        const clientId = ++this.lastClientId;
        
        // Armazena a conexão
        this.connections.set(clientId, {
            socket,
            clientId,
            address: socket.remoteAddress,
            port: socket.remotePort,
            connected: true,
            buffer: Buffer.alloc(0),
            authenticated: false,
            lastActivity: Date.now()
        });
        
        this._debug(`Nova conexão: Cliente ${clientId} (${socket.remoteAddress}:${socket.remotePort})`);
        this.emit('client_connected', { clientId, socket });
        
        // Configura manipuladores de eventos
        socket.on('data', (data) => {
            this._handleClientData(clientId, data);
        });
        
        socket.on('close', () => {
            this._handleClientDisconnect(clientId);
        });
        
        socket.on('error', (error) => {
            this._debug(`Erro em cliente ${clientId}: ${error.message}`);
            this.emit('client_error', { clientId, error });
        });
    }
    
    /**
     * Manipula dados recebidos de um cliente
     */
    _handleClientData(clientId, data) {
        const connection = this.connections.get(clientId);
        if (!connection) return;
        
        // Atualiza timestamp de atividade
        connection.lastActivity = Date.now();
        
        // Adiciona os dados recebidos ao buffer
        connection.buffer = Buffer.concat([connection.buffer, data]);
        
        // Processa todos os pacotes completos no buffer
        this._processPackets(clientId);
    }
    
    /**
     * Processa todos os pacotes completos no buffer
     */
    _processPackets(clientId) {
        const connection = this.connections.get(clientId);
        if (!connection) return;
        
        let offset = 0;
        
        /**
 * while
 * 
 * Implementa a funcionalidade while conforme especificação original.
 * @param offset + PACKET_HEADER_SIZE <= connection.buffer.length Parâmetro offset + PACKET_HEADER_SIZE <= connection.buffer.length
 */

        
        while(offset + PACKET_HEADER_SIZE <= connection.buffer.length) {
            // Lê tamanho do pacote (assumindo que não está criptografado)
            const size = connection.buffer.readUInt16LE(offset);
            
            // Validação básica de tamanho
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param size < PACKET_HEADER_SIZE || size > MAX_PACKET_SIZE Parâmetro size < PACKET_HEADER_SIZE || size > MAX_PACKET_SIZE
 */

            if(size < PACKET_HEADER_SIZE || size > MAX_PACKET_SIZE) {
                this._debug(`Cliente ${clientId}: Tamanho de pacote inválido: ${size}`);
                // Avança 1 byte e tenta novamente
                offset++;
                continue;
            }
            
            // Verifica se o pacote completo está disponível
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param offset + size > connection.buffer.length Parâmetro offset + size > connection.buffer.length
 */

            if(offset + size > connection.buffer.length) {
                // Não temos o pacote completo ainda
                break;
            }
            
            // Extrai o pacote completo
            const packetBuffer = connection.buffer.slice(offset, offset + size);
            
            try {
                // Analisa o pacote
                const packet = this.parsePacket(packetBuffer);
                
                // Envia para manipulação
                this._handlePacket(clientId, packet);
                
            } /**
 * catch
 * 
 * Implementa a funcionalidade catch conforme especificação original.
 * @param error Parâmetro error
 */
 catch(error) {
                this._debug(`Erro ao processar pacote do cliente ${clientId}: ${error.message}`);
                this.emit('packet_error', { clientId, error });
            }
            
            // Avança para o próximo pacote
            offset += size;
        }
        
        // Remove os pacotes processados do buffer
        connection.buffer = connection.buffer.slice(offset);
    }
    
    /**
     * Manipula um pacote analisado
     */
    _handlePacket(clientId, packet) {
        this._debug(`Pacote recebido do cliente ${clientId}: ${packet.typeName}`);
        this.emit('packet_received', { clientId, packet });
        
        // Verifica se temos um manipulador para este tipo de pacote
        const handler = this._packetHandlers[packet.header.type];
        
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param handler Parâmetro handler
 */

        
        if(handler) {
            // Chama o manipulador
            handler.call(this, clientId, packet);
        } else {
            this._debug(`Nenhum manipulador para pacote tipo ${packet.typeName}`);
        }
    }
    
    /**
     * Manipula a desconexão de um cliente
     */
    _handleClientDisconnect(clientId) {
        const connection = this.connections.get(clientId);
        if (!connection) return;
        
        this._debug(`Cliente ${clientId} desconectado`);
        
        // Remove a conexão
        this.connections.delete(clientId);
        
        // Notifica a desconexão
        this.emit('client_disconnected', { clientId });
    }
    
    /**
     * Registra os manipuladores padrão para tipos conhecidos de pacotes
     */
    _registerDefaultHandlers() {
        // LOGIN
        this.registerPacketHandler(PACKET_TYPES.LOGIN, (clientId, packet) => {
            const { username, password, version } = packet.data;
            
            this._debug(`Tentativa de login: ${username}, versão ${version}`);
            
            // Verifica versão
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param version < this.options.version Parâmetro version < this.options.version
 */

            if(version < this.options.version) {
                // Versão incompatível
                this.sendPacket(clientId, PACKET_TYPES.LOGIN_RESPONSE, {
                    result: 6, // Código de erro: versão antiga
                    resultMessage: 'Versão do cliente incompatível',
                    accountId: 0
                });
                return;
            }
            
            // Aqui você implementaria a verificação real de credenciais
            // Como é apenas um exemplo, qualquer login é aceito
            
            // Resposta de sucesso
            this.sendPacket(clientId, PACKET_TYPES.LOGIN_RESPONSE, {
                result: 0, // Sucesso
                resultMessage: 'Login bem-sucedido',
                accountId: 1000 + clientId, // ID de conta fictício
                accessLevel: 1 // Nível de acesso normal
            });
            
            // Marca o cliente como autenticado
            const connection = this.connections.get(clientId);
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param connection Parâmetro connection
 */

            if(connection) {
                connection.authenticated = true;
                connection.username = username;
            }
            
            this.emit('client_authenticated', { clientId, username });
        });
        
        // PING
        this.registerPacketHandler(PACKET_TYPES.PING, (clientId, packet) => {
            // Responde com PONG
            this.sendPacket(clientId, PACKET_TYPES.PONG, {
                timestamp: Math.floor(Date.now() / 1000)
            });
        });
        
        // MOVEMENT
        this.registerPacketHandler(PACKET_TYPES.MOVEMENT, (clientId, packet) => {
            const { posX, posY, direction } = packet.data;
            
            // Atualiza a posição do cliente na conexão
            const connection = this.connections.get(clientId);
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param connection Parâmetro connection
 */

            if(connection) {
                connection.posX = posX;
                connection.posY = posY;
                connection.direction = direction;
            }
            
            // Aqui você faria validações de movimento, colisão, etc.
            // E notificaria outros clientes próximos
            
            this.emit('client_moved', { clientId, posX, posY, direction });
        });
        
        // CHAT
        this.registerPacketHandler(PACKET_TYPES.CHAT, (clientId, packet) => {
            const { chatType, message } = packet.data;
            
            // Verifica se o cliente está autenticado
            const connection = this.connections.get(clientId);
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !connection || !connection.authenticated Parâmetro !connection || !connection.authenticated
 */

            if(!connection || !connection.authenticated) {
                return; // Ignora mensagens de clientes não autenticados
            }
            
            this._debug(`Chat de ${connection.username}: ${message}`);
            
            // Emite evento de chat
            this.emit('chat_message', {
                clientId,
                username: connection.username,
                type: chatType,
                message
            });
            
            // Aqui você distribuiria a mensagem para outros clientes
            // dependendo do tipo de chat (geral, grupo, etc.)
        });
    }
    
    /**
     * Obtém o construtor de pacote para o tipo especificado
     */
    _getPacketBuilder(packetType) {
        // Mapeamento de tipo de pacote para função de construção
        const builders = {
            // LOGIN
            [PACKET_TYPES.LOGIN]: (data) => {
                const buffer = Buffer.alloc(16 + 32 + 4 + 6 + 2); // username + password + version + mac + padding
                
                let offset = 0;
                
                // Username (16 bytes, null-terminated)
                this._writeString(buffer, data.username || '', offset, 16);
                offset += 16;
                
                // Password (32 bytes, null-terminated)
                this._writeString(buffer, data.password || '', offset, 32);
                offset += 32;
                
                // Version
                buffer.writeUInt32LE(data.version || this.options.version, offset);
                offset += 4;
                
                // MAC Address (6 bytes)
                /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param data.macAddress Parâmetro data.macAddress
 */

                if(data.macAddress) {
                    const macBytes = data.macAddress.split(':').map(hex => parseInt(hex, 16));
                    /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let i = 0; i < 6; i++ Parâmetro let i = 0; i < 6; i++
 */

                    for(let i = 0; i < 6; i++) {
                        buffer.writeUInt8(macBytes[i] || 0, offset + i);
                    }
                }
                offset += 6;
                
                // Padding
                buffer.writeUInt16LE(0, offset);
                
                return buffer;
            },
            
            [PACKET_TYPES.LOGIN_RESPONSE]: (data) => {
                const buffer = Buffer.alloc(2 + 4 + 1 + 11); // result + accountId + accessLevel + reserved
                
                buffer.writeUInt16LE(data.result || 0, 0);
                buffer.writeUInt32LE(data.accountId || 0, 2);
                buffer.writeUInt8(data.accessLevel || 0, 6);
                
                // Reserved bytes
                buffer.fill(0, 7, 18);
                
                return buffer;
            },
            
            // PING/PONG
            [PACKET_TYPES.PING]: (data) => {
                const buffer = Buffer.alloc(4);
                buffer.writeUInt32LE(data.timestamp || Math.floor(Date.now() / 1000), 0);
                return buffer;
            },
            
            [PACKET_TYPES.PONG]: (data) => {
                const buffer = Buffer.alloc(4);
                buffer.writeUInt32LE(data.timestamp || Math.floor(Date.now() / 1000), 0);
                return buffer;
            },
            
            // MOVEMENT
            [PACKET_TYPES.MOVEMENT]: (data) => {
                const buffer = Buffer.alloc(2 + 2 + 1 + 7); // posX + posY + direction + reserved
                
                buffer.writeUInt16LE(data.posX || 0, 0);
                buffer.writeUInt16LE(data.posY || 0, 2);
                buffer.writeUInt8(data.direction || 0, 4);
                
                // Reserved bytes
                buffer.fill(0, 5, 12);
                
                return buffer;
            },
            
            // CHAT
            [PACKET_TYPES.CHAT]: (data) => {
                const buffer = Buffer.alloc(1 + 16 + 100); // chatType + toName + message
                
                buffer.writeUInt8(data.chatType || 0, 0);
                this._writeString(buffer, data.toName || '', 1, 16);
                this._writeString(buffer, data.message || '', 17, 100);
                
                return buffer;
            }
            
            // Adicione construtores para outros tipos de pacotes conforme necessário
        };
        
        return builders[packetType];
    }
    
    /**
     * Obtém o parser de pacote para o tipo especificado
     */
    _getPacketParser(packetType) {
        // Mapeamento de tipo de pacote para função de análise
        const parsers = {
            // LOGIN
            [PACKET_TYPES.LOGIN]: (payload) => {
                return {
                    username: this._readString(payload, 0, 16),
                    password: this._readString(payload, 16, 32),
                    version: payload.readUInt32LE(48),
                    macAddress: this._formatMacAddress(payload.slice(52, 58))
                };
            },
            
            [PACKET_TYPES.LOGIN_RESPONSE]: (payload) => {
                return {
                    result: payload.readUInt16LE(0),
                    accountId: payload.readUInt32LE(2),
                    accessLevel: payload.readUInt8(6)
                };
            },
            
            // PING/PONG
            [PACKET_TYPES.PING]: (payload) => {
                return {
                    timestamp: payload.readUInt32LE(0)
                };
            },
            
            [PACKET_TYPES.PONG]: (payload) => {
                return {
                    timestamp: payload.readUInt32LE(0)
                };
            },
            
            // MOVEMENT
            [PACKET_TYPES.MOVEMENT]: (payload) => {
                return {
                    posX: payload.readUInt16LE(0),
                    posY: payload.readUInt16LE(2),
                    direction: payload.readUInt8(4)
                };
            },
            
            // CHAT
            [PACKET_TYPES.CHAT]: (payload) => {
                return {
                    chatType: payload.readUInt8(0),
                    toName: this._readString(payload, 1, 16),
                    message: this._readString(payload, 17, 100)
                };
            }
            
            // Adicione parsers para outros tipos de pacotes conforme necessário
        };
        
        return parsers[packetType];
    }
    
    /**
     * Lê uma string null-terminated de um buffer
     */
    _readString(buffer, offset, maxLength) {
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
     * Escreve uma string null-terminated em um buffer
     */
    _writeString(buffer, str, offset, maxLength) {
        if (!str) str = '';
        const bytes = Buffer.from(str, 'utf8');
        const length = Math.min(bytes.length, maxLength - 1);
        bytes.copy(buffer, offset, 0, length);
        buffer.writeUInt8(0, offset + length); // Termina com NULL
    }
    
    /**
     * Formata um buffer de endereço MAC
     */
    _formatMacAddress(buffer) {
        return Array.from(buffer).map(b => b.toString(16).padStart(2, '0')).join(':');
    }
    
    /**
     * Calcula o checksum de um pacote
     */
    _calculateChecksum(packet) {
        // Salva o checksum atual e zera-o para o cálculo
        const oldChecksum = packet.readUInt32LE(4);
        packet.writeUInt32LE(0, 4);
        
        // Calcula CRC32
        const checksum = this._calculateCRC32(packet);
        
        // Restaura o checksum original
        packet.writeUInt32LE(oldChecksum, 4);
        
        return checksum;
    }
    
    /**
     * Calcula CRC32 para um buffer
     */
    _calculateCRC32(buffer) {
        // Tabela de CRC32 pré-calculada
        const crcTable = [];
        /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let i = 0; i < 256; i++ Parâmetro let i = 0; i < 256; i++
 */

        for(let i = 0; i < 256; i++) {
            let crc = i;
            /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let j = 0; j < 8; j++ Parâmetro let j = 0; j < 8; j++
 */

            for(let j = 0; j < 8; j++) {
                crc = (crc & 1) ? ((crc >>> 1) ^ 0xEDB88320) : (crc >>> 1);
            }
            crcTable[i] = crc;
        }
        
        let crc = 0xFFFFFFFF;
        
        /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let i = 0; i < buffer.length; i++ Parâmetro let i = 0; i < buffer.length; i++
 */

        
        for(let i = 0; i < buffer.length; i++) {
            const byte = buffer[i];
            crc = (crc >>> 8) ^ crcTable[(crc ^ byte) & 0xFF];
        }
        
        return ~crc >>> 0;
    }
    
    /**
     * Criptografa um pacote
     */
    _encryptPacket(packet) {
        // Implementação simplificada com XOR usando a chave de criptografia
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !this.options.encryptionKey Parâmetro !this.options.encryptionKey
 */

        if(!this.options.encryptionKey) {
            return packet;
        }
        
        const key = Buffer.from(this.options.encryptionKey);
        const result = Buffer.alloc(packet.length);
        
        /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param let i = 0; i < packet.length; i++ Parâmetro let i = 0; i < packet.length; i++
 */

        
        for(let i = 0; i < packet.length; i++) {
            result[i] = packet[i] ^ key[i % key.length];
        }
        
        return result;
    }
    
    /**
     * Descriptografa um pacote
     */
    _decryptPacket(packet) {
        // A operação XOR é reversível, então usamos o mesmo método que encryptPacket
        return this._encryptPacket(packet);
    }
    
    /**
     * Log de depuração
     */
    _debug(message) {
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param this.options.debug Parâmetro this.options.debug
 */

        if(this.options.debug) {
            console.log(`[WYDProtocol] ${message}`);
        }
    }
}

/**
 * Função principal de execução (exemplo de uso)
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
    const mode = args[0] || 'client';
    const host = args[1] || 'localhost';
    const port = parseInt(args[2]) || 8000;
    
    const protocol = new WYDProtocol({ debug: true });
    
    /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param mode === 'server' Parâmetro mode === 'server'
 */

    
    if(mode === 'server') {
        // Exemplo de servidor
        protocol.startServer(port);
        
        protocol.on('client_connected', ({ clientId }) => {
            console.log(`Cliente ${clientId} conectado`);
        });
        
        protocol.on('client_authenticated', ({ clientId, username }) => {
            console.log(`Cliente ${clientId} autenticado como ${username}`);
        });
        
        protocol.on('chat_message', ({ clientId, username, message }) => {
            console.log(`[Chat] ${username}: ${message}`);
        });
        
    } else /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param mode === 'client' Parâmetro mode === 'client'
 */
 if(mode === 'client') {
        // Exemplo de cliente
        const socket = protocol.connectToServer(host, port);
        
        protocol.on('connected', () => {
            // Envia pacote de login
            protocol.sendPacket(socket, PACKET_TYPES.LOGIN, {
                username: 'testuser',
                password: 'testpass',
                version: 100
            });
        });
        
        protocol.on('packet_received', ({ packet }) => {
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param packet.header.type === PACKET_TYPES.LOGIN_RESPONSE Parâmetro packet.header.type === PACKET_TYPES.LOGIN_RESPONSE
 */

            if(packet.header.type === PACKET_TYPES.LOGIN_RESPONSE) {
                console.log(`Login response: ${packet.data.result === 0 ? 'SUCCESS' : 'FAILED'}`);
                
                /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param packet.data.result === 0 Parâmetro packet.data.result === 0
 */

                
                if(packet.data.result === 0) {
                    // Login bem-sucedido, envia um chat
                    protocol.sendPacket(socket, PACKET_TYPES.CHAT, {
                        chatType: 0, // Geral
                        message: 'Hello, world!'
                    });
                }
            }
        });
    } else {
        console.log('Modo desconhecido. Use: node wyd_protocol.js [server|client] [host] [port]');
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
    main();
}

module.exports = { WYDProtocol, PACKET_TYPES, PACKET_NAMES };
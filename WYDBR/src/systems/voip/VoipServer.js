/**
 * VoipServer Class
 * 
 * Implementa o servidor VOIP para comunicação peer-to-peer entre jogadores.
 * Gerencia as salas e as conexões de áudio, aplicando as regras de isolamento acústico.
 */
const WebSocket = require('ws');
const { v4: uuidv4 } = require('uuid');
const AudioRouter = require('./AudioRouter');
const AudioChannel = require('./AudioChannel');
const SignalingServer = require('./SignalingServer');

class VoipServer {
  constructor(options = {}) {
    this.options = {
      port: options.port || 8443,
      pingInterval: options.pingInterval || 30000,
      ...options
    };
    
    this.wss = null;
    this.clients = {};
    this.audioRouter = new AudioRouter();
    this.signaling = new SignalingServer();
    this.rooms = {};
    this.playerData = {};
    this.isRunning = false;
    
    console.log('Servidor VOIP sendo inicializado com as opções:', this.options);
  }
  
  // Inicializa o servidor
  start() {
    if (this.isRunning) {
      console.log('Servidor VOIP já está em execução');
      return false;
    }
    
    try {
      // Criar servidor WebSocket
      this.wss = new WebSocket.Server({ port: this.options.port });
      
      // Configurar eventos do servidor
      this.setupServerEvents();
      
      // Iniciar verificações periódicas
      this.startPeriodicChecks();
      
      this.isRunning = true;
      console.log(`Servidor VOIP iniciado na porta ${this.options.port}`);
      
      return true;
    } catch (error) {
      console.error('Erro ao iniciar servidor VOIP:', error);
      return false;
    }
  }
  
  // Configurar eventos do servidor
  setupServerEvents() {
    this.wss.on('connection', (ws, req) => {
      // Atribuir ID do cliente
      const clientId = uuidv4();
      ws.id = clientId;
      
      // Armazenar cliente
      this.clients[clientId] = {
        ws,
        ip: req.socket.remoteAddress,
        connectedAt: Date.now(),
        playerId: null,
        rooms: [],
        lastActivity: Date.now(),
        stats: {
          messagesSent: 0,
          messagesReceived: 0,
          bytesReceived: 0,
          bytesSent: 0
        }
      };
      
      console.log(`Nova conexão WebSocket: ${clientId} de ${req.socket.remoteAddress}`);
      
      // Configurar eventos do cliente
      this.setupClientEvents(ws, clientId);
    });
    
    this.wss.on('error', (error) => {
      console.error('Erro no servidor WebSocket:', error);
    });
    
    this.wss.on('close', () => {
      console.log('Servidor WebSocket encerrado');
      this.isRunning = false;
    });
  }
  
  // Configurar eventos do cliente
  setupClientEvents(ws, clientId) {
    const client = this.clients[clientId];
    
    ws.on('message', (message) => {
      try {
        // Atualizar estatísticas
        client.lastActivity = Date.now();
        client.stats.messagesReceived++;
        client.stats.bytesReceived += message.length || 0;
        
        // Processar mensagem
        const data = JSON.parse(message);
        this.handleClientMessage(clientId, data);
      } catch (error) {
        console.error(`Erro ao processar mensagem de ${clientId}:`, error);
        this.sendErrorToClient(clientId, 'invalid_message', 'Mensagem inválida');
      }
    });
    
    ws.on('close', () => {
      console.log(`Cliente desconectado: ${clientId}`);
      this.handleClientDisconnect(clientId);
    });
    
    ws.on('error', (error) => {
      console.error(`Erro no cliente ${clientId}:`, error);
    });
    
    // Enviar mensagem de boas-vindas
    this.sendToClient(clientId, {
      type: 'welcome',
      clientId: clientId,
      timestamp: Date.now(),
      serverVersion: '1.0.0'
    });
  }
  
  // Processar mensagem do cliente
  handleClientMessage(clientId, data) {
    const client = this.clients[clientId];
    
    if (!client) {
      console.error(`Mensagem recebida de cliente desconhecido: ${clientId}`);
      return;
    }
    
    // Verificar tipo de mensagem
    switch (data.type) {
      case 'auth':
        this.handleAuthMessage(clientId, data);
        break;
        
      case 'join_room':
        this.handleJoinRoomMessage(clientId, data);
        break;
        
      case 'leave_room':
        this.handleLeaveRoomMessage(clientId, data);
        break;
        
      case 'create_room':
        this.handleCreateRoomMessage(clientId, data);
        break;
        
      case 'signaling':
        this.handleSignalingMessage(clientId, data);
        break;
        
      case 'position_update':
        this.handlePositionUpdateMessage(clientId, data);
        break;
        
      case 'ping':
        this.handlePingMessage(clientId, data);
        break;
        
      default:
        console.warn(`Tipo de mensagem desconhecido de ${clientId}: ${data.type}`);
        this.sendErrorToClient(clientId, 'unknown_message_type', 'Tipo de mensagem desconhecido');
    }
  }
  
  // Processar mensagem de autenticação
  handleAuthMessage(clientId, data) {
    const client = this.clients[clientId];
    
    if (!data.playerId) {
      this.sendErrorToClient(clientId, 'auth_failed', 'ID do jogador ausente');
      return;
    }
    
    // Verificar se o jogador já está conectado
    for (const [existingClientId, existingClient] of Object.entries(this.clients)) {
      if (existingClientId !== clientId && 
          existingClient.playerId === data.playerId) {
        
        // Jogador já está conectado, desconectar sessão anterior
        console.log(`Jogador ${data.playerId} já está conectado, desconectando sessão anterior: ${existingClientId}`);
        this.sendToClient(existingClientId, {
          type: 'forced_disconnect',
          reason: 'new_session',
          message: 'Sua conta foi conectada em outro lugar'
        });
        
        if (existingClient.ws && existingClient.ws.readyState === WebSocket.OPEN) {
          existingClient.ws.close();
        }
        
        delete this.clients[existingClientId];
      }
    }
    
    // Atualizar informações do cliente
    client.playerId = data.playerId;
    client.playerName = data.playerName || `Player:${data.playerId}`;
    
    // Inicializar dados do jogador se não existirem
    if (!this.playerData[data.playerId]) {
      this.playerData[data.playerId] = {
        id: data.playerId,
        name: client.playerName,
        position: data.position || { x: 0, y: 0, z: 0 },
        rooms: [],
        online: true,
        lastSeen: Date.now()
      };
    } else {
      // Atualizar dados existentes
      this.playerData[data.playerId].online = true;
      this.playerData[data.playerId].lastSeen = Date.now();
      this.playerData[data.playerId].name = client.playerName;
      
      if (data.position) {
        this.playerData[data.playerId].position = data.position;
      }
    }
    
    // Adicionar ao canal de proximidade
    this.addPlayerToProximityChannel(data.playerId);
    
    // Enviar confirmação
    this.sendToClient(clientId, {
      type: 'auth_success',
      playerId: data.playerId,
      playerName: client.playerName,
      timestamp: Date.now()
    });
    
    console.log(`Jogador autenticado: ${data.playerId} (${client.playerName})`);
    
    // Notificar o jogador sobre salas e jogadores próximos
    this.sendRoomListToClient(clientId);
    this.updatePlayerProximityConnections(data.playerId);
  }
  
  // Adicionar jogador ao canal de proximidade
  addPlayerToProximityChannel(playerId) {
    // Verificar se o canal de proximidade padrão existe
    const proximityChannelId = 'proximity-default';
    let proximityChannel = this.rooms[proximityChannelId];
    
    if (!proximityChannel) {
      // Criar canal de proximidade se não existir
      proximityChannel = new AudioChannel(
        proximityChannelId,
        'proximity',
        []
      );
      
      this.rooms[proximityChannelId] = proximityChannel;
      this.audioRouter.addChannel(proximityChannel);
    }
    
    // Adicionar jogador ao canal
    proximityChannel.addMember(playerId);
    
    // Atualizar dados do jogador
    const playerData = this.playerData[playerId];
    if (playerData && !playerData.rooms.includes(proximityChannelId)) {
      playerData.rooms.push(proximityChannelId);
    }
    
    console.log(`Jogador ${playerId} adicionado ao canal de proximidade`);
  }
  
  // Processar mensagem de entrada em sala
  handleJoinRoomMessage(clientId, data) {
    const client = this.clients[clientId];
    
    if (!client.playerId) {
      this.sendErrorToClient(clientId, 'not_authenticated', 'Autenticação necessária');
      return;
    }
    
    const roomId = data.roomId;
    if (!roomId || !this.rooms[roomId]) {
      this.sendErrorToClient(clientId, 'room_not_found', 'Sala não encontrada');
      return;
    }
    
    const room = this.rooms[roomId];
    
    // Adicionar jogador à sala
    if (room.addMember(client.playerId)) {
      // Atualizar dados do jogador
      const playerData = this.playerData[client.playerId];
      if (playerData && !playerData.rooms.includes(roomId)) {
        playerData.rooms.push(roomId);
      }
      
      // Adicionar sala ao cliente
      if (!client.rooms.includes(roomId)) {
        client.rooms.push(roomId);
      }
      
      // Notificar cliente
      this.sendToClient(clientId, {
        type: 'room_joined',
        roomId: roomId,
        roomName: room.name || roomId,
        members: room.members.map(memberId => ({
          id: memberId,
          name: this.playerData[memberId] ? this.playerData[memberId].name : memberId
        }))
      });
      
      // Notificar outros membros da sala
      this.notifyRoomMembers(roomId, client.playerId, 'player_joined_room', {
        playerId: client.playerId,
        playerName: client.playerName
      });
      
      console.log(`Jogador ${client.playerId} entrou na sala ${roomId}`);
      
      // Atualizar conexões de áudio
      this.updateRoomConnections(roomId);
    } else {
      this.sendErrorToClient(clientId, 'already_in_room', 'Jogador já está na sala');
    }
  }
  
  // Processar mensagem de saída de sala
  handleLeaveRoomMessage(clientId, data) {
    const client = this.clients[clientId];
    
    if (!client.playerId) {
      this.sendErrorToClient(clientId, 'not_authenticated', 'Autenticação necessária');
      return;
    }
    
    const roomId = data.roomId;
    if (!roomId || !this.rooms[roomId]) {
      this.sendErrorToClient(clientId, 'room_not_found', 'Sala não encontrada');
      return;
    }
    
    const room = this.rooms[roomId];
    
    // Salas de proximidade não podem ser deixadas
    if (room.type === 'proximity') {
      this.sendErrorToClient(clientId, 'cannot_leave_proximity', 'Não é possível sair de salas de proximidade');
      return;
    }
    
    // Remover jogador da sala
    if (room.removeMember(client.playerId)) {
      // Atualizar dados do jogador
      const playerData = this.playerData[client.playerId];
      if (playerData) {
        const index = playerData.rooms.indexOf(roomId);
        if (index !== -1) {
          playerData.rooms.splice(index, 1);
        }
      }
      
      // Remover sala do cliente
      const clientRoomIndex = client.rooms.indexOf(roomId);
      if (clientRoomIndex !== -1) {
        client.rooms.splice(clientRoomIndex, 1);
      }
      
      // Notificar cliente
      this.sendToClient(clientId, {
        type: 'room_left',
        roomId: roomId
      });
      
      // Notificar outros membros da sala
      this.notifyRoomMembers(roomId, client.playerId, 'player_left_room', {
        playerId: client.playerId,
        playerName: client.playerName
      });
      
      console.log(`Jogador ${client.playerId} saiu da sala ${roomId}`);
      
      // Se a sala ficou vazia e não é de proximidade, removê-la
      if (room.members.length === 0 && room.type !== 'proximity') {
        delete this.rooms[roomId];
        this.audioRouter.removeChannel(roomId);
        console.log(`Sala removida: ${roomId}`);
      } else {
        // Atualizar conexões de áudio
        this.updateRoomConnections(roomId);
      }
    } else {
      this.sendErrorToClient(clientId, 'not_in_room', 'Jogador não está na sala');
    }
  }
  
  // Processar mensagem de criação de sala
  handleCreateRoomMessage(clientId, data) {
    const client = this.clients[clientId];
    
    if (!client.playerId) {
      this.sendErrorToClient(clientId, 'not_authenticated', 'Autenticação necessária');
      return;
    }
    
    if (!data.roomName) {
      this.sendErrorToClient(clientId, 'invalid_room_name', 'Nome da sala é obrigatório');
      return;
    }
    
    // Criar ID para a sala
    const roomId = `room-${Date.now()}-${Math.floor(Math.random() * 10000)}`;
    
    // Definir tipo da sala
    const roomType = data.isGuildRoom ? 'guild' : 'group';
    
    // Criar sala
    const room = new AudioChannel(
      roomId,
      roomType,
      [client.playerId]
    );
    
    // Adicionar metadados
    room.name = data.roomName;
    room.createdAt = Date.now();
    room.createdBy = client.playerId;
    room.isPrivate = data.isPrivate || false;
    room.password = data.password || null;
    
    // Adicionar sala
    this.rooms[roomId] = room;
    this.audioRouter.addChannel(room);
    
    // Atualizar dados do jogador
    const playerData = this.playerData[client.playerId];
    if (playerData) {
      playerData.rooms.push(roomId);
    }
    
    // Adicionar sala ao cliente
    client.rooms.push(roomId);
    
    // Notificar cliente
    this.sendToClient(clientId, {
      type: 'room_created',
      roomId: roomId,
      roomName: room.name,
      roomType: roomType,
      isPrivate: room.isPrivate
    });
    
    console.log(`Sala criada: ${roomId} (${room.name}) por ${client.playerId}`);
    
    // Notificar todos os clientes conectados sobre a nova sala (se não for privada)
    if (!room.isPrivate) {
      this.broadcastRoomUpdate(roomId);
    }
  }
  
  // Processar mensagem de sinalização WebRTC
  handleSignalingMessage(clientId, data) {
    const client = this.clients[clientId];
    
    if (!client.playerId) {
      this.sendErrorToClient(clientId, 'not_authenticated', 'Autenticação necessária');
      return;
    }
    
    if (!data.target) {
      this.sendErrorToClient(clientId, 'missing_target', 'Destinatário não especificado');
      return;
    }
    
    const targetPlayerId = data.target;
    
    // Verificar se os jogadores podem se comunicar
    if (!this.audioRouter.canPlayerHear(client.playerId, targetPlayerId)) {
      this.sendErrorToClient(clientId, 'cannot_connect', 'Não é possível estabelecer conexão com o jogador');
      return;
    }
    
    // Encaminhar mensagem de sinalização
    let targetClientId = null;
    
    // Encontrar o cliente do jogador alvo
    for (const [id, c] of Object.entries(this.clients)) {
      if (c.playerId === targetPlayerId) {
        targetClientId = id;
        break;
      }
    }
    
    if (!targetClientId) {
      this.sendErrorToClient(clientId, 'target_not_found', 'Jogador alvo não está conectado');
      return;
    }
    
    // Encaminhar mensagem de sinalização
    this.sendToClient(targetClientId, {
      type: 'signaling',
      signalType: data.signalType,
      source: client.playerId,
      sourceName: client.playerName,
      payload: data.payload
    });
  }
  
  // Processar mensagem de atualização de posição
  handlePositionUpdateMessage(clientId, data) {
    const client = this.clients[clientId];
    
    if (!client.playerId) {
      this.sendErrorToClient(clientId, 'not_authenticated', 'Autenticação necessária');
      return;
    }
    
    if (!data.position || 
        typeof data.position.x !== 'number' || 
        typeof data.position.y !== 'number' || 
        typeof data.position.z !== 'number') {
      this.sendErrorToClient(clientId, 'invalid_position', 'Posição inválida');
      return;
    }
    
    const { x, y, z } = data.position;
    
    // Atualizar posição do jogador
    if (this.playerData[client.playerId]) {
      this.playerData[client.playerId].position = { x, y, z };
    }
    
    // Atualizar posição no router de áudio
    this.audioRouter.updatePlayerPosition(client.playerId, x, y, z);
    
    // Atualizar conexões de proximidade
    this.updatePlayerProximityConnections(client.playerId);
  }
  
  // Processar mensagem de ping
  handlePingMessage(clientId, data) {
    this.sendToClient(clientId, {
      type: 'pong',
      timestamp: Date.now(),
      echo: data.timestamp
    });
  }
  
  // Atualizar conexões de proximidade de um jogador
  updatePlayerProximityConnections(playerId) {
    const playerData = this.playerData[playerId];
    if (!playerData) return;
    
    // Verificar se o jogador está em algum grupo
    const isInGroup = this.audioRouter.isPlayerInGroup(playerId);
    
    // Se estiver em grupo, não se conecta por proximidade
    if (isInGroup) return;
    
    // Obter jogadores próximos
    const nearbyPlayers = this.audioRouter.getNearbyPlayers(playerId);
    
    // Filtrar apenas jogadores que não estão em grupos
    const connectablePlayers = nearbyPlayers.filter(player => 
      !this.audioRouter.isPlayerInGroup(player.playerId)
    );
    
    // Notificar o jogador sobre jogadores próximos
    this.sendToPlayerByPlayerId(playerId, {
      type: 'nearby_players',
      players: connectablePlayers.map(player => ({
        id: player.playerId,
        name: this.playerData[player.playerId] ? this.playerData[player.playerId].name : player.playerId,
        distance: player.distance
      }))
    });
    
    // Para cada jogador próximo, verificar se é necessário iniciar conexão
    for (const player of connectablePlayers) {
      // Verificar se ambos estão online
      const otherPlayerData = this.playerData[player.playerId];
      if (!otherPlayerData || !otherPlayerData.online) continue;
      
      // Iniciar processo de sinalização entre os jogadores
      this.signaling.initiateConnection(playerId, player.playerId);
    }
  }
  
  // Atualizar conexões de uma sala
  updateRoomConnections(roomId) {
    const room = this.rooms[roomId];
    if (!room) return;
    
    // Atualizar conexões no canal de áudio
    room.updateConnections();
    
    // Para cada conexão na sala, verificar se é necessário iniciar sinalização
    for (const connectionId in room.activeConnections) {
      const connection = room.activeConnections[connectionId];
      const [player1, player2] = connection.players;
      
      // Verificar se ambos estão online
      const player1Data = this.playerData[player1];
      const player2Data = this.playerData[player2];
      
      if (!player1Data || !player1Data.online || !player2Data || !player2Data.online) {
        continue;
      }
      
      // Iniciar processo de sinalização entre os jogadores
      this.signaling.initiateConnection(player1, player2);
    }
  }
  
  // Notificar membros de uma sala sobre um evento
  notifyRoomMembers(roomId, excludePlayerId, eventType, data) {
    const room = this.rooms[roomId];
    if (!room) return;
    
    for (const memberId of room.members) {
      if (memberId === excludePlayerId) continue;
      
      this.sendToPlayerByPlayerId(memberId, {
        type: eventType,
        roomId: roomId,
        ...data
      });
    }
  }
  
  // Enviar lista de salas para um cliente
  sendRoomListToClient(clientId) {
    const client = this.clients[clientId];
    if (!client) return;
    
    // Filtrar apenas salas não privadas ou salas do jogador
    const roomList = Object.values(this.rooms)
      .filter(room => !room.isPrivate || room.members.includes(client.playerId))
      .filter(room => room.type !== 'proximity') // Filtrar salas de proximidade
      .map(room => ({
        id: room.id,
        name: room.name || room.id,
        type: room.type,
        memberCount: room.members.length,
        isPrivate: room.isPrivate || false,
        isPasswordProtected: !!room.password
      }));
    
    this.sendToClient(clientId, {
      type: 'room_list',
      rooms: roomList
    });
  }
  
  // Enviar mensagem para um cliente
  sendToClient(clientId, data) {
    const client = this.clients[clientId];
    if (!client || !client.ws || client.ws.readyState !== WebSocket.OPEN) {
      return false;
    }
    
    try {
      const message = JSON.stringify(data);
      client.ws.send(message);
      
      // Atualizar estatísticas
      client.stats.messagesSent++;
      client.stats.bytesSent += message.length;
      
      return true;
    } catch (error) {
      console.error(`Erro ao enviar mensagem para ${clientId}:`, error);
      return false;
    }
  }
  
  // Enviar mensagem para um jogador pelo ID
  sendToPlayerByPlayerId(playerId, data) {
    for (const [clientId, client] of Object.entries(this.clients)) {
      if (client.playerId === playerId) {
        return this.sendToClient(clientId, data);
      }
    }
    return false;
  }
  
  // Enviar erro para um cliente
  sendErrorToClient(clientId, code, message) {
    return this.sendToClient(clientId, {
      type: 'error',
      code: code,
      message: message
    });
  }
  
  // Broadcast para todos os clientes
  broadcastToAll(data, excludeClientId = null) {
    for (const clientId in this.clients) {
      if (clientId !== excludeClientId) {
        this.sendToClient(clientId, data);
      }
    }
  }
  
  // Broadcast de atualização de sala
  broadcastRoomUpdate(roomId) {
    const room = this.rooms[roomId];
    if (!room || room.isPrivate) return;
    
    const roomData = {
      id: room.id,
      name: room.name || room.id,
      type: room.type,
      memberCount: room.members.length,
      isPasswordProtected: !!room.password
    };
    
    this.broadcastToAll({
      type: 'room_update',
      room: roomData
    });
  }
  
  // Lidar com desconexão de cliente
  handleClientDisconnect(clientId) {
    const client = this.clients[clientId];
    if (!client) return;
    
    // Se o cliente estava autenticado
    if (client.playerId) {
      // Atualizar status do jogador
      if (this.playerData[client.playerId]) {
        this.playerData[client.playerId].online = false;
        this.playerData[client.playerId].lastSeen = Date.now();
      }
      
      // Remover jogador de todas as salas (exceto proximidade)
      const roomsToLeave = [...client.rooms];
      for (const roomId of roomsToLeave) {
        const room = this.rooms[roomId];
        if (!room) continue;
        
        // Não remover de salas de proximidade
        if (room.type === 'proximity') continue;
        
        // Remover jogador da sala
        room.removeMember(client.playerId);
        
        // Notificar outros membros da sala
        this.notifyRoomMembers(roomId, client.playerId, 'player_left_room', {
          playerId: client.playerId,
          playerName: client.playerName,
          reason: 'disconnected'
        });
        
        // Se a sala ficou vazia e não é de proximidade, removê-la
        if (room.members.length === 0 && room.type !== 'proximity') {
          delete this.rooms[roomId];
          this.audioRouter.removeChannel(roomId);
          console.log(`Sala removida: ${roomId}`);
        } else {
          // Atualizar conexões de áudio
          this.updateRoomConnections(roomId);
        }
      }
      
      console.log(`Jogador desconectado: ${client.playerId} (${client.playerName})`);
    }
    
    // Remover cliente
    delete this.clients[clientId];
  }
  
  // Iniciar verificações periódicas
  startPeriodicChecks() {
    // Ping para manter conexões ativas
    setInterval(() => {
      this.checkActiveConnections();
    }, this.options.pingInterval);
    
    // Limpeza de dados antigos
    setInterval(() => {
      this.cleanupOldData();
    }, 300000); // A cada 5 minutos
  }
  
  // Verificar conexões ativas
  checkActiveConnections() {
    const now = Date.now();
    const timeout = 2 * this.options.pingInterval; // Tempo limite: 2x intervalo de ping
    
    for (const [clientId, client] of Object.entries(this.clients)) {
      // Verificar se o cliente está inativo
      if (now - client.lastActivity > timeout) {
        console.log(`Cliente ${clientId} inativo por mais de ${timeout}ms, desconectando...`);
        
        if (client.ws && client.ws.readyState === WebSocket.OPEN) {
          client.ws.close();
        }
        
        this.handleClientDisconnect(clientId);
      }
    }
  }
  
  // Limpar dados antigos
  cleanupOldData() {
    const now = Date.now();
    const oldDataThreshold = 24 * 60 * 60 * 1000; // 24 horas
    
    let cleanedEntries = 0;
    
    // Limpar dados de jogadores offline por mais de 24 horas
    for (const [playerId, playerData] of Object.entries(this.playerData)) {
      if (!playerData.online && now - playerData.lastSeen > oldDataThreshold) {
        delete this.playerData[playerId];
        cleanedEntries++;
      }
    }
    
    if (cleanedEntries > 0) {
      console.log(`Limpeza de dados: ${cleanedEntries} entradas antigas removidas`);
    }
  }
  
  // Obter estatísticas do servidor
  getStats() {
    const clientCount = Object.keys(this.clients).length;
    const authenticatedCount = Object.values(this.clients).filter(c => c.playerId).length;
    const roomCount = Object.keys(this.rooms).length;
    
    return {
      uptime: this.isRunning ? Date.now() - this._startTime : 0,
      clientCount,
      authenticatedCount,
      roomCount,
      playerCount: Object.values(this.playerData).filter(p => p.online).length
    };
  }
  
  // Parar o servidor
  stop() {
    if (!this.isRunning) return true;
    
    try {
      // Fechar todas as conexões
      for (const clientId in this.clients) {
        const client = this.clients[clientId];
        if (client.ws && client.ws.readyState === WebSocket.OPEN) {
          client.ws.close();
        }
      }
      
      // Fechar servidor
      this.wss.close();
      
      this.isRunning = false;
      console.log('Servidor VOIP encerrado');
      
      return true;
    } catch (error) {
      console.error('Erro ao encerrar servidor VOIP:', error);
      return false;
    }
  }
}

module.exports = VoipServer;
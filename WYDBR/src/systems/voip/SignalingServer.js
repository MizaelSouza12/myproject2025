/**
 * SignalingServer Class
 * 
 * Implementa o servidor de sinalização WebRTC para a comunicação VOIP.
 * Gerencia o estabelecimento de conexões peer-to-peer entre jogadores.
 */
const { v4: uuidv4 } = require('uuid');

class SignalingServer {
  constructor() {
    this.pendingConnections = {}; // Conexões em andamento
    this.activeConnections = {};  // Conexões ativas
    this.connectionAttempts = {}; // Tentativas de conexão
    
    console.log('Servidor de sinalização WebRTC inicializado');
  }
  
  // Iniciar processo de conexão entre dois jogadores
  initiateConnection(player1Id, player2Id) {
    // Ordenar IDs para criar identificador de conexão consistente
    const connectionId = [player1Id, player2Id].sort().join('-');
    
    // Verificar se já existe uma conexão ativa
    if (this.activeConnections[connectionId]) {
      return {
        status: 'already_connected',
        connectionId
      };
    }
    
    // Verificar se já existe uma tentativa de conexão em andamento
    if (this.pendingConnections[connectionId]) {
      // Verificar se a conexão está em andamento há muito tempo
      const pendingConnection = this.pendingConnections[connectionId];
      const now = Date.now();
      
      // Se a conexão está pendente há mais de 30 segundos, reiniciá-la
      if (now - pendingConnection.startTime > 30000) {
        delete this.pendingConnections[connectionId];
      } else {
        return {
          status: 'pending',
          connectionId,
          startTime: pendingConnection.startTime
        };
      }
    }
    
    // Criar nova conexão pendente
    const pendingConnection = {
      id: connectionId,
      startTime: Date.now(),
      players: [player1Id, player2Id],
      status: 'initiating',
      signalData: {
        [player1Id]: null,
        [player2Id]: null
      },
      messages: []
    };
    
    this.pendingConnections[connectionId] = pendingConnection;
    
    // Registrar tentativa de conexão
    if (!this.connectionAttempts[connectionId]) {
      this.connectionAttempts[connectionId] = {
        attempts: 0,
        lastAttempt: 0,
        success: false
      };
    }
    
    this.connectionAttempts[connectionId].attempts++;
    this.connectionAttempts[connectionId].lastAttempt = Date.now();
    
    console.log(`Iniciando conexão WebRTC entre ${player1Id} e ${player2Id}`);
    
    return {
      status: 'initiated',
      connectionId,
      startTime: pendingConnection.startTime
    };
  }
  
  // Processar mensagem de sinalização de um jogador
  processSignalMessage(playerId, targetId, signalType, signalData) {
    // Ordenar IDs para obter identificador de conexão
    const connectionId = [playerId, targetId].sort().join('-');
    
    // Verificar tipo de mensagem
    switch (signalType) {
      case 'offer':
        return this.processOfferMessage(connectionId, playerId, targetId, signalData);
        
      case 'answer':
        return this.processAnswerMessage(connectionId, playerId, targetId, signalData);
        
      case 'ice-candidate':
        return this.processIceCandidateMessage(connectionId, playerId, targetId, signalData);
        
      case 'connection-established':
        return this.processConnectionEstablishedMessage(connectionId, playerId, targetId);
        
      case 'connection-failed':
        return this.processConnectionFailedMessage(connectionId, playerId, targetId, signalData);
        
      default:
        console.warn(`Tipo de mensagem de sinalização desconhecido: ${signalType}`);
        return {
          status: 'error',
          error: 'unknown_signal_type',
          message: 'Tipo de mensagem de sinalização desconhecido'
        };
    }
  }
  
  // Processar mensagem de oferta
  processOfferMessage(connectionId, playerId, targetId, signalData) {
    let pendingConnection = this.pendingConnections[connectionId];
    
    // Se não houver conexão pendente, criar uma
    if (!pendingConnection) {
      pendingConnection = {
        id: connectionId,
        startTime: Date.now(),
        players: [playerId, targetId],
        status: 'offer_received',
        signalData: {
          [playerId]: signalData,
          [targetId]: null
        },
        messages: [{
          time: Date.now(),
          from: playerId,
          type: 'offer',
          data: signalData
        }]
      };
      
      this.pendingConnections[connectionId] = pendingConnection;
      console.log(`Nova oferta WebRTC recebida de ${playerId} para ${targetId}`);
    } else {
      // Atualizar dados da oferta
      pendingConnection.signalData[playerId] = signalData;
      pendingConnection.status = 'offer_received';
      pendingConnection.messages.push({
        time: Date.now(),
        from: playerId,
        type: 'offer',
        data: signalData
      });
      
      console.log(`Oferta WebRTC atualizada de ${playerId} para ${targetId}`);
    }
    
    return {
      status: 'offer_processed',
      connectionId
    };
  }
  
  // Processar mensagem de resposta
  processAnswerMessage(connectionId, playerId, targetId, signalData) {
    const pendingConnection = this.pendingConnections[connectionId];
    
    // Verificar se existe uma conexão pendente
    if (!pendingConnection) {
      console.warn(`Resposta WebRTC recebida para conexão inexistente: ${connectionId}`);
      return {
        status: 'error',
        error: 'no_pending_connection',
        message: 'Não há conexão pendente para esta resposta'
      };
    }
    
    // Verificar se houve uma oferta
    if (pendingConnection.status !== 'offer_received') {
      console.warn(`Resposta WebRTC recebida sem oferta prévia: ${connectionId}`);
      return {
        status: 'error',
        error: 'no_offer',
        message: 'Não há oferta para responder'
      };
    }
    
    // Atualizar dados da resposta
    pendingConnection.signalData[playerId] = signalData;
    pendingConnection.status = 'answer_received';
    pendingConnection.messages.push({
      time: Date.now(),
      from: playerId,
      type: 'answer',
      data: signalData
    });
    
    console.log(`Resposta WebRTC recebida de ${playerId} para ${targetId}`);
    
    return {
      status: 'answer_processed',
      connectionId
    };
  }
  
  // Processar mensagem de candidato ICE
  processIceCandidateMessage(connectionId, playerId, targetId, signalData) {
    const pendingConnection = this.pendingConnections[connectionId];
    
    // Verificar se existe uma conexão pendente
    if (!pendingConnection) {
      // Pode ser uma conexão já estabelecida recebendo candidatos tardios
      if (this.activeConnections[connectionId]) {
        console.log(`Candidato ICE tardio para conexão já estabelecida: ${connectionId}`);
        return {
          status: 'ice_candidate_for_active_connection',
          connectionId
        };
      }
      
      console.warn(`Candidato ICE recebido para conexão inexistente: ${connectionId}`);
      return {
        status: 'error',
        error: 'no_pending_connection',
        message: 'Não há conexão pendente para este candidato ICE'
      };
    }
    
    // Adicionar candidato ICE à lista de mensagens
    pendingConnection.messages.push({
      time: Date.now(),
      from: playerId,
      type: 'ice-candidate',
      data: signalData
    });
    
    return {
      status: 'ice_candidate_processed',
      connectionId
    };
  }
  
  // Processar mensagem de conexão estabelecida
  processConnectionEstablishedMessage(connectionId, playerId, targetId) {
    const pendingConnection = this.pendingConnections[connectionId];
    
    // Verificar se existe uma conexão pendente
    if (!pendingConnection) {
      console.warn(`Notificação de conexão estabelecida para conexão inexistente: ${connectionId}`);
      return {
        status: 'error',
        error: 'no_pending_connection',
        message: 'Não há conexão pendente para marcar como estabelecida'
      };
    }
    
    // Mover para conexões ativas
    this.activeConnections[connectionId] = {
      id: connectionId,
      players: pendingConnection.players,
      establishedTime: Date.now(),
      establishedBy: playerId,
      lastActivity: Date.now()
    };
    
    delete this.pendingConnections[connectionId];
    
    // Atualizar estatísticas
    if (this.connectionAttempts[connectionId]) {
      this.connectionAttempts[connectionId].success = true;
    }
    
    console.log(`Conexão WebRTC estabelecida entre ${pendingConnection.players[0]} e ${pendingConnection.players[1]}`);
    
    return {
      status: 'connection_established',
      connectionId
    };
  }
  
  // Processar mensagem de falha de conexão
  processConnectionFailedMessage(connectionId, playerId, targetId, signalData) {
    const pendingConnection = this.pendingConnections[connectionId];
    
    // Verificar se existe uma conexão pendente
    if (!pendingConnection) {
      console.warn(`Notificação de falha para conexão inexistente: ${connectionId}`);
      return {
        status: 'error',
        error: 'no_pending_connection',
        message: 'Não há conexão pendente para marcar como falha'
      };
    }
    
    // Registrar falha
    pendingConnection.status = 'failed';
    pendingConnection.failReason = signalData.reason || 'unknown';
    pendingConnection.failTime = Date.now();
    pendingConnection.failedBy = playerId;
    
    // Adicionar à lista de mensagens
    pendingConnection.messages.push({
      time: Date.now(),
      from: playerId,
      type: 'connection-failed',
      data: signalData
    });
    
    console.log(`Falha na conexão WebRTC entre ${pendingConnection.players[0]} e ${pendingConnection.players[1]}: ${pendingConnection.failReason}`);
    
    // Remover após registro de falha
    delete this.pendingConnections[connectionId];
    
    return {
      status: 'connection_failed',
      connectionId,
      reason: pendingConnection.failReason
    };
  }
  
  // Verificar status de uma conexão
  getConnectionStatus(connectionId) {
    // Verificar se é uma conexão ativa
    if (this.activeConnections[connectionId]) {
      return {
        status: 'active',
        connectionId,
        establishedTime: this.activeConnections[connectionId].establishedTime,
        players: this.activeConnections[connectionId].players
      };
    }
    
    // Verificar se é uma conexão pendente
    if (this.pendingConnections[connectionId]) {
      return {
        status: 'pending',
        connectionId,
        pendingState: this.pendingConnections[connectionId].status,
        startTime: this.pendingConnections[connectionId].startTime,
        players: this.pendingConnections[connectionId].players
      };
    }
    
    // Não encontrada
    return {
      status: 'not_found',
      connectionId
    };
  }
  
  // Encerrar uma conexão ativa
  terminateConnection(connectionId, reason = 'manual') {
    // Verificar se é uma conexão ativa
    if (this.activeConnections[connectionId]) {
      console.log(`Encerrando conexão WebRTC ativa: ${connectionId} (${reason})`);
      delete this.activeConnections[connectionId];
      return {
        status: 'terminated',
        connectionId,
        wasActive: true
      };
    }
    
    // Verificar se é uma conexão pendente
    if (this.pendingConnections[connectionId]) {
      console.log(`Encerrando conexão WebRTC pendente: ${connectionId} (${reason})`);
      delete this.pendingConnections[connectionId];
      return {
        status: 'terminated',
        connectionId,
        wasActive: false
      };
    }
    
    // Não encontrada
    return {
      status: 'not_found',
      connectionId
    };
  }
  
  // Verificar e limpar conexões pendentes antigas
  cleanupOldConnections() {
    const now = Date.now();
    const timeout = 60000; // 1 minuto
    let cleaned = 0;
    
    // Verificar conexões pendentes
    for (const connectionId in this.pendingConnections) {
      const connection = this.pendingConnections[connectionId];
      
      if (now - connection.startTime > timeout) {
        delete this.pendingConnections[connectionId];
        cleaned++;
      }
    }
    
    if (cleaned > 0) {
      console.log(`Limpeza de conexões: ${cleaned} conexões pendentes antigas removidas`);
    }
    
    return cleaned;
  }
  
  // Obter estatísticas de conexões
  getStats() {
    const activeCount = Object.keys(this.activeConnections).length;
    const pendingCount = Object.keys(this.pendingConnections).length;
    
    // Contar conexões por status
    const pendingStatuses = {};
    for (const connectionId in this.pendingConnections) {
      const status = this.pendingConnections[connectionId].status;
      pendingStatuses[status] = (pendingStatuses[status] || 0) + 1;
    }
    
    // Calcular taxa de sucesso
    const attempts = Object.values(this.connectionAttempts);
    const totalAttempts = attempts.length;
    const successfulAttempts = attempts.filter(a => a.success).length;
    const successRate = totalAttempts > 0 ? successfulAttempts / totalAttempts : 0;
    
    return {
      activeConnections: activeCount,
      pendingConnections: pendingCount,
      pendingStatuses,
      totalConnectionAttempts: totalAttempts,
      successfulConnections: successfulAttempts,
      successRate: successRate
    };
  }
}

module.exports = SignalingServer;
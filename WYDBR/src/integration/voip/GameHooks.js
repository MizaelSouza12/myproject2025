/**
 * GameHooks Class
 * 
 * Implementa os hooks para integrar o sistema VOIP com o jogo.
 * Captura eventos do jogo e atualiza o sistema VOIP conforme necessário.
 */
const VoipIntegration = require('../../systems/voip/VoipIntegration');

class GameHooks {
  constructor(gameEngine, options = {}) {
    this.gameEngine = gameEngine;
    this.voipIntegration = options.voipIntegration || new VoipIntegration();
    this.enabled = false;
    this.hooks = {};
    this.playerData = null;
    this.nearbyPlayers = [];
    this.positionUpdateInterval = null;
    this.updateInterval = options.updateInterval || 1000; // 1 segundo por padrão
    this.lastPosition = { x: 0, y: 0, z: 0 };
    this.minPositionDelta = options.minPositionDelta || 0.5; // Mudança mínima para atualizar
    
    console.log('GameHooks inicializado');
  }
  
  // Inicializa os hooks do jogo
  async initialize() {
    if (this.enabled) return true;
    
    try {
      // Verificar se o engine do jogo está disponível
      if (!this.gameEngine) {
        console.error('Engine do jogo não definido');
        return false;
      }
      
      // Obter dados do jogador atual
      this.playerData = this._getPlayerData();
      
      if (!this.playerData || !this.playerData.id) {
        console.error('Não foi possível obter dados do jogador');
        return false;
      }
      
      // Inicializar sistema VOIP com ID do jogador
      await this.voipIntegration.initialize(this.playerData.id);
      
      // Instalar hooks no engine do jogo
      this._installHooks();
      
      // Iniciar atualizações periódicas
      this._startPeriodicUpdates();
      
      this.enabled = true;
      console.log(`GameHooks inicializado para jogador ${this.playerData.id}`);
      
      return true;
    } catch (error) {
      console.error('Erro ao inicializar GameHooks:', error);
      return false;
    }
  }
  
  // Desativa os hooks
  shutdown() {
    if (!this.enabled) return true;
    
    // Remover hooks
    this._removeHooks();
    
    // Parar atualizações periódicas
    this._stopPeriodicUpdates();
    
    // Encerrar sistema VOIP
    this.voipIntegration.shutdown();
    
    this.enabled = false;
    console.log('GameHooks desativado');
    
    return true;
  }
  
  // Obtém dados do jogador atual
  _getPlayerData() {
    try {
      if (!this.gameEngine) return null;
      
      // Acesso aos dados do jogador no engine do jogo
      // Nomes de propriedades podem variar conforme implementação específica
      if (this.gameEngine.player) {
        return {
          id: this.gameEngine.player.id || this.gameEngine.player.uniqueId || 'player1',
          name: this.gameEngine.player.name || this.gameEngine.player.displayName || 'Player',
          position: this._getPlayerPosition()
        };
      } else if (this.gameEngine.localPlayer) {
        return {
          id: this.gameEngine.localPlayer.id || this.gameEngine.localPlayer.uniqueId || 'player1',
          name: this.gameEngine.localPlayer.name || this.gameEngine.localPlayer.displayName || 'Player',
          position: this._getPlayerPosition()
        };
      }
      
      // Se não conseguir obter dos lugares usuais, gerar ID temporário
      return {
        id: 'player_' + Math.floor(Math.random() * 10000),
        name: 'Player',
        position: { x: 0, y: 0, z: 0 }
      };
    } catch (error) {
      console.error('Erro ao obter dados do jogador:', error);
      
      // Retornar dados padrão em caso de erro
      return {
        id: 'player_' + Math.floor(Math.random() * 10000),
        name: 'Player',
        position: { x: 0, y: 0, z: 0 }
      };
    }
  }
  
  // Obtém a posição atual do jogador
  _getPlayerPosition() {
    try {
      if (!this.gameEngine) return { x: 0, y: 0, z: 0 };
      
      let position = { x: 0, y: 0, z: 0 };
      
      // Acessar posição do jogador no engine
      // Nomes de propriedades podem variar conforme implementação específica
      if (this.gameEngine.player && this.gameEngine.player.position) {
        position = {
          x: this.gameEngine.player.position.x || 0,
          y: this.gameEngine.player.position.y || 0,
          z: this.gameEngine.player.position.z || 0
        };
      } else if (this.gameEngine.localPlayer && this.gameEngine.localPlayer.position) {
        position = {
          x: this.gameEngine.localPlayer.position.x || 0,
          y: this.gameEngine.localPlayer.position.y || 0,
          z: this.gameEngine.localPlayer.position.z || 0
        };
      } else if (this.gameEngine.getPlayerPosition) {
        // Se existir método dedicado para obter posição
        const pos = this.gameEngine.getPlayerPosition();
        position = {
          x: pos.x || 0,
          y: pos.y || 0,
          z: pos.z || 0
        };
      }
      
      return position;
    } catch (error) {
      console.error('Erro ao obter posição do jogador:', error);
      return { x: 0, y: 0, z: 0 };
    }
  }
  
  // Obtém jogadores próximos
  _getNearbyPlayers() {
    try {
      if (!this.gameEngine) return [];
      
      let nearbyPlayers = [];
      
      // Tentar acessar lista de jogadores próximos no engine
      // Nomes de propriedades podem variar conforme implementação específica
      if (this.gameEngine.getNearbyPlayers) {
        nearbyPlayers = this.gameEngine.getNearbyPlayers();
      } else if (this.gameEngine.players) {
        // Se tiver lista de todos os jogadores, filtrar os próximos
        const position = this._getPlayerPosition();
        const proximityRadius = 15; // Raio de proximidade em unidades do jogo
        
        nearbyPlayers = this.gameEngine.players.filter(player => {
          if (!player.position || player.id === this.playerData.id) return false;
          
          // Calcular distância
          const distance = Math.sqrt(
            Math.pow(player.position.x - position.x, 2) +
            Math.pow(player.position.y - position.y, 2) +
            Math.pow(player.position.z - position.z, 2)
          );
          
          return distance <= proximityRadius;
        });
      }
      
      return nearbyPlayers.map(player => ({
        id: player.id || player.uniqueId,
        name: player.name || player.displayName || `Player-${player.id}`,
        position: {
          x: player.position.x || 0,
          y: player.position.y || 0,
          z: player.position.z || 0
        },
        distance: this._calculateDistance(
          this._getPlayerPosition(),
          {
            x: player.position.x || 0,
            y: player.position.y || 0,
            z: player.position.z || 0
          }
        )
      }));
    } catch (error) {
      console.error('Erro ao obter jogadores próximos:', error);
      return [];
    }
  }
  
  // Calcula distância entre duas posições
  _calculateDistance(pos1, pos2) {
    return Math.sqrt(
      Math.pow(pos1.x - pos2.x, 2) +
      Math.pow(pos1.y - pos2.y, 2) +
      Math.pow(pos1.z - pos2.z, 2)
    );
  }
  
  // Instala hooks no engine do jogo
  _installHooks() {
    // Hook para eventos de movimentação
    this._addHook('playerMove', this._onPlayerMove.bind(this));
    
    // Hook para eventos de entrada/saída de jogadores
    this._addHook('playerJoin', this._onPlayerJoin.bind(this));
    this._addHook('playerLeave', this._onPlayerLeave.bind(this));
    
    // Hook para eventos de grupo/partido
    this._addHook('partyJoin', this._onPartyJoin.bind(this));
    this._addHook('partyLeave', this._onPartyLeave.bind(this));
    
    // Hook para eventos de guilda
    this._addHook('guildMemberOnline', this._onGuildMemberStatusChange.bind(this));
    
    // Hook para eventos de área
    this._addHook('areaChanged', this._onAreaChanged.bind(this));
    
    console.log('Hooks instalados no engine do jogo');
  }
  
  // Adiciona um hook específico
  _addHook(eventName, handler) {
    if (!this.gameEngine || !this.gameEngine.on) return false;
    
    try {
      // Tentar se inscrever no evento
      this.gameEngine.on(eventName, handler);
      this.hooks[eventName] = handler;
      return true;
    } catch (error) {
      console.warn(`Erro ao adicionar hook para ${eventName}:`, error);
      return false;
    }
  }
  
  // Remove hooks instalados
  _removeHooks() {
    if (!this.gameEngine || !this.gameEngine.off) return;
    
    // Remover cada hook registrado
    for (const [eventName, handler] of Object.entries(this.hooks)) {
      try {
        this.gameEngine.off(eventName, handler);
      } catch (error) {
        console.warn(`Erro ao remover hook para ${eventName}:`, error);
      }
    }
    
    this.hooks = {};
    console.log('Hooks removidos do engine do jogo');
  }
  
  // Inicia atualizações periódicas
  _startPeriodicUpdates() {
    if (this.positionUpdateInterval) return;
    
    // Iniciar intervalo de atualização de posição
    this.positionUpdateInterval = setInterval(() => {
      this._updatePlayerPosition();
    }, this.updateInterval);
    
    console.log(`Atualizações periódicas iniciadas (intervalo: ${this.updateInterval}ms)`);
  }
  
  // Para atualizações periódicas
  _stopPeriodicUpdates() {
    if (this.positionUpdateInterval) {
      clearInterval(this.positionUpdateInterval);
      this.positionUpdateInterval = null;
      
      console.log('Atualizações periódicas interrompidas');
    }
  }
  
  // Atualiza a posição do jogador no sistema VOIP
  _updatePlayerPosition() {
    try {
      // Obter posição atual
      const position = this._getPlayerPosition();
      
      // Verificar se a posição mudou o suficiente para justificar uma atualização
      const delta = this._calculateDistance(position, this.lastPosition);
      
      if (delta >= this.minPositionDelta) {
        // Atualizar posição no VOIP
        this.voipIntegration.updatePlayerPosition(
          position.x,
          position.y,
          position.z
        );
        
        // Atualizar última posição
        this.lastPosition = { ...position };
        
        // Atualizar lista de jogadores próximos
        this._updateNearbyPlayers();
      }
    } catch (error) {
      console.error('Erro ao atualizar posição do jogador:', error);
    }
  }
  
  // Atualiza a lista de jogadores próximos
  _updateNearbyPlayers() {
    try {
      // Obter jogadores próximos do engine
      this.nearbyPlayers = this._getNearbyPlayers();
      
      // Hook para plugins ou extensões
      if (typeof this.onNearbyPlayersUpdated === 'function') {
        this.onNearbyPlayersUpdated(this.nearbyPlayers);
      }
    } catch (error) {
      console.error('Erro ao atualizar jogadores próximos:', error);
    }
  }
  
  // Handler para evento de movimento do jogador
  _onPlayerMove(data) {
    // Atualizar posição quando o jogador se move
    // (pode ser redundante com a atualização periódica, mas útil para eventos específicos)
    this._updatePlayerPosition();
  }
  
  // Handler para evento de entrada de jogador
  _onPlayerJoin(player) {
    // Atualizar lista de jogadores próximos
    this._updateNearbyPlayers();
  }
  
  // Handler para evento de saída de jogador
  _onPlayerLeave(player) {
    // Atualizar lista de jogadores próximos
    this._updateNearbyPlayers();
  }
  
  // Handler para evento de entrada em grupo/partido
  _onPartyJoin(partyData) {
    try {
      // Criar sala VOIP para o grupo, se não existir
      const partyId = partyData.id || `party_${Date.now()}`;
      const partyName = partyData.name || 'Grupo';
      
      // Criar sala para o grupo
      const room = this.voipIntegration.createRoom(partyName);
      
      console.log(`Sala VOIP criada para grupo: ${partyName} (ID: ${room.id})`);
      
      // Adicionar outros membros do grupo
      if (partyData.members && Array.isArray(partyData.members)) {
        // Processar após pequeno delay para garantir que a sala foi criada
        setTimeout(() => {
          partyData.members.forEach(member => {
            if (member.id !== this.playerData.id) {
              // Simular entrada do membro
              this._onPartyMemberJoin({
                partyId: partyId,
                memberId: member.id,
                memberName: member.name
              });
            }
          });
        }, 500);
      }
    } catch (error) {
      console.error('Erro ao processar entrada em grupo:', error);
    }
  }
  
  // Handler para evento de entrada de membro no grupo
  _onPartyMemberJoin(memberData) {
    // Pode ser chamado pelo sistema do jogo ou manualmente pelo _onPartyJoin
    console.log(`Membro entrou no grupo: ${memberData.memberName}`);
    
    // Idealmente, o sistema de jogo forneceria o ID da sala VOIP associada ao grupo
    // Como não temos essa informação, usaríamos uma tabela de mapeamento ou uma convenção de nomenclatura
  }
  
  // Handler para evento de saída de grupo
  _onPartyLeave(partyData) {
    try {
      // Sair da sala VOIP do grupo
      // Idealmente, o sistema de jogo forneceria o ID da sala VOIP associada ao grupo
      
      console.log(`Saiu do grupo: ${partyData.name || 'Grupo'}`);
      
      // Na ausência dessa informação, podemos verificar as salas ativas e encontrar a mais provável
      // baseado no nome ou membros
    } catch (error) {
      console.error('Erro ao processar saída de grupo:', error);
    }
  }
  
  // Handler para mudança de status de membro da guilda
  _onGuildMemberStatusChange(memberData) {
    // Atualizar status de amigos com base em membros da guilda
    console.log(`Status de membro da guilda alterado: ${memberData.name} (${memberData.status})`);
    
    // Idealmente, teríamos uma integração com o sistema de amigos do VOIP
  }
  
  // Handler para mudança de área/mapa
  _onAreaChanged(areaData) {
    // Atualizar quando o jogador muda de área/mapa
    console.log(`Área alterada: ${areaData.name || 'Desconhecida'}`);
    
    // Atualizar posição imediatamente
    this._updatePlayerPosition();
    
    // Atualizar jogadores próximos
    this._updateNearbyPlayers();
  }
  
  // Métodos públicos para manipulação manual
  
  // Cria uma sala VOIP manualmente
  createVoipRoom(roomName, isGuildRoom = false) {
    if (!this.enabled || !this.voipIntegration) return null;
    
    return this.voipIntegration.createRoom(roomName, isGuildRoom);
  }
  
  // Entra em uma sala VOIP manualmente
  joinVoipRoom(roomId) {
    if (!this.enabled || !this.voipIntegration) return false;
    
    return this.voipIntegration.joinRoom(roomId);
  }
  
  // Sai de uma sala VOIP manualmente
  leaveVoipRoom(roomId) {
    if (!this.enabled || !this.voipIntegration) return false;
    
    return this.voipIntegration.leaveRoom(roomId);
  }
  
  // Ativa/desativa microfone
  toggleMicrophone(active = null) {
    if (!this.enabled || !this.voipIntegration) return false;
    
    return this.voipIntegration.toggleMicrophone(active);
  }
  
  // Ativa/desativa áudio
  toggleAudio(active = null) {
    if (!this.enabled || !this.voipIntegration) return false;
    
    return this.voipIntegration.toggleAudio(active);
  }
  
  // Obtém lista de jogadores próximos
  getNearbyPlayers() {
    return [...this.nearbyPlayers];
  }
}

module.exports = GameHooks;
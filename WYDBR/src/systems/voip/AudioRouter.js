/**
 * AudioRouter Class
 * 
 * Gerencia o roteamento de áudio entre jogadores baseado em grupos e proximidade.
 * Implementa as regras de isolamento acústico entre jogadores.
 */
class AudioRouter {
  constructor() {
    this.channels = {};         // Todos os canais de áudio
    this.playerChannelMap = {}; // Mapeamento de jogadores para seus canais
    this.proximityDetector = new (require('./ProximityDetector'))();
    console.log('Router de áudio inicializado');
  }
  
  // Determina se um jogador pode ouvir outro
  canPlayerHear(listenerId, speakerId) {
    // Se ambos estão no mesmo grupo, podem se ouvir
    if (this.areInSameGroup(listenerId, speakerId)) {
      return true;
    }
    
    // Se algum deles está em um grupo, não podem se ouvir
    if (this.isPlayerInGroup(listenerId) || this.isPlayerInGroup(speakerId)) {
      return false;
    }
    
    // Se nenhum está em grupo, verificar proximidade
    return this.proximityDetector.arePlayersNearby(listenerId, speakerId);
  }
  
  // Verifica se jogadores estão no mesmo grupo
  areInSameGroup(player1Id, player2Id) {
    // Obter canais de cada jogador
    const player1Channels = this.getPlayerChannels(player1Id);
    const player2Channels = this.getPlayerChannels(player2Id);
    
    // Verificar se há algum canal em comum
    for (const channel1 of player1Channels) {
      for (const channel2 of player2Channels) {
        if (channel1 === channel2 && this.channels[channel1].type !== 'proximity') {
          return true;
        }
      }
    }
    
    return false;
  }
  
  // Verifica se jogador está em algum grupo (não proximidade)
  isPlayerInGroup(playerId) {
    const channels = this.getPlayerChannels(playerId);
    
    for (const channelId of channels) {
      if (this.channels[channelId].type !== 'proximity') {
        return true;
      }
    }
    
    return false;
  }
  
  // Obter canais de um jogador
  getPlayerChannels(playerId) {
    return this.playerChannelMap[playerId] || [];
  }
  
  // Adicionar canal
  addChannel(channel) {
    this.channels[channel.id] = channel;
    console.log(`Canal adicionado ao router: ${channel.id}`);
    
    // Atualizar mapeamento de jogadores
    for (const playerId of channel.members) {
      if (!this.playerChannelMap[playerId]) {
        this.playerChannelMap[playerId] = [];
      }
      
      if (!this.playerChannelMap[playerId].includes(channel.id)) {
        this.playerChannelMap[playerId].push(channel.id);
      }
    }
    
    return true;
  }
  
  // Remover canal
  removeChannel(channelId) {
    const channel = this.channels[channelId];
    if (!channel) return false;
    
    // Remover canal do mapeamento de todos os jogadores
    for (const playerId of channel.members) {
      if (this.playerChannelMap[playerId]) {
        const index = this.playerChannelMap[playerId].indexOf(channelId);
        if (index !== -1) {
          this.playerChannelMap[playerId].splice(index, 1);
        }
      }
    }
    
    delete this.channels[channelId];
    console.log(`Canal removido do router: ${channelId}`);
    return true;
  }
  
  // Atualizar posição de jogador (para detecção de proximidade)
  updatePlayerPosition(playerId, x, y, z) {
    return this.proximityDetector.updatePlayerPosition(playerId, x, y, z);
  }
  
  // Obter jogadores próximos
  getNearbyPlayers(playerId) {
    return this.proximityDetector.getNearbyPlayers(playerId);
  }
}

module.exports = AudioRouter;
/**
 * AudioChannel Class
 * 
 * Gerencia um canal de áudio para comunicação entre jogadores.
 * Implementa o sistema de isolamento acústico descrito no plano VOIP.
 */
class AudioChannel {
  constructor(id, type, members = []) {
    this.id = id;               // Identificador único do canal
    this.type = type;           // "group", "proximity", "private", "guild"
    this.members = members;     // Lista de membros do canal
    this.activeConnections = {}; // Conexões ativas
    console.log(`Canal de áudio criado: ${id} (tipo: ${type})`);
  }
  
  // Adiciona membro ao canal
  addMember(playerId) {
    if (!this.hasMember(playerId)) {
      this.members.push(playerId);
      console.log(`Jogador ${playerId} adicionado ao canal ${this.id}`);
      this.updateConnections();
      return true;
    }
    return false;
  }
  
  // Remove membro do canal
  removeMember(playerId) {
    const index = this.members.indexOf(playerId);
    if (index !== -1) {
      this.members.splice(index, 1);
      console.log(`Jogador ${playerId} removido do canal ${this.id}`);
      this.updateConnections();
      return true;
    }
    return false;
  }
  
  // Verifica se jogador está no canal
  hasMember(playerId) {
    return this.members.includes(playerId);
  }
  
  // Estabelece conexões de áudio entre todos os membros
  updateConnections() {
    // Limpar conexões antigas
    this.activeConnections = {};
    
    // Estabelecer novas conexões entre todos os membros
    for (let i = 0; i < this.members.length; i++) {
      for (let j = i + 1; j < this.members.length; j++) {
        const player1 = this.members[i];
        const player2 = this.members[j];
        const connectionId = [player1, player2].sort().join('-');
        
        this.activeConnections[connectionId] = {
          players: [player1, player2],
          established: Date.now()
        };
      }
    }
    
    console.log(`Conexões atualizadas para canal ${this.id}: ${Object.keys(this.activeConnections).length} conexões ativas`);
  }
  
  // Retorna estatísticas do canal
  getStats() {
    return {
      id: this.id,
      type: this.type,
      memberCount: this.members.length,
      connectionCount: Object.keys(this.activeConnections).length
    };
  }
}

module.exports = AudioChannel;
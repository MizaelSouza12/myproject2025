/**
 * ProximityDetector Class
 * 
 * Detecta proximidade entre jogadores no mundo do jogo.
 * Usado para determinar quem pode se comunicar por voz baseado na distância.
 */
class ProximityDetector {
  constructor(radius = 15) {
    this.proximityRadius = radius; // Raio de detecção em unidades do jogo
    this.playerPositions = {};     // Cache de posições dos jogadores
    console.log(`Detector de proximidade inicializado com raio ${radius}`);
  }
  
  // Atualiza posição de um jogador
  updatePlayerPosition(playerId, x, y, z) {
    this.playerPositions[playerId] = { x, y, z, lastUpdate: Date.now() };
    return true;
  }
  
  // Obtém jogadores próximos a um jogador específico
  getNearbyPlayers(playerId) {
    const position = this.playerPositions[playerId];
    if (!position) return [];
    
    const nearbyPlayers = [];
    
    for (const [id, pos] of Object.entries(this.playerPositions)) {
      if (id === playerId) continue;
      
      // Calcular distância euclidiana
      const distance = Math.sqrt(
        Math.pow(position.x - pos.x, 2) + 
        Math.pow(position.y - pos.y, 2) + 
        Math.pow(position.z - pos.z, 2)
      );
      
      if (distance <= this.proximityRadius) {
        nearbyPlayers.push({
          playerId: id,
          distance: distance,
          position: pos
        });
      }
    }
    
    // Ordenar por distância, mais próximos primeiro
    nearbyPlayers.sort((a, b) => a.distance - b.distance);
    
    return nearbyPlayers;
  }
  
  // Verifica se dois jogadores estão próximos
  arePlayersNearby(player1Id, player2Id) {
    const pos1 = this.playerPositions[player1Id];
    const pos2 = this.playerPositions[player2Id];
    
    if (!pos1 || !pos2) return false;
    
    // Calcular distância euclidiana
    const distance = Math.sqrt(
      Math.pow(pos1.x - pos2.x, 2) + 
      Math.pow(pos1.y - pos2.y, 2) + 
      Math.pow(pos1.z - pos2.z, 2)
    );
    
    return distance <= this.proximityRadius;
  }
  
  // Limpa posições antigas (jogadores desconectados)
  cleanupPositions(maxAgeMs = 300000) { // 5 minutos padrão
    const now = Date.now();
    let removed = 0;
    
    for (const [playerId, position] of Object.entries(this.playerPositions)) {
      if (now - position.lastUpdate > maxAgeMs) {
        delete this.playerPositions[playerId];
        removed++;
      }
    }
    
    if (removed > 0) {
      console.log(`Limpeza de posições: ${removed} jogadores removidos do cache`);
    }
    
    return removed;
  }
  
  // Configura o raio de proximidade
  setProximityRadius(radius) {
    if (radius > 0) {
      this.proximityRadius = radius;
      console.log(`Raio de proximidade ajustado para ${radius}`);
      return true;
    }
    return false;
  }
}

module.exports = ProximityDetector;
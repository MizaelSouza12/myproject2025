/**
 * RoomManager Class
 * 
 * Gerencia as salas VOIP no cliente, incluindo criação, 
 * entrada, saída e atualização de lista de salas.
 */
class RoomManager {
  constructor() {
    this.rooms = {};          // Salas disponíveis
    this.activeRooms = [];    // IDs das salas em que o jogador está
    this.selectedRoomId = null; // Sala atualmente selecionada
    this.eventHandlers = {};  // Manipuladores de eventos
    
    console.log('RoomManager inicializado');
  }
  
  // Define a lista de salas disponíveis
  setRooms(roomList) {
    // Atualizar salas existentes e adicionar novas
    roomList.forEach(room => {
      if (this.rooms[room.id]) {
        // Atualizar sala existente
        this.rooms[room.id] = {
          ...this.rooms[room.id],
          ...room,
          lastUpdate: Date.now()
        };
      } else {
        // Adicionar nova sala
        this.rooms[room.id] = {
          ...room,
          lastUpdate: Date.now(),
          members: room.members || []
        };
      }
    });
    
    // Emitir evento de atualização
    this._emitEvent('roomsUpdated', { rooms: this.getRoomList() });
    
    return this.getRoomList();
  }
  
  // Adiciona uma nova sala à lista
  addRoom(room) {
    if (!room.id) {
      console.error('Tentativa de adicionar sala sem ID');
      return false;
    }
    
    this.rooms[room.id] = {
      ...room,
      lastUpdate: Date.now(),
      members: room.members || []
    };
    
    // Emitir evento de atualização
    this._emitEvent('roomAdded', { room: this.rooms[room.id] });
    this._emitEvent('roomsUpdated', { rooms: this.getRoomList() });
    
    return this.rooms[room.id];
  }
  
  // Remove uma sala da lista
  removeRoom(roomId) {
    if (!this.rooms[roomId]) {
      return false;
    }
    
    const room = this.rooms[roomId];
    delete this.rooms[roomId];
    
    // Se era a sala selecionada, desselecionar
    if (this.selectedRoomId === roomId) {
      this.selectedRoomId = null;
    }
    
    // Remover da lista de salas ativas, se presente
    const activeIndex = this.activeRooms.indexOf(roomId);
    if (activeIndex !== -1) {
      this.activeRooms.splice(activeIndex, 1);
    }
    
    // Emitir evento de atualização
    this._emitEvent('roomRemoved', { roomId, room });
    this._emitEvent('roomsUpdated', { rooms: this.getRoomList() });
    
    return true;
  }
  
  // Atualiza informações de uma sala
  updateRoom(roomId, roomData) {
    if (!this.rooms[roomId]) {
      return false;
    }
    
    this.rooms[roomId] = {
      ...this.rooms[roomId],
      ...roomData,
      lastUpdate: Date.now()
    };
    
    // Emitir evento de atualização
    this._emitEvent('roomUpdated', { roomId, room: this.rooms[roomId] });
    this._emitEvent('roomsUpdated', { rooms: this.getRoomList() });
    
    return this.rooms[roomId];
  }
  
  // Adiciona um membro a uma sala
  addMemberToRoom(roomId, memberId, memberName) {
    if (!this.rooms[roomId]) {
      return false;
    }
    
    const room = this.rooms[roomId];
    
    // Verificar se o membro já está na sala
    if (!room.members) {
      room.members = [];
    }
    
    const existingMember = room.members.find(m => m.id === memberId);
    if (existingMember) {
      // Atualizar informações do membro
      existingMember.name = memberName || existingMember.name;
      existingMember.lastSeen = Date.now();
    } else {
      // Adicionar novo membro
      room.members.push({
        id: memberId,
        name: memberName || memberId,
        joinedAt: Date.now(),
        lastSeen: Date.now()
      });
    }
    
    // Emitir evento de atualização
    this._emitEvent('memberAdded', { 
      roomId, 
      memberId, 
      memberName: memberName || memberId 
    });
    
    this._emitEvent('roomUpdated', { roomId, room });
    
    return true;
  }
  
  // Remove um membro de uma sala
  removeMemberFromRoom(roomId, memberId) {
    if (!this.rooms[roomId] || !this.rooms[roomId].members) {
      return false;
    }
    
    const room = this.rooms[roomId];
    const memberIndex = room.members.findIndex(m => m.id === memberId);
    
    if (memberIndex === -1) {
      return false;
    }
    
    const member = room.members[memberIndex];
    room.members.splice(memberIndex, 1);
    
    // Emitir evento de atualização
    this._emitEvent('memberRemoved', { 
      roomId, 
      memberId, 
      memberName: member.name 
    });
    
    this._emitEvent('roomUpdated', { roomId, room });
    
    return true;
  }
  
  // Marca uma sala como ativa (jogador entrou)
  setRoomActive(roomId) {
    if (!this.rooms[roomId]) {
      return false;
    }
    
    if (!this.activeRooms.includes(roomId)) {
      this.activeRooms.push(roomId);
    }
    
    // Emitir evento
    this._emitEvent('roomJoined', { 
      roomId, 
      room: this.rooms[roomId] 
    });
    
    return true;
  }
  
  // Marca uma sala como inativa (jogador saiu)
  setRoomInactive(roomId) {
    const index = this.activeRooms.indexOf(roomId);
    if (index === -1) {
      return false;
    }
    
    this.activeRooms.splice(index, 1);
    
    // Se era a sala selecionada, desselecionar
    if (this.selectedRoomId === roomId) {
      this.selectedRoomId = null;
    }
    
    // Emitir evento
    this._emitEvent('roomLeft', { 
      roomId, 
      room: this.rooms[roomId] 
    });
    
    return true;
  }
  
  // Define a sala selecionada
  selectRoom(roomId) {
    if (roomId && !this.rooms[roomId]) {
      return false;
    }
    
    const previousRoom = this.selectedRoomId;
    this.selectedRoomId = roomId;
    
    // Emitir evento
    this._emitEvent('roomSelected', { 
      roomId, 
      previousRoom,
      room: roomId ? this.rooms[roomId] : null
    });
    
    return true;
  }
  
  // Cria uma nova sala
  createRoom(roomData) {
    // Gerar ID para a sala se não fornecido
    const roomId = roomData.id || `room-${Date.now()}-${Math.floor(Math.random() * 10000)}`;
    
    // Criar sala
    const room = {
      id: roomId,
      name: roomData.name || `Sala ${Math.floor(Math.random() * 1000)}`,
      type: roomData.type || 'group',
      members: roomData.members || [],
      isPrivate: roomData.isPrivate || false,
      createdAt: Date.now(),
      lastUpdate: Date.now()
    };
    
    // Adicionar sala
    this.rooms[roomId] = room;
    
    // Emitir evento
    this._emitEvent('roomCreated', { 
      roomId, 
      room 
    });
    
    this._emitEvent('roomsUpdated', { rooms: this.getRoomList() });
    
    return room;
  }
  
  // Obtém a lista de salas disponíveis
  getRoomList() {
    return Object.values(this.rooms).map(room => ({
      ...room,
      isActive: this.activeRooms.includes(room.id),
      isSelected: this.selectedRoomId === room.id,
      memberCount: room.members ? room.members.length : 0
    }));
  }
  
  // Obtém as salas ativas do jogador
  getActiveRooms() {
    return this.activeRooms.map(roomId => this.rooms[roomId])
      .filter(room => room !== undefined);
  }
  
  // Obtém uma sala específica
  getRoom(roomId) {
    return this.rooms[roomId];
  }
  
  // Registra um manipulador de eventos
  on(eventName, handler) {
    if (!this.eventHandlers[eventName]) {
      this.eventHandlers[eventName] = [];
    }
    
    this.eventHandlers[eventName].push(handler);
    
    return () => this.off(eventName, handler);
  }
  
  // Remove um manipulador de eventos
  off(eventName, handler) {
    if (!this.eventHandlers[eventName]) {
      return false;
    }
    
    const index = this.eventHandlers[eventName].indexOf(handler);
    if (index === -1) {
      return false;
    }
    
    this.eventHandlers[eventName].splice(index, 1);
    return true;
  }
  
  // Emite um evento para os manipuladores registrados
  _emitEvent(eventName, data) {
    if (!this.eventHandlers[eventName]) {
      return;
    }
    
    for (const handler of this.eventHandlers[eventName]) {
      try {
        handler(data);
      } catch (error) {
        console.error(`Erro ao processar evento ${eventName}:`, error);
      }
    }
  }
  
  // Limpa dados antigos
  cleanup() {
    const now = Date.now();
    const oldDataThreshold = 3600000; // 1 hora
    
    // Remover salas antigas que não são ativas
    for (const roomId in this.rooms) {
      const room = this.rooms[roomId];
      
      // Não remover salas ativas
      if (this.activeRooms.includes(roomId)) {
        continue;
      }
      
      // Remover salas antigas
      if (now - room.lastUpdate > oldDataThreshold) {
        delete this.rooms[roomId];
      }
    }
  }
}

// Exportar como módulo ES ou CommonJS
if (typeof module !== 'undefined' && module.exports) {
  module.exports = RoomManager;
} else {
  window.RoomManager = RoomManager;
}
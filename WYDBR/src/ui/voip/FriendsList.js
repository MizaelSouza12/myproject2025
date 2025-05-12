/**
 * FriendsList Class
 * 
 * Gerencia a lista de amigos para o sistema VOIP.
 * Permite adicionar, remover e atualizar status de amigos.
 */
class FriendsList {
  constructor() {
    this.friends = {};        // Lista de amigos
    this.pendingRequests = {}; // Solicitações pendentes
    this.blockedUsers = {};   // Usuários bloqueados
    this.eventHandlers = {};  // Manipuladores de eventos
    this.selectedFriendId = null; // Amigo selecionado
    
    console.log('FriendsList inicializado');
  }
  
  // Define a lista completa de amigos
  setFriends(friendsList) {
    // Manter estado de seleção
    const selectedFriendId = this.selectedFriendId;
    
    // Atualizar amigos existentes e adicionar novos
    friendsList.forEach(friend => {
      if (this.friends[friend.id]) {
        // Atualizar amigo existente
        this.friends[friend.id] = {
          ...this.friends[friend.id],
          ...friend,
          lastUpdate: Date.now()
        };
      } else {
        // Adicionar novo amigo
        this.friends[friend.id] = {
          ...friend,
          lastUpdate: Date.now()
        };
      }
    });
    
    // Restaurar seleção se o amigo ainda existir
    if (selectedFriendId && this.friends[selectedFriendId]) {
      this.selectedFriendId = selectedFriendId;
    } else {
      this.selectedFriendId = null;
    }
    
    // Emitir evento de atualização
    this._emitEvent('friendsUpdated', { friends: this.getFriendsList() });
    
    return this.getFriendsList();
  }
  
  // Adiciona um amigo à lista
  addFriend(friend) {
    if (!friend.id) {
      console.error('Tentativa de adicionar amigo sem ID');
      return false;
    }
    
    // Verificar se já existe
    const isNew = !this.friends[friend.id];
    
    // Adicionar ou atualizar
    this.friends[friend.id] = {
      ...friend,
      lastUpdate: Date.now()
    };
    
    // Emitir evento apropriado
    if (isNew) {
      this._emitEvent('friendAdded', { friend: this.friends[friend.id] });
    } else {
      this._emitEvent('friendUpdated', { friend: this.friends[friend.id] });
    }
    
    this._emitEvent('friendsUpdated', { friends: this.getFriendsList() });
    
    return this.friends[friend.id];
  }
  
  // Remove um amigo da lista
  removeFriend(friendId) {
    if (!this.friends[friendId]) {
      return false;
    }
    
    const friend = this.friends[friendId];
    delete this.friends[friendId];
    
    // Se era o amigo selecionado, desselecionar
    if (this.selectedFriendId === friendId) {
      this.selectedFriendId = null;
    }
    
    // Emitir evento de atualização
    this._emitEvent('friendRemoved', { friendId, friend });
    this._emitEvent('friendsUpdated', { friends: this.getFriendsList() });
    
    return true;
  }
  
  // Atualiza informações de um amigo
  updateFriend(friendId, friendData) {
    if (!this.friends[friendId]) {
      return false;
    }
    
    this.friends[friendId] = {
      ...this.friends[friendId],
      ...friendData,
      lastUpdate: Date.now()
    };
    
    // Emitir evento de atualização
    this._emitEvent('friendUpdated', { 
      friendId, 
      friend: this.friends[friendId] 
    });
    
    this._emitEvent('friendsUpdated', { friends: this.getFriendsList() });
    
    return this.friends[friendId];
  }
  
  // Atualiza o status de um amigo (online/offline)
  updateFriendStatus(friendId, isOnline, status = null) {
    if (!this.friends[friendId]) {
      return false;
    }
    
    const previousStatus = this.friends[friendId].online;
    this.friends[friendId].online = isOnline;
    
    if (status !== null) {
      this.friends[friendId].status = status;
    }
    
    this.friends[friendId].lastUpdate = Date.now();
    
    // Emitir evento de atualização
    this._emitEvent('friendStatusChanged', { 
      friendId, 
      friend: this.friends[friendId],
      previousStatus
    });
    
    this._emitEvent('friendUpdated', { 
      friendId, 
      friend: this.friends[friendId] 
    });
    
    this._emitEvent('friendsUpdated', { friends: this.getFriendsList() });
    
    return this.friends[friendId];
  }
  
  // Adiciona uma solicitação de amizade pendente
  addFriendRequest(request) {
    if (!request.id) {
      console.error('Tentativa de adicionar solicitação sem ID');
      return false;
    }
    
    this.pendingRequests[request.id] = {
      ...request,
      receivedAt: Date.now()
    };
    
    // Emitir evento
    this._emitEvent('friendRequestReceived', { 
      requestId: request.id, 
      request: this.pendingRequests[request.id] 
    });
    
    this._emitEvent('friendRequestsUpdated', { 
      requests: this.getPendingRequests() 
    });
    
    return this.pendingRequests[request.id];
  }
  
  // Remove uma solicitação de amizade pendente
  removeFriendRequest(requestId) {
    if (!this.pendingRequests[requestId]) {
      return false;
    }
    
    const request = this.pendingRequests[requestId];
    delete this.pendingRequests[requestId];
    
    // Emitir evento
    this._emitEvent('friendRequestRemoved', { requestId, request });
    this._emitEvent('friendRequestsUpdated', { 
      requests: this.getPendingRequests() 
    });
    
    return true;
  }
  
  // Aceita uma solicitação de amizade
  acceptFriendRequest(requestId) {
    if (!this.pendingRequests[requestId]) {
      return false;
    }
    
    const request = this.pendingRequests[requestId];
    
    // Adicionar como amigo
    this.addFriend({
      id: request.senderId,
      name: request.senderName,
      online: true // Assumimos que está online se enviou solicitação
    });
    
    // Remover solicitação
    delete this.pendingRequests[requestId];
    
    // Emitir evento
    this._emitEvent('friendRequestAccepted', { requestId, request });
    this._emitEvent('friendRequestsUpdated', { 
      requests: this.getPendingRequests() 
    });
    
    return true;
  }
  
  // Rejeita uma solicitação de amizade
  rejectFriendRequest(requestId) {
    if (!this.pendingRequests[requestId]) {
      return false;
    }
    
    const request = this.pendingRequests[requestId];
    
    // Remover solicitação
    delete this.pendingRequests[requestId];
    
    // Emitir evento
    this._emitEvent('friendRequestRejected', { requestId, request });
    this._emitEvent('friendRequestsUpdated', { 
      requests: this.getPendingRequests() 
    });
    
    return true;
  }
  
  // Adiciona um usuário à lista de bloqueados
  blockUser(userId, userName) {
    this.blockedUsers[userId] = {
      id: userId,
      name: userName || userId,
      blockedAt: Date.now()
    };
    
    // Se era um amigo, removê-lo
    if (this.friends[userId]) {
      this.removeFriend(userId);
    }
    
    // Se tinha solicitação pendente, removê-la
    for (const requestId in this.pendingRequests) {
      if (this.pendingRequests[requestId].senderId === userId) {
        this.removeFriendRequest(requestId);
      }
    }
    
    // Emitir evento
    this._emitEvent('userBlocked', { 
      userId, 
      userName 
    });
    
    return true;
  }
  
  // Remove um usuário da lista de bloqueados
  unblockUser(userId) {
    if (!this.blockedUsers[userId]) {
      return false;
    }
    
    const user = this.blockedUsers[userId];
    delete this.blockedUsers[userId];
    
    // Emitir evento
    this._emitEvent('userUnblocked', { 
      userId, 
      userName: user.name 
    });
    
    return true;
  }
  
  // Verifica se um usuário está bloqueado
  isUserBlocked(userId) {
    return !!this.blockedUsers[userId];
  }
  
  // Seleciona um amigo
  selectFriend(friendId) {
    if (friendId && !this.friends[friendId]) {
      return false;
    }
    
    const previousFriend = this.selectedFriendId;
    this.selectedFriendId = friendId;
    
    // Emitir evento
    this._emitEvent('friendSelected', { 
      friendId, 
      previousFriend,
      friend: friendId ? this.friends[friendId] : null
    });
    
    return true;
  }
  
  // Obtém a lista de amigos
  getFriendsList() {
    return Object.values(this.friends).map(friend => ({
      ...friend,
      isSelected: this.selectedFriendId === friend.id
    }));
  }
  
  // Obtém a lista de solicitações pendentes
  getPendingRequests() {
    return Object.values(this.pendingRequests);
  }
  
  // Obtém a lista de usuários bloqueados
  getBlockedUsers() {
    return Object.values(this.blockedUsers);
  }
  
  // Obtém um amigo específico
  getFriend(friendId) {
    return this.friends[friendId];
  }
  
  // Obtém amigos online
  getOnlineFriends() {
    return Object.values(this.friends)
      .filter(friend => friend.online)
      .map(friend => ({
        ...friend,
        isSelected: this.selectedFriendId === friend.id
      }));
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
    
    // Limpar solicitações antigas
    for (const requestId in this.pendingRequests) {
      const request = this.pendingRequests[requestId];
      
      if (now - request.receivedAt > oldDataThreshold) {
        delete this.pendingRequests[requestId];
      }
    }
  }
}

// Exportar como módulo ES ou CommonJS
if (typeof module !== 'undefined' && module.exports) {
  module.exports = FriendsList;
} else {
  window.FriendsList = FriendsList;
}
/**
 * Componente de Menu VOIP
 * 
 * Implementa o menu lateral responsivo para o sistema VOIP.
 * Inclui controle de microfone, lista de salas e lista de amigos.
 */

// Estilos CSS
import './VoipSidebar.css';

class VoipMenu {
  constructor(containerId = 'voip-container') {
    this.containerId = containerId;
    this.expanded = false;
    this.activeTab = 'rooms'; // 'rooms', 'friends', 'settings'
    this.container = null;
    this.initialized = false;
    
    // Elementos do menu
    this.sidebarEl = null;
    this.tabsEl = null;
    this.contentEl = null;
    
    // Dados
    this.rooms = [];
    this.friends = [];
    
    console.log('Menu VOIP inicializado');
  }
  
  // Inicializa o menu
  init() {
    if (this.initialized) return;
    
    // Verificar se container existe
    this.container = document.getElementById(this.containerId);
    if (!this.container) {
      console.log(`Criando container VOIP: ${this.containerId}`);
      this.container = document.createElement('div');
      this.container.id = this.containerId;
      document.body.appendChild(this.container);
    }
    
    // Criar estrutura básica
    this.createMenuStructure();
    
    // Adicionar eventos
    this.attachEventListeners();
    
    this.initialized = true;
    console.log('Menu VOIP estrutura criada');
    
    return this;
  }
  
  // Cria a estrutura base do menu
  createMenuStructure() {
    const sidebarHtml = `
      <div class="voip-sidebar" data-expanded="${this.expanded}">
        <div class="voip-toggle">
          <button id="voip-toggle-btn">
            <i class="voip-icon voip-icon-mic"></i>
          </button>
        </div>
        <div class="voip-content">
          <div class="voip-header">
            <h3>VOIP</h3>
            <div class="voip-controls">
              <button id="voip-mic-btn" class="voip-btn"><i class="voip-icon voip-icon-mic"></i></button>
              <button id="voip-settings-btn" class="voip-btn"><i class="voip-icon voip-icon-settings"></i></button>
            </div>
          </div>
          <div class="voip-tabs">
            <button id="voip-tab-rooms" class="voip-tab-btn active">Salas</button>
            <button id="voip-tab-friends" class="voip-tab-btn">Amigos</button>
            <button id="voip-tab-settings" class="voip-tab-btn">Config</button>
          </div>
          <div class="voip-tab-content">
            <div id="voip-content-rooms" class="voip-tab-panel active">
              <div class="voip-room-list"></div>
              <button id="voip-create-room" class="voip-btn voip-btn-primary">+ Nova Sala</button>
            </div>
            <div id="voip-content-friends" class="voip-tab-panel">
              <div class="voip-friend-list"></div>
            </div>
            <div id="voip-content-settings" class="voip-tab-panel">
              <div class="voip-settings-form">
                <div class="voip-form-group">
                  <label>Volume de Entrada</label>
                  <input type="range" min="0" max="100" value="80" id="voip-input-volume">
                </div>
                <div class="voip-form-group">
                  <label>Volume de Saída</label>
                  <input type="range" min="0" max="100" value="80" id="voip-output-volume">
                </div>
                <div class="voip-form-group">
                  <label>Dispositivo de Entrada</label>
                  <select id="voip-input-device"></select>
                </div>
                <div class="voip-form-group">
                  <label>Dispositivo de Saída</label>
                  <select id="voip-output-device"></select>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    `;
    
    this.container.innerHTML = sidebarHtml;
    
    // Guardar referências aos elementos
    this.sidebarEl = this.container.querySelector('.voip-sidebar');
    this.tabsEl = this.container.querySelector('.voip-tabs');
    this.contentEl = this.container.querySelector('.voip-tab-content');
  }
  
  // Adiciona event listeners
  attachEventListeners() {
    // Toggle para expandir/recolher
    const toggleBtn = this.container.querySelector('#voip-toggle-btn');
    if (toggleBtn) {
      toggleBtn.addEventListener('click', () => this.toggleSidebar());
    }
    
    // Tabs
    const tabBtns = this.container.querySelectorAll('.voip-tab-btn');
    tabBtns.forEach(btn => {
      btn.addEventListener('click', (e) => {
        const tabId = e.target.id.replace('voip-tab-', '');
        this.switchTab(tabId);
      });
    });
    
    // Botão de criar sala
    const createRoomBtn = this.container.querySelector('#voip-create-room');
    if (createRoomBtn) {
      createRoomBtn.addEventListener('click', () => this.createRoom());
    }
    
    // Controle de microfone
    const micBtn = this.container.querySelector('#voip-mic-btn');
    if (micBtn) {
      micBtn.addEventListener('click', () => this.toggleMicrophone());
    }
  }
  
  // Alterna o estado do sidebar (expandido/recolhido)
  toggleSidebar() {
    this.expanded = !this.expanded;
    if (this.sidebarEl) {
      this.sidebarEl.setAttribute('data-expanded', this.expanded);
    }
    console.log(`Menu VOIP ${this.expanded ? 'expandido' : 'recolhido'}`);
    return this.expanded;
  }
  
  // Alterna entre as abas
  switchTab(tabId) {
    if (!tabId || !['rooms', 'friends', 'settings'].includes(tabId)) return;
    
    this.activeTab = tabId;
    
    // Atualizar classes das abas
    const allTabs = this.container.querySelectorAll('.voip-tab-btn');
    allTabs.forEach(tab => {
      tab.classList.remove('active');
      if (tab.id === `voip-tab-${tabId}`) {
        tab.classList.add('active');
      }
    });
    
    // Atualizar conteúdo visível
    const allPanels = this.container.querySelectorAll('.voip-tab-panel');
    allPanels.forEach(panel => {
      panel.classList.remove('active');
      if (panel.id === `voip-content-${tabId}`) {
        panel.classList.add('active');
      }
    });
    
    console.log(`Menu VOIP: aba alterada para ${tabId}`);
    return true;
  }
  
  // Atualiza a lista de salas
  updateRooms(rooms) {
    this.rooms = rooms || [];
    
    const roomListEl = this.container.querySelector('.voip-room-list');
    if (!roomListEl) return;
    
    let roomsHtml = '';
    
    if (this.rooms.length === 0) {
      roomsHtml = '<div class="voip-empty-list">Nenhuma sala disponível</div>';
    } else {
      this.rooms.forEach(room => {
        roomsHtml += `
          <div class="voip-room-item" data-room-id="${room.id}">
            <div class="voip-room-info">
              <span class="voip-room-name">${room.name}</span>
              <span class="voip-room-members">${room.members} membros</span>
            </div>
            <div class="voip-room-actions">
              <button class="voip-btn voip-join-room" data-room-id="${room.id}">Entrar</button>
            </div>
          </div>
        `;
      });
    }
    
    roomListEl.innerHTML = roomsHtml;
    
    // Adicionar eventos aos botões de entrar
    const joinBtns = roomListEl.querySelectorAll('.voip-join-room');
    joinBtns.forEach(btn => {
      btn.addEventListener('click', (e) => {
        const roomId = e.target.getAttribute('data-room-id');
        this.joinRoom(roomId);
      });
    });
  }
  
  // Atualiza a lista de amigos
  updateFriends(friends) {
    this.friends = friends || [];
    
    const friendListEl = this.container.querySelector('.voip-friend-list');
    if (!friendListEl) return;
    
    let friendsHtml = '';
    
    if (this.friends.length === 0) {
      friendsHtml = '<div class="voip-empty-list">Nenhum amigo adicionado</div>';
    } else {
      this.friends.forEach(friend => {
        const statusClass = friend.online ? 'online' : 'offline';
        friendsHtml += `
          <div class="voip-friend-item" data-friend-id="${friend.id}">
            <div class="voip-friend-avatar">
              <div class="voip-status-indicator ${statusClass}"></div>
            </div>
            <div class="voip-friend-info">
              <span class="voip-friend-name">${friend.name}</span>
              <span class="voip-friend-status">${friend.status || (friend.online ? 'Online' : 'Offline')}</span>
            </div>
            <div class="voip-friend-actions">
              ${friend.online ? `<button class="voip-btn voip-call-friend" data-friend-id="${friend.id}">Chamar</button>` : ''}
            </div>
          </div>
        `;
      });
    }
    
    friendListEl.innerHTML = friendsHtml;
    
    // Adicionar eventos aos botões de chamar
    const callBtns = friendListEl.querySelectorAll('.voip-call-friend');
    callBtns.forEach(btn => {
      btn.addEventListener('click', (e) => {
        const friendId = e.target.getAttribute('data-friend-id');
        this.callFriend(friendId);
      });
    });
  }
  
  // Funções para manipulação de salas
  createRoom() {
    console.log('Função createRoom chamada');
    // Implementação específica será adicionada
  }
  
  joinRoom(roomId) {
    console.log(`Função joinRoom chamada: ${roomId}`);
    // Implementação específica será adicionada
  }
  
  // Funções para manipulação de amigos
  callFriend(friendId) {
    console.log(`Função callFriend chamada: ${friendId}`);
    // Implementação específica será adicionada
  }
  
  // Controle de microfone
  toggleMicrophone() {
    console.log('Função toggleMicrophone chamada');
    // Implementação específica será adicionada
  }
}

export default VoipMenu;
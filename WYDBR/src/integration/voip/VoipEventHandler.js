/**
 * VoipEventHandler Class
 * 
 * Gerencia eventos relacionados ao sistema VOIP.
 * Fornece integração com sistemas externos, notificações de sistema e interfaces.
 */
class VoipEventHandler {
  constructor(options = {}) {
    this.options = {
      debug: options.debug || false,
      notifyUI: options.notifyUI !== false,
      notifySystem: options.notifySystem !== false,
      ...options
    };
    
    this.eventListeners = {};
    this.uiNotifier = options.uiNotifier || this._createDefaultUINotifier();
    this.systemNotifier = options.systemNotifier || this._createDefaultSystemNotifier();
    this.lastEvents = {}; // Armazena os últimos eventos de cada tipo
    
    console.log('VoipEventHandler inicializado');
  }
  
  // Registra um listener para um tipo específico de evento
  addEventListener(eventType, listener) {
    if (!this.eventListeners[eventType]) {
      this.eventListeners[eventType] = [];
    }
    
    this.eventListeners[eventType].push(listener);
    
    // Retornar função para remover o listener
    return () => this.removeEventListener(eventType, listener);
  }
  
  // Remove um listener específico
  removeEventListener(eventType, listener) {
    if (!this.eventListeners[eventType]) {
      return false;
    }
    
    const index = this.eventListeners[eventType].indexOf(listener);
    if (index !== -1) {
      this.eventListeners[eventType].splice(index, 1);
      return true;
    }
    
    return false;
  }
  
  // Dispara um evento para todos os listeners registrados
  dispatchEvent(eventType, eventData) {
    // Armazenar como último evento deste tipo
    this.lastEvents[eventType] = {
      data: eventData,
      timestamp: Date.now()
    };
    
    // Notificar interface do usuário se configurado
    if (this.options.notifyUI) {
      this._notifyUI(eventType, eventData);
    }
    
    // Notificar sistema se configurado
    if (this.options.notifySystem) {
      this._notifySystem(eventType, eventData);
    }
    
    // Log de debug
    if (this.options.debug) {
      console.log(`Evento VOIP [${eventType}]:`, eventData);
    }
    
    // Notificar listeners registrados
    if (this.eventListeners[eventType]) {
      this.eventListeners[eventType].forEach(listener => {
        try {
          listener(eventData);
        } catch (error) {
          console.error(`Erro em listener de evento ${eventType}:`, error);
        }
      });
    }
    
    // Notificar listeners de "all"
    if (this.eventListeners['all']) {
      this.eventListeners['all'].forEach(listener => {
        try {
          listener({ type: eventType, data: eventData });
        } catch (error) {
          console.error(`Erro em listener de 'all' para evento ${eventType}:`, error);
        }
      });
    }
    
    return true;
  }
  
  // Notifica a interface do usuário sobre um evento
  _notifyUI(eventType, eventData) {
    if (!this.uiNotifier) return;
    
    try {
      this.uiNotifier(eventType, eventData);
    } catch (error) {
      console.error('Erro ao notificar UI:', error);
    }
  }
  
  // Notifica o sistema sobre um evento
  _notifySystem(eventType, eventData) {
    if (!this.systemNotifier) return;
    
    try {
      this.systemNotifier(eventType, eventData);
    } catch (error) {
      console.error('Erro ao notificar sistema:', error);
    }
  }
  
  // Cria um notificador de UI padrão
  _createDefaultUINotifier() {
    return (eventType, eventData) => {
      // Verificar se estamos em ambiente do navegador
      if (typeof window === 'undefined') return;
      
      // Verificar se temos um container de notificação
      let notificationContainer = document.getElementById('voip-notifications');
      
      if (!notificationContainer) {
        // Criar container de notificação
        notificationContainer = document.createElement('div');
        notificationContainer.id = 'voip-notifications';
        notificationContainer.style.position = 'fixed';
        notificationContainer.style.bottom = '20px';
        notificationContainer.style.right = '20px';
        notificationContainer.style.zIndex = '9999';
        document.body.appendChild(notificationContainer);
      }
      
      // Filtrar eventos que devem gerar notificação visual
      if (this._shouldShowVisualNotification(eventType, eventData)) {
        this._createVisualNotification(notificationContainer, eventType, eventData);
      }
      
      // Disparar evento customizado para qualquer componente UI registrado
      const customEvent = new CustomEvent('voip-event', {
        detail: { type: eventType, data: eventData }
      });
      
      window.dispatchEvent(customEvent);
    };
  }
  
  // Cria um notificador de sistema padrão
  _createDefaultSystemNotifier() {
    return (eventType, eventData) => {
      // Somente para ambiente do navegador
      if (typeof window === 'undefined') return;
      
      // Reproduzir som para certos eventos
      if (this._shouldPlaySoundNotification(eventType, eventData)) {
        this._playSoundNotification(eventType);
      }
    };
  }
  
  // Verifica se deve mostrar notificação visual
  _shouldShowVisualNotification(eventType, eventData) {
    // Lista de eventos que devem gerar notificação visual
    const visualNotificationEvents = [
      'call_incoming', 
      'call_missed',
      'friend_online',
      'friend_request_received',
      'room_invitation',
      'player_speaking_nearby',
      'mic_status_changed',
      'connection_status_changed',
      'error'
    ];
    
    return visualNotificationEvents.includes(eventType);
  }
  
  // Verifica se deve reproduzir notificação sonora
  _shouldPlaySoundNotification(eventType, eventData) {
    // Lista de eventos que devem gerar som
    const soundNotificationEvents = [
      'call_incoming',
      'friend_online',
      'friend_request_received',
      'room_invitation',
      'error'
    ];
    
    return soundNotificationEvents.includes(eventType);
  }
  
  // Cria notificação visual
  _createVisualNotification(container, eventType, eventData) {
    // Criar elemento de notificação
    const notification = document.createElement('div');
    notification.className = 'voip-notification';
    
    // Estilizar notificação
    notification.style.backgroundColor = 'rgba(33, 33, 33, 0.9)';
    notification.style.color = 'white';
    notification.style.padding = '10px 15px';
    notification.style.borderRadius = '5px';
    notification.style.margin = '5px 0';
    notification.style.maxWidth = '300px';
    notification.style.boxShadow = '0 2px 4px rgba(0, 0, 0, 0.2)';
    notification.style.transition = 'all 0.3s ease';
    notification.style.opacity = '0';
    notification.style.transform = 'translateX(20px)';
    
    // Definir conteúdo conforme tipo de evento
    let title = '';
    let message = '';
    let icon = '';
    
    switch (eventType) {
      case 'call_incoming':
        title = 'Chamada recebida';
        message = `${eventData.callerName || 'Alguém'} está chamando você`;
        icon = '📞';
        notification.style.borderLeft = '4px solid #4c84ff';
        break;
        
      case 'call_missed':
        title = 'Chamada perdida';
        message = `Você perdeu uma chamada de ${eventData.callerName || 'alguém'}`;
        icon = '📱';
        notification.style.borderLeft = '4px solid #ff4c4c';
        break;
        
      case 'friend_online':
        title = 'Amigo online';
        message = `${eventData.friendName || 'Seu amigo'} acabou de ficar online`;
        icon = '👋';
        notification.style.borderLeft = '4px solid #4caf50';
        break;
        
      case 'friend_request_received':
        title = 'Nova solicitação de amizade';
        message = `${eventData.senderName || 'Alguém'} quer ser seu amigo`;
        icon = '🤝';
        notification.style.borderLeft = '4px solid #ff9800';
        break;
        
      case 'room_invitation':
        title = 'Convite para sala';
        message = `${eventData.inviterName || 'Alguém'} convidou você para ${eventData.roomName || 'uma sala'}`;
        icon = '🚪';
        notification.style.borderLeft = '4px solid #2196f3';
        break;
        
      case 'player_speaking_nearby':
        title = 'Jogador próximo falando';
        message = `${eventData.playerName || 'Alguém próximo'} está falando`;
        icon = '🔊';
        notification.style.borderLeft = '4px solid #9c27b0';
        break;
        
      case 'mic_status_changed':
        title = eventData.active ? 'Microfone ativado' : 'Microfone desativado';
        message = eventData.active ? 'Seu microfone está ativo' : 'Seu microfone está mudo';
        icon = eventData.active ? '🎙️' : '🔇';
        notification.style.borderLeft = `4px solid ${eventData.active ? '#4caf50' : '#ff9800'}`;
        break;
        
      case 'connection_status_changed':
        title = eventData.connected ? 'Conectado' : 'Desconectado';
        message = eventData.connected ? 'Você está conectado ao servidor VOIP' : 'Você está desconectado do servidor VOIP';
        icon = eventData.connected ? '🟢' : '🔴';
        notification.style.borderLeft = `4px solid ${eventData.connected ? '#4caf50' : '#f44336'}`;
        break;
        
      case 'error':
        title = 'Erro no sistema VOIP';
        message = eventData.message || 'Ocorreu um erro no sistema VOIP';
        icon = '⚠️';
        notification.style.borderLeft = '4px solid #f44336';
        break;
        
      default:
        title = 'Notificação VOIP';
        message = 'Uma nova notificação do sistema VOIP';
        icon = '📢';
        notification.style.borderLeft = '4px solid #607d8b';
    }
    
    // Montar HTML da notificação
    notification.innerHTML = `
      <div style="display: flex; align-items: center;">
        <div style="font-size: 24px; margin-right: 10px;">${icon}</div>
        <div>
          <div style="font-weight: bold; margin-bottom: 3px;">${title}</div>
          <div style="font-size: 14px;">${message}</div>
        </div>
      </div>
    `;
    
    // Adicionar botão de fechar
    const closeButton = document.createElement('div');
    closeButton.innerHTML = '&times;';
    closeButton.style.position = 'absolute';
    closeButton.style.top = '5px';
    closeButton.style.right = '10px';
    closeButton.style.fontSize = '18px';
    closeButton.style.cursor = 'pointer';
    closeButton.style.opacity = '0.7';
    closeButton.addEventListener('click', () => {
      this._removeNotification(notification);
    });
    
    notification.style.position = 'relative';
    notification.appendChild(closeButton);
    
    // Adicionar ao container
    container.appendChild(notification);
    
    // Animar entrada (pequeno delay para o navegador processar)
    setTimeout(() => {
      notification.style.opacity = '1';
      notification.style.transform = 'translateX(0)';
    }, 10);
    
    // Configurar para remover após 5 segundos
    setTimeout(() => {
      this._removeNotification(notification);
    }, 5000);
  }
  
  // Remove uma notificação com animação
  _removeNotification(notification) {
    notification.style.opacity = '0';
    notification.style.transform = 'translateX(20px)';
    
    // Remover do DOM após a animação
    setTimeout(() => {
      if (notification.parentNode) {
        notification.parentNode.removeChild(notification);
      }
    }, 300);
  }
  
  // Reproduz notificação sonora
  _playSoundNotification(eventType) {
    // Mapeamento de eventos para sons
    const soundMap = {
      'call_incoming': 'ring',
      'friend_online': 'online',
      'friend_request_received': 'request',
      'room_invitation': 'invite',
      'error': 'error'
    };
    
    const soundType = soundMap[eventType] || 'notification';
    
    // Carregar e tocar som
    this._playSound(soundType);
  }
  
  // Reproduz um som específico
  _playSound(soundType) {
    // Verificar se estamos em ambiente do navegador
    if (typeof window === 'undefined' || !window.Audio) return;
    
    // Criar elemento de áudio (não adicionado ao DOM para evitar poluição visual)
    const audio = new Audio();
    
    // Pode ser útil usar uma biblioteca de sons embutida como data URLs
    // Aqui estamos simulando isso com nomes de arquivos base
    const soundFiles = {
      'ring': 'ring.mp3',
      'online': 'online.mp3',
      'request': 'request.mp3',
      'invite': 'invite.mp3',
      'error': 'error.mp3',
      'notification': 'notification.mp3'
    };
    
    // Em uma implementação real, você teria os arquivos de som ou data URLs
    // Para este exemplo, vamos apenas simular
    audio.src = `/sounds/${soundFiles[soundType]}`;
    
    // Configurar volume e tocar
    audio.volume = 0.7;
    audio.play().catch(err => {
      // Muitos navegadores bloqueiam a reprodução automática
      console.warn('Não foi possível reproduzir som:', err);
    });
  }
  
  // Obtém o último evento de um tipo específico
  getLastEvent(eventType) {
    return this.lastEvents[eventType];
  }
  
  // Define um novo notificador UI
  setUINotifier(notifier) {
    if (typeof notifier !== 'function') {
      throw new Error('O notificador UI deve ser uma função');
    }
    
    this.uiNotifier = notifier;
  }
  
  // Define um novo notificador de sistema
  setSystemNotifier(notifier) {
    if (typeof notifier !== 'function') {
      throw new Error('O notificador de sistema deve ser uma função');
    }
    
    this.systemNotifier = notifier;
  }
}

// Eventos padrão para VOIP
VoipEventHandler.EVENTS = {
  // Eventos de conexão
  CONNECTION_ESTABLISHED: 'connection_established',
  CONNECTION_LOST: 'connection_lost',
  CONNECTION_STATUS_CHANGED: 'connection_status_changed',
  
  // Eventos de sala
  ROOM_CREATED: 'room_created',
  ROOM_JOINED: 'room_joined',
  ROOM_LEFT: 'room_left',
  ROOM_INVITATION: 'room_invitation',
  PLAYER_JOINED_ROOM: 'player_joined_room',
  PLAYER_LEFT_ROOM: 'player_left_room',
  
  // Eventos de chamada
  CALL_INCOMING: 'call_incoming',
  CALL_OUTGOING: 'call_outgoing',
  CALL_CONNECTED: 'call_connected',
  CALL_ENDED: 'call_ended',
  CALL_MISSED: 'call_missed',
  CALL_REJECTED: 'call_rejected',
  
  // Eventos de amigos
  FRIEND_ONLINE: 'friend_online',
  FRIEND_OFFLINE: 'friend_offline',
  FRIEND_ADDED: 'friend_added',
  FRIEND_REMOVED: 'friend_removed',
  FRIEND_REQUEST_RECEIVED: 'friend_request_received',
  FRIEND_REQUEST_SENT: 'friend_request_sent',
  FRIEND_REQUEST_ACCEPTED: 'friend_request_accepted',
  FRIEND_REQUEST_REJECTED: 'friend_request_rejected',
  
  // Eventos de áudio
  MIC_STATUS_CHANGED: 'mic_status_changed',
  AUDIO_STATUS_CHANGED: 'audio_status_changed',
  PLAYER_SPEAKING: 'player_speaking',
  PLAYER_SPEAKING_NEARBY: 'player_speaking_nearby',
  DEVICE_CHANGED: 'device_changed',
  
  // Eventos de erro
  ERROR: 'error',
  WARNING: 'warning'
};

// Exportar como módulo ES ou CommonJS
if (typeof module !== 'undefined' && module.exports) {
  module.exports = VoipEventHandler;
} else {
  window.VoipEventHandler = VoipEventHandler;
}
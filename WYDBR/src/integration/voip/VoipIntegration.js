/**
 * VoipIntegration
 * 
 * Integra o sistema VOIP com o cliente do jogo.
 * Estabelece hooks para posições dos jogadores e eventos do jogo.
 */
const WebRTC = require('webrtc');
const AudioRouter = require('./AudioRouter');
const AudioChannel = require('./AudioChannel');
const WindowsMicrophonePermissions = require('../../systems/voip/WindowsMicrophonePermissions');

class VoipIntegration {
  constructor(config = {}) {
    this.config = {
      serverUrl: config.serverUrl || 'wss://voip.wydbr.com',
      signalingPort: config.signalingPort || 8443,
      iceServers: config.iceServers || [
        { urls: 'stun:stun.l.google.com:19302' },
        { urls: 'stun:stun1.l.google.com:19302' }
      ],
      proximityRadius: config.proximityRadius || 15,
      ...config
    };
    
    // Componentes principais
    this.audioRouter = new AudioRouter();
    this.channels = {};
    this.activeConnections = {};
    this.peerConnections = {};
    this.localStream = null;
    this.initialized = false;
    
    // Gerenciador de permissões do microfone
    this.microphonePermissions = new WindowsMicrophonePermissions();
    
    // Configuração de áudio
    this.audioConfig = {
      echoCancellation: true,
      noiseSuppression: true,
      autoGainControl: true
    };
    
    // Estado do jogador
    this.playerState = {
      id: null,
      position: { x: 0, y: 0, z: 0 },
      activeChannels: [],
      microphoneActive: false,
      audioActive: true
    };
    
    console.log('VoipIntegration instanciado');
  }
  
  // Inicializa o sistema VOIP
  async initialize(playerId) {
    if (this.initialized) return true;
    
    try {
      this.playerState.id = playerId;
      
      // Verificar permissão de microfone
      const hasMicrophonePermission = await this.requestMicrophonePermission();
      if (!hasMicrophonePermission) {
        console.warn('Sistema VOIP iniciado sem permissão de microfone');
      }
      
      // Inicializar mídia local
      await this.initializeLocalMedia();
      
      // Conectar ao servidor de sinalização
      await this.connectSignalingServer();
      
      // Criar canal de proximidade padrão
      this.createProximityChannel();
      
      this.initialized = true;
      console.log(`Sistema VOIP inicializado para jogador ${playerId}`);
      return true;
    } catch (error) {
      console.error('Erro ao inicializar sistema VOIP:', error);
      return false;
    }
  }
  
  // Solicita permissão para usar o microfone
  async requestMicrophonePermission() {
    try {
      const hasPermission = await this.microphonePermissions.requestPermissionWithUI({
        showInstructionsOnDenied: true,
        retryButton: true,
        onRetry: (success) => {
          if (success) {
            console.log('Permissão de microfone concedida após nova tentativa');
            this.initializeLocalMedia().catch(err => console.error('Erro ao inicializar mídia após nova tentativa:', err));
          }
        }
      });
      
      return hasPermission;
    } catch (error) {
      console.error('Erro ao solicitar permissão de microfone:', error);
      return false;
    }
  }
  
  // Inicializa a mídia local (microfone)
  async initializeLocalMedia() {
    try {
      // Verificar se temos permissão de microfone
      const hasPermission = await this.microphonePermissions.checkPermission();
      
      if (!hasPermission) {
        console.warn('Tentando inicializar mídia local sem permissão de microfone');
        // Vamos prosseguir, mas provavelmente falhará
      }
      
      this.localStream = await navigator.mediaDevices.getUserMedia({
        audio: this.audioConfig,
        video: false
      });
      
      console.log('Mídia local inicializada (microfone)');
      return true;
    } catch (error) {
      if (error.name === 'NotAllowedError' || error.name === 'PermissionDeniedError') {
        console.error('Permissão de microfone negada pelo usuário');
      } else {
        console.error('Erro ao acessar microfone:', error.name, error.message);
      }
      throw error;
    }
  }
  
  // Conecta ao servidor de sinalização
  async connectSignalingServer() {
    // Este é um placeholder - em uma implementação real,
    // conectaríamos a um servidor WebSocket para sinalização WebRTC
    console.log(`Conectando ao servidor de sinalização: ${this.config.serverUrl}`);
    
    // Simular conexão bem-sucedida
    await new Promise(resolve => setTimeout(resolve, 100));
    
    return true;
  }
  
  // Atualiza a posição do jogador
  updatePlayerPosition(x, y, z) {
    this.playerState.position = { x, y, z };
    this.audioRouter.updatePlayerPosition(this.playerState.id, x, y, z);
    
    // Se estiver em um canal de proximidade, verificar jogadores próximos
    if (this.isInProximityChannel()) {
      this.updateProximityConnections();
    }
    
    return true;
  }
  
  // Verifica se o jogador está em um canal de proximidade
  isInProximityChannel() {
    return this.playerState.activeChannels.some(channelId => 
      this.channels[channelId] && this.channels[channelId].type === 'proximity'
    );
  }
  
  // Cria canal de proximidade padrão
  createProximityChannel() {
    const proximityChannel = new AudioChannel(
      'proximity-default',
      'proximity',
      [this.playerState.id]
    );
    
    this.channels[proximityChannel.id] = proximityChannel;
    this.audioRouter.addChannel(proximityChannel);
    this.playerState.activeChannels.push(proximityChannel.id);
    
    console.log('Canal de proximidade padrão criado');
    return proximityChannel.id;
  }
  
  // Atualiza conexões baseadas em proximidade
  updateProximityConnections() {
    const nearbyPlayers = this.audioRouter.getNearbyPlayers(this.playerState.id);
    
    console.log(`${nearbyPlayers.length} jogadores próximos detectados`);
    
    // Para cada jogador próximo, estabelecer conexão se ainda não existir
    nearbyPlayers.forEach(player => {
      this.establishConnection(player.playerId);
    });
    
    // Pode adicionar lógica para desconectar jogadores que não estão mais próximos
  }
  
  // Estabelece conexão VOIP com outro jogador
  establishConnection(targetPlayerId) {
    // Em uma implementação real, isso criaria uma conexão WebRTC peer-to-peer
    const connectionId = [this.playerState.id, targetPlayerId].sort().join('-');
    
    if (this.activeConnections[connectionId]) {
      // Conexão já existe
      return;
    }
    
    console.log(`Estabelecendo conexão VOIP com jogador ${targetPlayerId}`);
    
    this.activeConnections[connectionId] = {
      peerId: targetPlayerId,
      established: Date.now(),
      active: true
    };
    
    // Aqui seria criada a conexão peer-to-peer real usando WebRTC
    
    return connectionId;
  }
  
  // Cria uma sala VOIP
  createRoom(roomName, isGuildRoom = false) {
    const roomId = `room-${Date.now()}-${Math.floor(Math.random() * 10000)}`;
    
    const room = new AudioChannel(
      roomId,
      isGuildRoom ? 'guild' : 'group',
      [this.playerState.id]
    );
    
    this.channels[roomId] = room;
    this.audioRouter.addChannel(room);
    this.playerState.activeChannels.push(roomId);
    
    console.log(`Sala VOIP criada: ${roomName} (ID: ${roomId})`);
    
    return {
      id: roomId,
      name: roomName,
      type: isGuildRoom ? 'guild' : 'group',
      createdAt: Date.now(),
      ownerId: this.playerState.id
    };
  }
  
  // Entra em uma sala VOIP
  joinRoom(roomId) {
    const room = this.channels[roomId];
    
    if (!room) {
      console.error(`Sala não encontrada: ${roomId}`);
      return false;
    }
    
    // Adicionar jogador à sala
    room.addMember(this.playerState.id);
    
    // Adicionar sala aos canais ativos do jogador
    if (!this.playerState.activeChannels.includes(roomId)) {
      this.playerState.activeChannels.push(roomId);
    }
    
    console.log(`Entrou na sala VOIP: ${roomId}`);
    
    // Estabelecer conexão com outros membros da sala
    room.members.forEach(memberId => {
      if (memberId !== this.playerState.id) {
        this.establishConnection(memberId);
      }
    });
    
    return true;
  }
  
  // Sai de uma sala VOIP
  leaveRoom(roomId) {
    const room = this.channels[roomId];
    
    if (!room) {
      console.error(`Sala não encontrada: ${roomId}`);
      return false;
    }
    
    // Remover jogador da sala
    room.removeMember(this.playerState.id);
    
    // Remover sala dos canais ativos do jogador
    const index = this.playerState.activeChannels.indexOf(roomId);
    if (index !== -1) {
      this.playerState.activeChannels.splice(index, 1);
    }
    
    console.log(`Saiu da sala VOIP: ${roomId}`);
    
    // Se a sala ficou vazia, remover completamente
    if (room.members.length === 0) {
      delete this.channels[roomId];
      this.audioRouter.removeChannel(roomId);
      console.log(`Sala VOIP removida: ${roomId}`);
    }
    
    return true;
  }
  
  // Ativa/desativa microfone
  toggleMicrophone(active = null) {
    if (active === null) {
      active = !this.playerState.microphoneActive;
    }
    
    this.playerState.microphoneActive = active;
    
    // Ativar/desativar trilhas de áudio no stream local
    if (this.localStream) {
      this.localStream.getAudioTracks().forEach(track => {
        track.enabled = active;
      });
    }
    
    console.log(`Microfone ${active ? 'ativado' : 'desativado'}`);
    return active;
  }
  
  // Ativa/desativa áudio
  toggleAudio(active = null) {
    if (active === null) {
      active = !this.playerState.audioActive;
    }
    
    this.playerState.audioActive = active;
    
    // Em uma implementação real, isso controlaria o áudio dos peers
    
    console.log(`Áudio ${active ? 'ativado' : 'desativado'}`);
    return active;
  }
  
  // Obtém lista de dispositivos de áudio
  async getAudioDevices() {
    try {
      const devices = await navigator.mediaDevices.enumerateDevices();
      
      const audioDevices = {
        inputs: devices.filter(device => device.kind === 'audioinput'),
        outputs: devices.filter(device => device.kind === 'audiooutput')
      };
      
      return audioDevices;
    } catch (error) {
      console.error('Erro ao obter dispositivos de áudio:', error);
      return { inputs: [], outputs: [] };
    }
  }
  
  // Altera o dispositivo de captura de áudio
  async changeAudioInputDevice(deviceId) {
    try {
      // Parar streams existentes
      if (this.localStream) {
        this.localStream.getTracks().forEach(track => track.stop());
      }
      
      // Criar novo stream com o dispositivo selecionado
      this.localStream = await navigator.mediaDevices.getUserMedia({
        audio: {
          ...this.audioConfig,
          deviceId: { exact: deviceId }
        },
        video: false
      });
      
      // Atualizar conexões existentes
      // Aqui seria atualizado o stream em todas as conexões peer
      
      console.log(`Dispositivo de entrada de áudio alterado para: ${deviceId}`);
      return true;
    } catch (error) {
      console.error('Erro ao alterar dispositivo de áudio:', error);
      return false;
    }
  }
  
  // Encerra o sistema VOIP
  shutdown() {
    if (!this.initialized) return true;
    
    // Parar streams locais
    if (this.localStream) {
      this.localStream.getTracks().forEach(track => track.stop());
    }
    
    // Fechar todas as conexões peer
    Object.values(this.peerConnections).forEach(connection => {
      if (connection && connection.close) {
        connection.close();
      }
    });
    
    // Limpar canais
    this.channels = {};
    this.activeConnections = {};
    this.peerConnections = {};
    this.playerState.activeChannels = [];
    
    this.initialized = false;
    console.log('Sistema VOIP encerrado');
    
    return true;
  }
}

module.exports = VoipIntegration;
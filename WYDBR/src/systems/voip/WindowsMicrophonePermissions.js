/**
 * WindowsMicrophonePermissions
 * 
 * Módulo responsável por gerenciar as permissões de microfone no Windows
 * Implementa verificações de permissão e exibe instruções para o usuário
 * caso as permissões não estejam habilitadas.
 */

class WindowsMicrophonePermissions {
  constructor() {
    this.hasPermission = null; // null = não verificado, true/false = resultado
    this.lastError = null;
    this.isWindows = this._detectWindowsOS();
    this.permissionCallbacks = {
      onGranted: [], 
      onDenied: [],
      onChange: []
    };
  }
  
  /**
   * Detecta se o sistema operacional é Windows
   * @private
   * @returns {boolean} Verdadeiro se for Windows
   */
  _detectWindowsOS() {
    if (typeof navigator !== 'undefined') {
      const userAgent = navigator.userAgent.toLowerCase();
      return userAgent.indexOf('windows') !== -1;
    }
    return false; // ambiente sem navigator (Node.js)
  }
  
  /**
   * Verifica se o aplicativo tem permissão para usar o microfone
   * @returns {Promise<boolean>} Promise que resolve para true se tiver permissão
   */
  async checkPermission() {
    try {
      // Se não estiver no Windows, assumimos que a permissão é tratada diferentemente
      if (!this.isWindows) {
        this.hasPermission = true;
        return true;
      }
      
      if (typeof navigator === 'undefined' || !navigator.mediaDevices || !navigator.mediaDevices.getUserMedia) {
        throw new Error('API de mídia não disponível neste ambiente');
      }
      
      // Tenta obter acesso ao microfone
      const stream = await navigator.mediaDevices.getUserMedia({ audio: true, video: false });
      
      // Se chegou aqui, temos permissão
      this.hasPermission = true;
      
      // Para não deixar o microfone ativo, paramos as trilhas de áudio
      if (stream && stream.getTracks) {
        stream.getTracks().forEach(track => track.stop());
      }
      
      // Notificar callbacks
      this._notifyPermissionChange(true);
      
      return true;
    } catch (error) {
      this.lastError = error;
      
      // Determinar se o erro foi de permissão negada
      if (error.name === 'NotAllowedError' || error.name === 'PermissionDeniedError') {
        this.hasPermission = false;
        this._notifyPermissionChange(false);
        return false;
      }
      
      // Outro tipo de erro (hardware, etc)
      console.error('Erro ao verificar permissão de microfone:', error);
      this.hasPermission = false;
      this._notifyPermissionChange(false);
      return false;
    }
  }
  
  /**
   * Notifica os callbacks registrados sobre mudanças de permissão
   * @private
   * @param {boolean} granted Status da permissão
   */
  _notifyPermissionChange(granted) {
    // Notificar callbacks específicos
    const callbackList = granted 
      ? this.permissionCallbacks.onGranted 
      : this.permissionCallbacks.onDenied;
    
    // Executar callbacks
    callbackList.forEach(callback => {
      try {
        callback();
      } catch (e) {
        console.error('Erro ao executar callback de permissão:', e);
      }
    });
    
    // Notificar callbacks de mudança
    this.permissionCallbacks.onChange.forEach(callback => {
      try {
        callback(granted);
      } catch (e) {
        console.error('Erro ao executar callback de mudança de permissão:', e);
      }
    });
  }
  
  /**
   * Registra callbacks para eventos de permissão
   * @param {string} event 'granted', 'denied' ou 'change'
   * @param {Function} callback Função a ser chamada quando o evento ocorrer
   */
  on(event, callback) {
    if (typeof callback !== 'function') {
      throw new Error('Callback deve ser uma função');
    }
    
    switch (event) {
      case 'granted':
        this.permissionCallbacks.onGranted.push(callback);
        // Se já temos permissão, chamar imediatamente
        if (this.hasPermission === true) callback();
        break;
      case 'denied':
        this.permissionCallbacks.onDenied.push(callback);
        // Se já foi negada, chamar imediatamente
        if (this.hasPermission === false) callback();
        break;
      case 'change':
        this.permissionCallbacks.onChange.push(callback);
        // Se já foi verificado, chamar imediatamente com o status atual
        if (this.hasPermission !== null) callback(this.hasPermission);
        break;
      default:
        throw new Error(`Evento desconhecido: ${event}`);
    }
    
    return this; // Para encadeamento
  }
  
  /**
   * Retorna instruções sobre como habilitar o microfone no Windows
   * @returns {Object} Objeto com as instruções detalhadas
   */
  getEnablingInstructions() {
    if (!this.isWindows) {
      return {
        title: 'Habilitar Microfone',
        subtitle: 'Instruções para seu dispositivo',
        steps: [
          'Consulte a documentação do seu sistema operacional para habilitar o microfone.',
          'Verifique as configurações de privacidade do seu navegador ou aplicativo.'
        ]
      };
    }
    
    // Instruções específicas para Windows 10/11
    return {
      title: 'Como Habilitar o Microfone no Windows',
      subtitle: 'Siga estas etapas para permitir que o WYD acesse seu microfone',
      steps: [
        'Clique no botão Iniciar do Windows e selecione "Configurações" (ícone de engrenagem).',
        'Clique em "Privacidade".',
        'No menu à esquerda, selecione "Microfone".',
        'Verifique se "Permitir que aplicativos acessem seu microfone" está Ativado.',
        'Procure o WYD na lista de aplicativos e certifique-se de que esteja ativado.',
        'Se o WYD não estiver na lista, reinstale o aplicativo.',
        'Reinicie o WYD após habilitar as permissões.'
      ],
      note: 'Se você negou o acesso permanentemente, pode ser necessário reiniciar o Windows para que as novas configurações tenham efeito.'
    };
  }
  
  /**
   * Gera HTML com instruções para habilitar o microfone
   * @returns {string} HTML formatado com as instruções
   */
  getInstructionsHTML() {
    const instructions = this.getEnablingInstructions();
    
    // Constrói o HTML para as instruções
    let html = `
      <div class="microphone-instructions">
        <h3>${instructions.title}</h3>
        <p>${instructions.subtitle}</p>
        <ol>
    `;
    
    // Adiciona cada passo como um item da lista
    instructions.steps.forEach(step => {
      html += `<li>${step}</li>`;
    });
    
    html += `
        </ol>
    `;
    
    // Adiciona nota se existir
    if (instructions.note) {
      html += `<p class="note"><strong>Nota:</strong> ${instructions.note}</p>`;
    }
    
    html += `</div>`;
    
    return html;
  }
  
  /**
   * Exibe um diálogo modal com instruções para habilitar o microfone
   * @param {HTMLElement} container Elemento onde o modal será anexado (opcional)
   * @returns {HTMLElement} O elemento do modal criado
   */
  showInstructionsModal(container = document.body) {
    // Criar o elemento do modal
    const modalElement = document.createElement('div');
    modalElement.className = 'microphone-instructions-modal';
    modalElement.style.position = 'fixed';
    modalElement.style.top = '0';
    modalElement.style.left = '0';
    modalElement.style.width = '100%';
    modalElement.style.height = '100%';
    modalElement.style.backgroundColor = 'rgba(0, 0, 0, 0.7)';
    modalElement.style.display = 'flex';
    modalElement.style.justifyContent = 'center';
    modalElement.style.alignItems = 'center';
    modalElement.style.zIndex = '10000';
    
    // Criar o conteúdo do modal
    const modalContent = document.createElement('div');
    modalContent.className = 'modal-content';
    modalContent.style.backgroundColor = '#fff';
    modalContent.style.padding = '20px';
    modalContent.style.borderRadius = '8px';
    modalContent.style.maxWidth = '500px';
    modalContent.style.maxHeight = '80vh';
    modalContent.style.overflowY = 'auto';
    modalContent.style.color = '#333';
    
    // Adicionar o conteúdo HTML com as instruções
    modalContent.innerHTML = this.getInstructionsHTML();
    
    // Adicionar botão de fechar
    const closeButton = document.createElement('button');
    closeButton.textContent = 'Entendi';
    closeButton.style.backgroundColor = '#4c84ff';
    closeButton.style.color = 'white';
    closeButton.style.border = 'none';
    closeButton.style.padding = '10px 15px';
    closeButton.style.borderRadius = '4px';
    closeButton.style.cursor = 'pointer';
    closeButton.style.marginTop = '15px';
    closeButton.style.display = 'block';
    closeButton.style.marginLeft = 'auto';
    closeButton.style.marginRight = 'auto';
    
    // Adicionar evento para fechar o modal
    closeButton.addEventListener('click', () => {
      modalElement.parentNode.removeChild(modalElement);
    });
    
    // Montar a estrutura do modal
    modalContent.appendChild(closeButton);
    modalElement.appendChild(modalContent);
    
    // Adicionar ao container
    container.appendChild(modalElement);
    
    return modalElement;
  }
  
  /**
   * Solicita permissão para usar o microfone e mostra instruções se negado
   * @param {Object} options Opções de configuração
   * @returns {Promise<boolean>} Promise que resolve para true se a permissão for concedida
   */
  async requestPermissionWithUI(options = {}) {
    const defaults = {
      showInstructionsOnDenied: true,
      container: document.body,
      retryButton: true,
      onRetry: null
    };
    
    const config = { ...defaults, ...options };
    
    // Verificar permissão
    const hasPermission = await this.checkPermission();
    
    // Se não tiver permissão e showInstructionsOnDenied estiver ativado
    if (!hasPermission && config.showInstructionsOnDenied) {
      const modal = this.showInstructionsModal(config.container);
      
      // Adicionar botão de tentar novamente, se necessário
      if (config.retryButton) {
        const modalContent = modal.querySelector('.modal-content');
        const retryButton = document.createElement('button');
        retryButton.textContent = 'Tentar Novamente';
        retryButton.style.backgroundColor = '#4CAF50';
        retryButton.style.color = 'white';
        retryButton.style.border = 'none';
        retryButton.style.padding = '10px 15px';
        retryButton.style.borderRadius = '4px';
        retryButton.style.cursor = 'pointer';
        retryButton.style.marginTop = '10px';
        retryButton.style.marginRight = '10px';
        
        retryButton.addEventListener('click', async () => {
          // Remover o modal
          modal.parentNode.removeChild(modal);
          
          // Verificar permissão novamente
          const newPermissionStatus = await this.checkPermission();
          
          // Callback de tentativa, se fornecido
          if (typeof config.onRetry === 'function') {
            config.onRetry(newPermissionStatus);
          }
          
          // Se ainda não tiver permissão, mostrar o modal novamente
          if (!newPermissionStatus && config.showInstructionsOnDenied) {
            this.showInstructionsModal(config.container);
          }
        });
        
        // Adicionar antes do botão de fechar
        modalContent.insertBefore(retryButton, modalContent.lastChild);
      }
    }
    
    return hasPermission;
  }
}

module.exports = WindowsMicrophonePermissions;
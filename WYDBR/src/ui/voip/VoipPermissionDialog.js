/**
 * VoipPermissionDialog Component
 * 
 * Componente UI que exibe um diálogo de permissão para o microfone
 * integrado com o sistema de permissões do Windows.
 */

class VoipPermissionDialog {
  constructor(options = {}) {
    this.options = {
      containerId: 'voip-permission-dialog',
      onGrant: null,
      onDeny: null,
      autoShow: true,
      title: 'Permissão de Microfone',
      description: 'O WYDBR precisa de acesso ao seu microfone para funcionar corretamente.',
      closeOnGrant: true,
      darkMode: true,
      ...options
    };
    
    this.container = null;
    this.dialog = null;
    this.permissionManager = null;
    this.isVisible = false;
    
    if (this.options.autoShow) {
      this.initialize();
    }
  }
  
  /**
   * Inicializa o componente
   * @returns {VoipPermissionDialog} Instância para encadeamento
   */
  initialize() {
    // Verificar se já existe o container
    let container = document.getElementById(this.options.containerId);
    
    if (!container) {
      container = document.createElement('div');
      container.id = this.options.containerId;
      document.body.appendChild(container);
    }
    
    this.container = container;
    
    // Criar o gerenciador de permissões
    try {
      const WindowsMicrophonePermissions = require('../../systems/voip/WindowsMicrophonePermissions');
      this.permissionManager = new WindowsMicrophonePermissions();
    } catch (error) {
      console.error('Erro ao inicializar gerenciador de permissões:', error);
      this.permissionManager = null;
    }
    
    // Verificar permissão atual
    if (this.permissionManager) {
      this.permissionManager.on('granted', () => {
        if (this.options.closeOnGrant) {
          this.hide();
        }
        
        if (typeof this.options.onGrant === 'function') {
          this.options.onGrant();
        }
      });
      
      this.permissionManager.on('denied', () => {
        if (typeof this.options.onDeny === 'function') {
          this.options.onDeny();
        }
      });
      
      // Verificação inicial
      this.permissionManager.checkPermission().then(hasPermission => {
        if (!hasPermission && this.options.autoShow) {
          this.show();
        }
      });
    }
    
    return this;
  }
  
  /**
   * Exibe o diálogo de permissão
   * @returns {VoipPermissionDialog} Instância para encadeamento
   */
  show() {
    if (this.isVisible) return this;
    
    // Criar o diálogo
    const dialog = document.createElement('div');
    dialog.className = 'voip-permission-dialog';
    
    const colorClass = this.options.darkMode ? 'dark' : 'light';
    
    // Aplicar estilos
    dialog.style.position = 'fixed';
    dialog.style.top = '0';
    dialog.style.left = '0';
    dialog.style.width = '100%';
    dialog.style.height = '100%';
    dialog.style.display = 'flex';
    dialog.style.justifyContent = 'center';
    dialog.style.alignItems = 'center';
    dialog.style.backgroundColor = this.options.darkMode 
      ? 'rgba(0, 0, 0, 0.8)' 
      : 'rgba(255, 255, 255, 0.8)';
    dialog.style.zIndex = '9999';
    
    // Criar o conteúdo
    const content = document.createElement('div');
    content.className = 'voip-dialog-content ' + colorClass;
    content.style.backgroundColor = this.options.darkMode ? '#1E2329' : '#FFFFFF';
    content.style.color = this.options.darkMode ? '#FFFFFF' : '#333333';
    content.style.borderRadius = '8px';
    content.style.padding = '24px';
    content.style.width = '450px';
    content.style.maxWidth = '90%';
    content.style.boxShadow = '0 2px 10px rgba(0, 0, 0, 0.25)';
    
    // Incluir título
    const title = document.createElement('h3');
    title.textContent = this.options.title;
    title.style.margin = '0 0 16px 0';
    title.style.fontSize = '18px';
    title.style.fontWeight = '600';
    
    // Incluir descrição
    const description = document.createElement('p');
    description.textContent = this.options.description;
    description.style.margin = '0 0 24px 0';
    description.style.fontSize = '14px';
    description.style.lineHeight = '1.5';
    
    // Incluir icon do microfone
    const micIcon = document.createElement('div');
    micIcon.className = 'voip-mic-icon';
    micIcon.style.width = '64px';
    micIcon.style.height = '64px';
    micIcon.style.margin = '0 auto 24px auto';
    micIcon.style.backgroundImage = `url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='${this.options.darkMode ? 'white' : 'black'}'%3E%3Cpath d='M12 14c1.66 0 3-1.34 3-3V5c0-1.66-1.34-3-3-3S9 3.34 9 5v6c0 1.66 1.34 3 3 3zm5.91-3c-.49 0-.9.36-.98.85C16.52 14.2 14.47 16 12 16c-2.47 0-4.52-1.8-4.93-4.15-.08-.49-.49-.85-.98-.85-.61 0-1.09.54-1 1.14.49 3 2.89 5.35 5.91 5.78V20c0 .55.45 1 1 1s1-.45 1-1v-2.08c3.02-.43 5.42-2.78 5.91-5.78.1-.6-.39-1.14-1-1.14z'/%3E%3C/svg%3E")`;
    micIcon.style.backgroundSize = 'contain';
    micIcon.style.backgroundRepeat = 'no-repeat';
    micIcon.style.backgroundPosition = 'center';
    
    // Criar botões
    const buttonGroup = document.createElement('div');
    buttonGroup.style.display = 'flex';
    buttonGroup.style.justifyContent = 'space-between';
    buttonGroup.style.marginTop = '24px';
    
    // Botão de permitir
    const allowButton = document.createElement('button');
    allowButton.textContent = 'Permitir Acesso';
    allowButton.style.backgroundColor = '#4c84ff';
    allowButton.style.color = 'white';
    allowButton.style.border = 'none';
    allowButton.style.padding = '10px 16px';
    allowButton.style.borderRadius = '4px';
    allowButton.style.cursor = 'pointer';
    allowButton.style.fontSize = '14px';
    
    allowButton.addEventListener('click', () => {
      this.requestPermission();
    });
    
    // Botão de mais tarde
    const laterButton = document.createElement('button');
    laterButton.textContent = 'Mais Tarde';
    laterButton.style.backgroundColor = 'transparent';
    laterButton.style.color = this.options.darkMode ? '#9e9e9e' : '#666666';
    laterButton.style.border = `1px solid ${this.options.darkMode ? '#9e9e9e' : '#666666'}`;
    laterButton.style.padding = '10px 16px';
    laterButton.style.borderRadius = '4px';
    laterButton.style.cursor = 'pointer';
    laterButton.style.fontSize = '14px';
    
    laterButton.addEventListener('click', () => {
      this.hide();
      if (typeof this.options.onDeny === 'function') {
        this.options.onDeny();
      }
    });
    
    // Adicionar botões ao grupo
    buttonGroup.appendChild(laterButton);
    buttonGroup.appendChild(allowButton);
    
    // Montar a estrutura do diálogo
    content.appendChild(micIcon);
    content.appendChild(title);
    content.appendChild(description);
    
    // Adicionar instruções se disponíveis
    if (this.permissionManager) {
      const instructionsContainer = document.createElement('div');
      instructionsContainer.className = 'voip-permission-instructions';
      instructionsContainer.style.marginTop = '16px';
      instructionsContainer.style.fontSize = '13px';
      instructionsContainer.style.backgroundColor = this.options.darkMode ? 'rgba(255, 255, 255, 0.05)' : 'rgba(0, 0, 0, 0.05)';
      instructionsContainer.style.padding = '12px';
      instructionsContainer.style.borderRadius = '4px';
      
      const instructionsTitle = document.createElement('p');
      instructionsTitle.textContent = 'Como permitir o acesso:';
      instructionsTitle.style.fontWeight = 'bold';
      instructionsTitle.style.margin = '0 0 8px 0';
      
      const instructionsList = document.createElement('ol');
      instructionsList.style.margin = '0';
      instructionsList.style.paddingLeft = '24px';
      
      const instructions = this.permissionManager.getEnablingInstructions();
      
      instructions.steps.slice(0, 3).forEach(step => {
        const item = document.createElement('li');
        item.textContent = step;
        item.style.marginBottom = '4px';
        instructionsList.appendChild(item);
      });
      
      instructionsContainer.appendChild(instructionsTitle);
      instructionsContainer.appendChild(instructionsList);
      
      content.appendChild(instructionsContainer);
    }
    
    content.appendChild(buttonGroup);
    dialog.appendChild(content);
    
    // Adicionar ao container
    this.container.appendChild(dialog);
    this.dialog = dialog;
    this.isVisible = true;
    
    return this;
  }
  
  /**
   * Esconde o diálogo de permissão
   * @returns {VoipPermissionDialog} Instância para encadeamento
   */
  hide() {
    if (!this.isVisible || !this.dialog) return this;
    
    if (this.dialog.parentNode) {
      this.dialog.parentNode.removeChild(this.dialog);
    }
    
    this.dialog = null;
    this.isVisible = false;
    
    return this;
  }
  
  /**
   * Solicita permissão para o microfone
   * @returns {Promise<boolean>} Promise que resolve para true se a permissão foi concedida
   */
  async requestPermission() {
    if (!this.permissionManager) {
      console.error('Gerenciador de permissões não disponível');
      return false;
    }
    
    try {
      const hasPermission = await this.permissionManager.requestPermissionWithUI({
        container: this.container,
        showInstructionsOnDenied: true,
        retryButton: true
      });
      
      return hasPermission;
    } catch (error) {
      console.error('Erro ao solicitar permissão:', error);
      return false;
    }
  }
}

// Exportar para uso em navegador e Node.js
if (typeof module !== 'undefined' && module.exports) {
  module.exports = VoipPermissionDialog;
} else if (typeof window !== 'undefined') {
  window.VoipPermissionDialog = VoipPermissionDialog;
}
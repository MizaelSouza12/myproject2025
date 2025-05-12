/**
 * WYDBRASIL - JavaScript do Compilador
 */

document.addEventListener('DOMContentLoaded', () => {
    const fileTypes = document.querySelectorAll('.file-type');
    const selectFileBtn = document.getElementById('selectFileBtn');
    const fileInput = document.getElementById('fileInput');
    const selectedFileElement = document.getElementById('selectedFile');
    const compileBtn = document.getElementById('compileBtn');
    const resetBtn = document.getElementById('resetBtn');
    const consoleOutput = document.getElementById('consoleOutput');
    const outputPathInput = document.getElementById('outputPath');
    const formatSelector = document.getElementById('formatSelector');
    
    let selectedFileType = 'json'; // Padrão: JSON para BIN
    let selectedFile = null;
    
    // Seleciona o tipo de conversão (JSON → BIN ou BIN → JSON)
    fileTypes.forEach(type => {
        type.addEventListener('click', () => {
            fileTypes.forEach(t => t.classList.remove('active'));
            type.classList.add('active');
            selectedFileType = type.dataset.type;
            
            // Atualiza o texto do botão de compilação com base no tipo selecionado
            compileBtn.textContent = selectedFileType === 'json' ? 'Compilar para BIN' : 'Decodificar para JSON';
            
            // Limpa a seleção de arquivo quando o tipo é alterado
            resetSelection();
            
            logToConsole(`Modo de conversão alterado para: ${selectedFileType === 'json' ? 'JSON → BIN' : 'BIN → JSON'}`);
        });
    });
    
    // Ativa o primeiro tipo de arquivo por padrão
    fileTypes[0].classList.add('active');
    
    // Evento para o botão de seleção de arquivo
    selectFileBtn.addEventListener('click', () => {
        fileInput.click();
    });
    
    // Evento quando um arquivo é selecionado
    fileInput.addEventListener('change', (event) => {
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param event.target.files.length > 0 Parâmetro event.target.files.length > 0
 */

        if(event.target.files.length > 0) {
            selectedFile = event.target.files[0];
            selectedFileElement.textContent = selectedFile.name;
            
            // Define automaticamente o caminho de saída sugerido
            const extension = selectedFileType === 'json' ? '.bin' : '.json';
            const originalExt = selectedFileType === 'json' ? '.json' : '.bin';
            outputPathInput.value = selectedFile.name.replace(originalExt, extension);
            
            logToConsole(`Arquivo selecionado: ${selectedFile.name}`, 'success');
        }
    });
    
    // Evento para o botão de compilação
    compileBtn.addEventListener('click', () => {
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !selectedFile Parâmetro !selectedFile
 */

        if(!selectedFile) {
            logToConsole('Erro: Nenhum arquivo selecionado!', 'error');
            return;
        }
        
        // Exibe mensagem de processamento
        logToConsole(`Processando ${selectedFile.name}...`);
        
        // Simula envio para o servidor
        setTimeout(() => {
            mockApiCall();
        }, 1500);
    });
    
    // Evento para o botão de reset
    resetBtn.addEventListener('click', () => {
        resetSelection();
        logToConsole('Seleção de arquivo limpa');
    });
    
    // Função para simular uma chamada API (para demonstração)
    /**
 * mockApiCall
 * 
 * Função que implementa mockApiCall
 * @returns {*} Valor retornado pela função
 */
function /**
 * mockApiCall
 * 
 * Implementa a funcionalidade mockApiCall conforme especificação original.
 */
 mockApiCall() {
        const endpoint = selectedFileType === 'json' ? '/api/compile' : '/api/decompile';
        const outputPath = outputPathInput.value;
        const format = formatSelector.value;
        const createBackup = document.getElementById('backup').checked;
        
        logToConsole(`Enviando para ${endpoint}`);
        logToConsole(`Parâmetros: outputPath=${outputPath}, format=${format}, backup=${createBackup}`);
        
        // Simula processamento
        setTimeout(() => {
            const success = Math.random() > 0.2; // 80% de chance de sucesso para demonstração
            
            /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param success Parâmetro success
 */

            
            if(success) {
                logToConsole(`Arquivo processado com sucesso: ${outputPath}`, 'success');
            } else {
                logToConsole(`Erro durante o processamento: Formato incompatível`, 'error');
            }
        }, 2000);
    }
    
    // Função para resetar a seleção de arquivo
    /**
 * resetSelection
 * 
 * Função que implementa resetSelection
 * @returns {*} Valor retornado pela função
 */
function /**
 * resetSelection
 * 
 * Implementa a funcionalidade resetSelection conforme especificação original.
 */
 resetSelection() {
        fileInput.value = '';
        selectedFile = null;
        selectedFileElement.textContent = 'Nenhum arquivo selecionado';
        outputPathInput.value = '';
    }
    
    // Função para adicionar logs ao console
    /**
 * logToConsole
 * 
 * Função que implementa logToConsole
 * @param {*} message Parâmetro message
 * @param {*} type = '' Parâmetro type = ''
 * @returns {*} Valor retornado pela função
 */
function /**
 * logToConsole
 * 
 * Implementa a funcionalidade logToConsole conforme especificação original.
 * @param message Parâmetro message
 * @param type = '' Parâmetro type = ''
 */
 logToConsole(message, type = '') {
        const line = document.createElement('div');
        line.className = `console-line ${type}`;
        line.textContent = message;
        
        consoleOutput.appendChild(line);
        consoleOutput.scrollTop = consoleOutput.scrollHeight; // Rola para o final
    }
});
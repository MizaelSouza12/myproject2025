/**
 * WYDBRASIL - Servidor Básico
 * 
 * Este servidor Express básico fornece acesso à interface web e endpoints da API
 * para o projeto WYDBRASIL. Ele permite acesso aos recursos do projeto via web.
 */

const express = require('express');
const path = require('path');
const fs = require('fs');
const chalk = require('chalk');
const http = require('http');
const os = require('os');

// Criar aplicação Express
const app = express();
const PORT = 5000; // Porta principal para o servidor

// Implementando um mecanismo para manter o servidor rodando
process.on('uncaughtException', (err) => {
    console.error(chalk.red(`[ERRO] Exceção não capturada: ${err.message}`));
    console.error(err.stack);
});

// Configurar middleware
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

// Servir arquivos estáticos
app.use(express.static(path.join(__dirname, 'public')));

// Variáveis para manter estatísticas do servidor
let startTime = new Date();
let requestCount = 0;

// Rota principal
app.get('/', (req, res) => {
    requestCount++;
    res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

// Rota para o compilador
app.get('/compiler', (req, res) => {
    requestCount++;
    console.log(new Date().toISOString() + ' - GET /compiler');
    res.redirect('http://localhost:5100/compiler');
});

// API para informações do sistema
app.get('/api/system', (req, res) => {
    requestCount++;
    const uptime = new Date() - startTime;
    
    res.json({
        status: 'online',
        name: 'WYDBRASIL',
        version: '1.0.0',
        uptime: formatUptime(uptime),
        requests: requestCount,
        system: {
            platform: os.platform(),
            arch: os.arch(),
            cpus: os.cpus().length,
            memory: {
                total: Math.round(os.totalmem() / (1024 * 1024 * 1024)) + 'GB',
                free: Math.round(os.freemem() / (1024 * 1024 * 1024)) + 'GB'
            }
        },
        workflows: getWorkflowInfo()
    });
});

// Rota para teste de conexão
app.get('/ping', (req, res) => {
    res.send('pong');
});

// Função auxiliar para formatar o tempo de execução
/**
 * formatUptime
 * 
 * Função que implementa formatUptime
 * @param {*} uptime Parâmetro uptime
 * @returns {*} Valor retornado pela função
 */
function /**
 * formatUptime
 * 
 * Implementa a funcionalidade formatUptime conforme especificação original.
 * @param uptime Parâmetro uptime
 */
 formatUptime(uptime) {
    const seconds = Math.floor(uptime / 1000) % 60;
    const minutes = Math.floor(uptime / (1000 * 60)) % 60;
    const hours = Math.floor(uptime / (1000 * 60 * 60)) % 24;
    const days = Math.floor(uptime / (1000 * 60 * 60 * 24));
    
    return `${days}d ${hours}h ${minutes}m ${seconds}s`;
}

// Função para obter informações sobre workflows configurados
/**
 * getWorkflowInfo
 * 
 * Função que implementa getWorkflowInfo
 * @returns {*} Valor retornado pela função
 */
function /**
 * getWorkflowInfo
 * 
 * Implementa a funcionalidade getWorkflowInfo conforme especificação original.
 */
 getWorkflowInfo() {
    return [
        { name: 'WYDBRASIL Compiler', port: 5100, status: 'active' },
        { name: 'WYDBRASIL Server', port: 5000, status: 'active' }
    ];
}

// Criar servidor HTTP
const server = http.createServer(app);

// Iniciar servidor
server.listen(PORT, '0.0.0.0', () => {
    console.log(chalk.blue('======================================================'));
    console.log(chalk.green(`  WYDBRASIL - Servidor Web iniciado!`));
    console.log(chalk.blue('======================================================'));
    console.log(chalk.yellow(`Servidor rodando em http://0.0.0.0:${PORT}`));
    console.log(chalk.yellow(`Acesse http://localhost:${PORT} no navegador`));
});

// Manter o servidor vivo periodicamente
setInterval(() => {
    const currentTime = new Date();
    const uptime = currentTime - startTime;
    console.log(`[${currentTime.toISOString()}] Servidor ativo - Uptime: ${formatUptime(uptime)}`);
}, 300000);
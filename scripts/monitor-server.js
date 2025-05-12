require('dotenv').config();
const si = require('systeminformation');
const winston = require('winston');
const nodemailer = require('nodemailer');
const { WebhookClient } = require('discord.js');
const mysql = require('mysql2/promise');
const schedule = require('node-schedule');

// Configuração do logger
const logger = winston.createLogger({
    level: 'info',
    format: winston.format.combine(
        winston.format.timestamp(),
        winston.format.json()
    ),
    transports: [
        new winston.transports.File({ filename: 'logs/monitor.log' }),
        new winston.transports.Console({
            format: winston.format.combine(
                winston.format.colorize(),
                winston.format.simple()
            )
        })
    ]
});

// Configuração do Discord Webhook
const webhookClient = new WebhookClient({ url: process.env.DISCORD_WEBHOOK_URL });

// Configuração do email
const emailTransporter = nodemailer.createTransport({
    host: process.env.EMAIL_SMTP_HOST,
    port: process.env.EMAIL_SMTP_PORT,
    secure: false,
    auth: {
        user: process.env.EMAIL_USER,
        pass: process.env.EMAIL_PASS
    }
});

// Configuração do banco de dados
const dbConfig = {
    host: process.env.DATABASE_HOST,
    port: process.env.DATABASE_PORT,
    user: process.env.DATABASE_USER,
    password: process.env.DATABASE_PASS,
    database: process.env.DATABASE_NAME
};

// Limites de alerta
const THRESHOLDS = {
    CPU_USAGE: 80,
    MEMORY_USAGE: 85,
    DISK_USAGE: 90,
    NETWORK_LATENCY: 100
};

// Função para enviar alertas
async function sendAlert(type, message) {
    try {
        // Envia para o Discord
        await webhookClient.send({
            content: `🚨 **Alerta ${type}**\n${message}`
        });

        // Envia por email
        await emailTransporter.sendMail({
            from: process.env.EMAIL_USER,
            to: process.env.EMAIL_USER,
            subject: `Alerta WYDBR2.0 - ${type}`,
            text: message
        });

        logger.info(`Alerta enviado: ${type} - ${message}`);
    } catch (error) {
        logger.error('Erro ao enviar alerta:', error);
    }
}

// Função para coletar métricas do sistema
async function collectMetrics() {
    try {
        const [cpu, mem, disk, netStats] = await Promise.all([
            si.currentLoad(),
            si.mem(),
            si.fsSize(),
            si.networkStats()
        ]);

        const metrics = {
            cpu: cpu.currentLoad,
            memory: (mem.used / mem.total) * 100,
            disk: disk[0].use,
            network: {
                rx: netStats[0].rx_sec,
                tx: netStats[0].tx_sec
            }
        };

        // Verifica limites
        if (metrics.cpu > THRESHOLDS.CPU_USAGE) {
            await sendAlert('CPU', `Uso de CPU alto: ${metrics.cpu.toFixed(2)}%`);
        }
        if (metrics.memory > THRESHOLDS.MEMORY_USAGE) {
            await sendAlert('Memória', `Uso de memória alto: ${metrics.memory.toFixed(2)}%`);
        }
        if (metrics.disk > THRESHOLDS.DISK_USAGE) {
            await sendAlert('Disco', `Uso de disco alto: ${metrics.disk.toFixed(2)}%`);
        }

        // Salva métricas no banco
        const connection = await mysql.createConnection(dbConfig);
        await connection.execute(
            'INSERT INTO server_metrics (cpu_usage, memory_usage, disk_usage, network_rx, network_tx) VALUES (?, ?, ?, ?, ?)',
            [metrics.cpu, metrics.memory, metrics.disk, metrics.network.rx, metrics.network.tx]
        );
        await connection.end();

        logger.info('Métricas coletadas com sucesso', metrics);
    } catch (error) {
        logger.error('Erro ao coletar métricas:', error);
    }
}

// Função para verificar saúde do servidor
async function checkServerHealth() {
    try {
        const connection = await mysql.createConnection(dbConfig);
        await connection.ping();
        await connection.end();
        logger.info('Servidor está saudável');
        return true;
    } catch (error) {
        logger.error('Erro na verificação de saúde:', error);
        await sendAlert('Servidor', 'Servidor não está respondendo!');
        return false;
    }
}

// Agenda as tarefas
schedule.scheduleJob('*/5 * * * *', collectMetrics);  // A cada 5 minutos
schedule.scheduleJob('*/1 * * * *', checkServerHealth);  // A cada 1 minuto

logger.info('Monitor do servidor iniciado');

// Tratamento de erros não capturados
process.on('uncaughtException', async (error) => {
    logger.error('Erro não capturado:', error);
    await sendAlert('Sistema', `Erro crítico: ${error.message}`);
});

process.on('unhandledRejection', async (error) => {
    logger.error('Promise rejeitada não tratada:', error);
    await sendAlert('Sistema', `Erro assíncrono: ${error.message}`);
}); 
require('dotenv').config();
const mysql = require('mysql2/promise');
const { execSync } = require('child_process');
const fs = require('fs-extra');
const path = require('path');
const archiver = require('archiver');
const winston = require('winston');
const schedule = require('node-schedule');
const { WebhookClient } = require('discord.js');

// Configuração do logger
const logger = winston.createLogger({
    level: 'info',
    format: winston.format.combine(
        winston.format.timestamp(),
        winston.format.json()
    ),
    transports: [
        new winston.transports.File({ filename: 'logs/backup.log' }),
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

// Configuração do banco de dados
const dbConfig = {
    host: process.env.DATABASE_HOST,
    port: process.env.DATABASE_PORT,
    user: process.env.DATABASE_USER,
    password: process.env.DATABASE_PASS,
    database: process.env.DATABASE_NAME
};

// Diretórios importantes
const BACKUP_DIR = path.join(process.cwd(), 'backup');
const DATA_DIR = path.join(process.cwd(), 'data');

// Função para criar nome do arquivo de backup
function getBackupFileName(type) {
    const date = new Date();
    return `backup_${type}_${date.getFullYear()}${(date.getMonth() + 1).toString().padStart(2, '0')}${date.getDate().toString().padStart(2, '0')}_${date.getHours().toString().padStart(2, '0')}${date.getMinutes().toString().padStart(2, '0')}.zip`;
}

// Função para compactar diretório
async function compressDirectory(sourceDir, outputFile) {
    return new Promise((resolve, reject) => {
        const output = fs.createWriteStream(outputFile);
        const archive = archiver('zip', {
            zlib: { level: 9 }
        });

        output.on('close', resolve);
        archive.on('error', reject);

        archive.pipe(output);
        archive.directory(sourceDir, false);
        archive.finalize();
    });
}

// Função para fazer backup do banco de dados
async function backupDatabase(type) {
    const fileName = path.join(BACKUP_DIR, `database_${type}_${Date.now()}.sql`);
    
    try {
        const connection = await mysql.createConnection(dbConfig);
        const [rows] = await connection.execute('SHOW TABLES');
        const tables = rows.map(row => Object.values(row)[0]);

        let dumpCommand = `mysqldump -h ${dbConfig.host} -P ${dbConfig.port} -u ${dbConfig.user} -p${dbConfig.password} ${dbConfig.database} ${tables.join(' ')} > "${fileName}"`;
        execSync(dumpCommand);

        await connection.end();
        logger.info(`Backup do banco de dados (${type}) concluído: ${fileName}`);
        return fileName;
    } catch (error) {
        logger.error(`Erro no backup do banco de dados (${type}):`, error);
        throw error;
    }
}

// Função para fazer backup completo
async function fullBackup() {
    try {
        logger.info('Iniciando backup completo...');

        // Backup do banco de dados
        const dbBackupFile = await backupDatabase('full');

        // Backup dos arquivos
        const backupFileName = getBackupFileName('full');
        const backupFilePath = path.join(BACKUP_DIR, backupFileName);

        // Compacta diretório de dados e backup do banco
        await compressDirectory(DATA_DIR, backupFilePath);
        
        // Adiciona o arquivo de backup do banco ao zip
        const archive = archiver('zip', {
            zlib: { level: 9 }
        });
        const output = fs.createWriteStream(backupFilePath);
        archive.pipe(output);
        archive.file(dbBackupFile, { name: path.basename(dbBackupFile) });
        await archive.finalize();

        // Remove arquivo temporário do backup do banco
        fs.unlinkSync(dbBackupFile);

        // Mantém apenas os últimos 7 backups completos
        const files = fs.readdirSync(BACKUP_DIR)
            .filter(file => file.startsWith('backup_full_'))
            .sort()
            .reverse();

        if (files.length > 7) {
            files.slice(7).forEach(file => {
                fs.unlinkSync(path.join(BACKUP_DIR, file));
                logger.info(`Backup antigo removido: ${file}`);
            });
        }

        await webhookClient.send({
            content: `✅ Backup completo realizado com sucesso!\nArquivo: ${backupFileName}\nTamanho: ${(fs.statSync(backupFilePath).size / 1024 / 1024).toFixed(2)}MB`
        });

        logger.info('Backup completo finalizado com sucesso');
    } catch (error) {
        logger.error('Erro no backup completo:', error);
        await webhookClient.send({
            content: `❌ Erro no backup completo: ${error.message}`
        });
    }
}

// Função para fazer backup incremental
async function incrementalBackup() {
    try {
        logger.info('Iniciando backup incremental...');

        // Backup apenas das alterações no banco de dados desde o último backup
        const dbBackupFile = await backupDatabase('incremental');

        // Backup apenas dos arquivos modificados nas últimas 24 horas
        const backupFileName = getBackupFileName('incremental');
        const backupFilePath = path.join(BACKUP_DIR, backupFileName);

        const archive = archiver('zip', {
            zlib: { level: 9 }
        });
        const output = fs.createWriteStream(backupFilePath);
        archive.pipe(output);

        // Adiciona arquivos modificados nas últimas 24 horas
        const oneDayAgo = Date.now() - 24 * 60 * 60 * 1000;
        function addModifiedFiles(dir) {
            const files = fs.readdirSync(dir);
            files.forEach(file => {
                const filePath = path.join(dir, file);
                const stats = fs.statSync(filePath);
                if (stats.isDirectory()) {
                    addModifiedFiles(filePath);
                } else if (stats.mtime.getTime() > oneDayAgo) {
                    archive.file(filePath, { name: path.relative(process.cwd(), filePath) });
                }
            });
        }

        addModifiedFiles(DATA_DIR);
        archive.file(dbBackupFile, { name: path.basename(dbBackupFile) });
        await archive.finalize();

        // Remove arquivo temporário do backup do banco
        fs.unlinkSync(dbBackupFile);

        // Mantém apenas os últimos 7 backups incrementais
        const files = fs.readdirSync(BACKUP_DIR)
            .filter(file => file.startsWith('backup_incremental_'))
            .sort()
            .reverse();

        if (files.length > 7) {
            files.slice(7).forEach(file => {
                fs.unlinkSync(path.join(BACKUP_DIR, file));
                logger.info(`Backup incremental antigo removido: ${file}`);
            });
        }

        await webhookClient.send({
            content: `✅ Backup incremental realizado com sucesso!\nArquivo: ${backupFileName}\nTamanho: ${(fs.statSync(backupFilePath).size / 1024 / 1024).toFixed(2)}MB`
        });

        logger.info('Backup incremental finalizado com sucesso');
    } catch (error) {
        logger.error('Erro no backup incremental:', error);
        await webhookClient.send({
            content: `❌ Erro no backup incremental: ${error.message}`
        });
    }
}

// Agenda os backups
schedule.scheduleJob('0 0 * * 0', fullBackup);  // Backup completo todo domingo à meia-noite
schedule.scheduleJob('0 0 * * 1-6', incrementalBackup);  // Backup incremental de segunda a sábado à meia-noite

logger.info('Sistema de backup iniciado');

// Tratamento de erros não capturados
process.on('uncaughtException', async (error) => {
    logger.error('Erro não capturado:', error);
    await webhookClient.send({
        content: `❌ Erro crítico no sistema de backup: ${error.message}`
    });
});

process.on('unhandledRejection', async (error) => {
    logger.error('Promise rejeitada não tratada:', error);
    await webhookClient.send({
        content: `❌ Erro assíncrono no sistema de backup: ${error.message}`
    });
}); 
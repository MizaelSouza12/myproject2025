require('dotenv').config();
const { execSync } = require('child_process');
const mysql = require('mysql2/promise');
const fs = require('fs-extra');
const path = require('path');
const winston = require('winston');
const { WebhookClient } = require('discord.js');

// Configuração do logger
const logger = winston.createLogger({
    level: 'info',
    format: winston.format.combine(
        winston.format.timestamp(),
        winston.format.json()
    ),
    transports: [
        new winston.transports.File({ filename: 'logs/deploy.log' }),
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

// Função para fazer backup do banco antes do deploy
async function backupBeforeDeploy() {
    try {
        logger.info('Iniciando backup pré-deploy...');
        const backupFile = path.join('backup', `pre_deploy_${Date.now()}.sql`);
        
        const dumpCommand = `mysqldump -h ${dbConfig.host} -P ${dbConfig.port} -u ${dbConfig.user} -p${dbConfig.password} ${dbConfig.database} > "${backupFile}"`;
        execSync(dumpCommand);
        
        logger.info(`Backup pré-deploy concluído: ${backupFile}`);
        return backupFile;
    } catch (error) {
        logger.error('Erro ao fazer backup pré-deploy:', error);
        throw error;
    }
}

// Função para executar migrações do banco de dados
async function runMigrations() {
    try {
        logger.info('Executando migrações do banco de dados...');
        
        const connection = await mysql.createConnection(dbConfig);
        
        // Verifica se a tabela de migrações existe
        await connection.execute(`
            CREATE TABLE IF NOT EXISTS migrations (
                id INT AUTO_INCREMENT PRIMARY KEY,
                name VARCHAR(255) NOT NULL,
                executed_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        `);

        // Lista todos os arquivos de migração
        const migrationsDir = path.join(process.cwd(), 'migrations');
        const migrationFiles = fs.readdirSync(migrationsDir)
            .filter(file => file.endsWith('.sql'))
            .sort();

        // Obtém migrações já executadas
        const [executedMigrations] = await connection.execute('SELECT name FROM migrations');
        const executedNames = executedMigrations.map(m => m.name);

        // Executa novas migrações
        for (const file of migrationFiles) {
            if (!executedNames.includes(file)) {
                const migration = fs.readFileSync(path.join(migrationsDir, file), 'utf8');
                await connection.execute(migration);
                await connection.execute('INSERT INTO migrations (name) VALUES (?)', [file]);
                logger.info(`Migração executada: ${file}`);
            }
        }

        await connection.end();
        logger.info('Migrações concluídas com sucesso');
    } catch (error) {
        logger.error('Erro ao executar migrações:', error);
        throw error;
    }
}

// Função para executar build do projeto
async function buildProject() {
    try {
        logger.info('Iniciando build do projeto...');

        // Limpa diretório de build
        fs.removeSync('build');
        fs.mkdirSync('build');

        // Executa CMake
        execSync('cmake -B build -S .', { stdio: 'inherit' });
        execSync('cmake --build build --config Release', { stdio: 'inherit' });

        logger.info('Build concluído com sucesso');
    } catch (error) {
        logger.error('Erro no build do projeto:', error);
        throw error;
    }
}

// Função para executar testes
async function runTests() {
    try {
        logger.info('Executando testes...');
        execSync('npm test', { stdio: 'inherit' });
        logger.info('Testes concluídos com sucesso');
        return true;
    } catch (error) {
        logger.error('Erro nos testes:', error);
        return false;
    }
}

// Função para iniciar o servidor
async function startServer() {
    try {
        logger.info('Iniciando servidor...');
        
        // Verifica se o servidor já está rodando
        try {
            execSync('netstat -ano | findstr :8080');
            logger.info('Servidor já está rodando. Reiniciando...');
            execSync('taskkill /F /IM wydbr2_server.exe');
        } catch {
            logger.info('Nenhuma instância do servidor encontrada');
        }

        // Inicia o servidor
        execSync('start build/Release/wydbr2_server.exe', { stdio: 'inherit' });
        
        logger.info('Servidor iniciado com sucesso');
    } catch (error) {
        logger.error('Erro ao iniciar servidor:', error);
        throw error;
    }
}

// Função principal de deploy
async function deploy() {
    try {
        await webhookClient.send({
            content: '🚀 Iniciando processo de deploy...'
        });

        // Backup pré-deploy
        const backupFile = await backupBeforeDeploy();

        // Executa testes
        const testsOk = await runTests();
        if (!testsOk) {
            throw new Error('Testes falharam');
        }

        // Build do projeto
        await buildProject();

        // Executa migrações
        await runMigrations();

        // Inicia servidor
        await startServer();

        await webhookClient.send({
            content: '✅ Deploy concluído com sucesso!'
        });

        logger.info('Deploy concluído com sucesso');
    } catch (error) {
        logger.error('Erro no processo de deploy:', error);
        
        await webhookClient.send({
            content: `❌ Erro no deploy: ${error.message}\nIniciando rollback...`
        });

        // Tenta restaurar backup em caso de erro
        try {
            const connection = await mysql.createConnection(dbConfig);
            const backupContent = fs.readFileSync(backupFile, 'utf8');
            await connection.execute(backupContent);
            await connection.end();
            
            await webhookClient.send({
                content: '✅ Rollback concluído com sucesso'
            });
        } catch (rollbackError) {
            logger.error('Erro no rollback:', rollbackError);
            await webhookClient.send({
                content: `❌ Erro no rollback: ${rollbackError.message}`
            });
        }

        process.exit(1);
    }
}

// Executa deploy
deploy();

// Tratamento de erros não capturados
process.on('uncaughtException', async (error) => {
    logger.error('Erro não capturado:', error);
    await webhookClient.send({
        content: `❌ Erro crítico no deploy: ${error.message}`
    });
});

process.on('unhandledRejection', async (error) => {
    logger.error('Promise rejeitada não tratada:', error);
    await webhookClient.send({
        content: `❌ Erro assíncrono no deploy: ${error.message}`
    });
}); 
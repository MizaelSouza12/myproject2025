const { execSync } = require('child_process');
const fs = require('fs-extra');
const path = require('path');
const chalk = require('chalk');
const winston = require('winston');

// Configuração do logger
const logger = winston.createLogger({
    level: 'info',
    format: winston.format.combine(
        winston.format.timestamp(),
        winston.format.json()
    ),
    transports: [
        new winston.transports.File({ filename: 'logs/installation.log' }),
        new winston.transports.Console({
            format: winston.format.combine(
                winston.format.colorize(),
                winston.format.simple()
            )
        })
    ]
});

// Função principal de instalação
async function installDependencies() {
    try {
        logger.info('Iniciando instalação das dependências...');

        // Verifica e cria diretórios necessários
        const dirs = ['logs', 'temp', 'backup', 'data'];
        dirs.forEach(dir => {
            if (!fs.existsSync(dir)) {
                fs.mkdirSync(dir, { recursive: true });
                logger.info(`Diretório ${dir} criado com sucesso`);
            }
        });

        // Instala dependências do Node.js
        logger.info('Instalando dependências do Node.js...');
        execSync('npm install', { stdio: 'inherit' });

        // Verifica e instala MySQL
        try {
            execSync('mysql --version', { stdio: 'ignore' });
            logger.info('MySQL já está instalado');
        } catch {
            logger.info('Instalando MySQL...');
            // No Windows, recomendamos baixar e instalar manualmente
            logger.warn('Por favor, baixe e instale o MySQL manualmente de: https://dev.mysql.com/downloads/installer/');
        }

        // Verifica e instala CMake
        try {
            execSync('cmake --version', { stdio: 'ignore' });
            logger.info('CMake já está instalado');
        } catch {
            logger.info('Instalando CMake...');
            logger.warn('Por favor, baixe e instale o CMake manualmente de: https://cmake.org/download/');
        }

        // Verifica e instala Git
        try {
            execSync('git --version', { stdio: 'ignore' });
            logger.info('Git já está instalado');
        } catch {
            logger.info('Instalando Git...');
            logger.warn('Por favor, baixe e instale o Git manualmente de: https://git-scm.com/downloads');
        }

        // Verifica Visual Studio Build Tools
        try {
            execSync('cl', { stdio: 'ignore' });
            logger.info('Visual Studio Build Tools já está instalado');
        } catch {
            logger.warn('Por favor, instale o Visual Studio Build Tools de: https://visualstudio.microsoft.com/visual-cpp-build-tools/');
        }

        // Configuração do ambiente
        const envContent = `
DATABASE_HOST=localhost
DATABASE_PORT=3306
DATABASE_USER=wydbr2_user
DATABASE_PASS=your_secure_password
DATABASE_NAME=wydbr2
SERVER_PORT=8080
DISCORD_WEBHOOK_URL=your_webhook_url
EMAIL_SMTP_HOST=smtp.example.com
EMAIL_SMTP_PORT=587
EMAIL_USER=your_email
EMAIL_PASS=your_email_password
`;

        fs.writeFileSync('.env', envContent);
        logger.info('Arquivo .env criado com sucesso');

        logger.info(chalk.green('✔ Instalação concluída com sucesso!'));
        logger.info(chalk.yellow('⚠ Por favor, configure as variáveis de ambiente no arquivo .env'));

    } catch (error) {
        logger.error('Erro durante a instalação:', error);
        process.exit(1);
    }
}

// Executa a instalação
installDependencies(); 
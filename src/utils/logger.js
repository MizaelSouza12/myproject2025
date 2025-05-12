const winston = require('winston');
const path = require('path');

// Configuração do formato de log
const logFormat = winston.format.combine(
    winston.format.timestamp(),
    winston.format.json()
);

// Criar diretório de logs se não existir
const logDir = path.join(process.cwd(), 'logs');
require('fs').mkdirSync(logDir, { recursive: true });

// Criar logger
const logger = winston.createLogger({
    level: process.env.LOG_LEVEL || 'info',
    format: logFormat,
    transports: [
        // Arquivo de log para todos os níveis
        new winston.transports.File({
            filename: path.join(logDir, 'wydbr.log'),
            maxsize: 5242880, // 5MB
            maxFiles: 5,
            tailable: true
        }),
        // Arquivo separado para erros
        new winston.transports.File({
            filename: path.join(logDir, 'error.log'),
            level: 'error',
            maxsize: 5242880, // 5MB
            maxFiles: 5,
            tailable: true
        }),
        // Console para desenvolvimento
        new winston.transports.Console({
            format: winston.format.combine(
                winston.format.colorize(),
                winston.format.simple()
            )
        })
    ]
});

// Adicionar handler para exceções não tratadas
logger.exceptions.handle(
    new winston.transports.File({
        filename: path.join(logDir, 'exceptions.log'),
        maxsize: 5242880, // 5MB
        maxFiles: 5,
        tailable: true
    })
);

// Não encerrar em exceções não tratadas
logger.exitOnError = false;

module.exports = logger; 
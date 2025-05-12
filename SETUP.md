# Configuração do Ambiente WYDBR 2.0

## Pré-requisitos
- Node.js 18+ instalado
- MySQL 8.0+ instalado
- Git instalado
- CMake instalado (para build do core)

## Passos para Configuração

1. Clone o repositório (se ainda não tiver feito):
```bash
git clone https://seu-repositorio/wydbr2.0.git
cd wydbr2.0
```

2. Instale as dependências do Node.js:
```bash
npm install
```

3. Crie um arquivo `.env` na raiz do projeto com as seguintes variáveis:
```env
# Configurações do Banco de Dados
DB_HOST=localhost
DB_USER=root
DB_PASSWORD=sua_senha
DB_NAME=wydbr2

# Configurações do Discord (para notificações)
DISCORD_TOKEN=seu_token_aqui
DISCORD_CHANNEL_ID=seu_channel_id_aqui

# Configurações de Email
SMTP_HOST=smtp.gmail.com
SMTP_PORT=587
SMTP_USER=seu_email@gmail.com
SMTP_PASS=sua_senha_app

# Configurações de Backup
BACKUP_DIR=./backup
BACKUP_RETENTION_DAYS=7

# Configurações de Monitoramento
MONITOR_INTERVAL=5000
CPU_THRESHOLD=80
MEMORY_THRESHOLD=90
DISK_THRESHOLD=85

# Configurações de Log
LOG_LEVEL=info
LOG_DIR=./logs
```

4. Crie os diretórios necessários:
```bash
mkdir -p backup logs data config
```

5. Execute o script de instalação de dependências:
```bash
npm run install-deps
```

6. Execute o build do core do sistema:
- Windows:
```bash
./build_windows.bat
```
- Linux:
```bash
./build_linux.sh
```

## Scripts Disponíveis

- `npm run install-deps`: Instala todas as dependências do sistema
- `npm run deploy`: Executa o deploy do sistema
- `npm run monitor`: Inicia o monitoramento do servidor
- `npm run backup`: Executa o backup do servidor

## Estrutura de Diretórios

- `/scripts`: Scripts de automação
- `/src`: Código fonte do sistema
- `/config`: Arquivos de configuração
- `/data`: Dados do sistema
- `/backup`: Diretório de backups
- `/logs`: Logs do sistema
- `/build`: Arquivos compilados

## Observações Importantes

1. Certifique-se de ter todas as permissões necessárias nos diretórios
2. Configure corretamente as credenciais no arquivo .env
3. O MySQL deve estar rodando antes de iniciar o sistema
4. Para notificações Discord, configure um webhook válido
5. Para emails, use uma senha de aplicativo do Gmail

## Troubleshooting

Se encontrar problemas:

1. Verifique os logs em ./logs
2. Certifique-se que todas as portas necessárias estão liberadas
3. Verifique as permissões dos diretórios
4. Confirme que todas as variáveis de ambiente estão configuradas 
# WYDBR2.0 - Sistema de Deploy e Monitoramento

Este repositório contém scripts para automatizar o deploy, monitoramento e manutenção do servidor WYDBR2.0.

## Requisitos

- Windows 10 ou superior
- PowerShell 7.0 ou superior
- MySQL 8.0 ou superior
- CMake 3.20 ou superior
- Compilador C++ (MSVC ou MinGW)
- Git

## Estrutura de Diretórios

```
WYDBR2.0/
├── scripts/
│   ├── deploy.ps1        # Script principal de deploy
│   ├── backup.ps1        # Script de backup automático
│   └── health_check.ps1  # Script de verificação de saúde
├── logs/                 # Diretório de logs
├── config/              # Arquivos de configuração
├── database/           # Scripts e schemas do banco de dados
└── build/              # Diretório de build
```

## Configuração

1. Edite o arquivo `scripts/deploy.ps1` e ajuste as configurações:

```powershell
$config = @{
    ProjectRoot = "F:\Users\Mizael\WYDBR2.0"
    BuildType = "Release"
    DatabaseName = "wydbr2"
    DatabaseUser = "wydbr2_user"
    DatabasePassword = "your_secure_password"
    ServerPort = 8080
    MaxConnections = 1000
    LogLevel = "INFO"
    BackupPath = "F:\Backups\WYDBR2.0"
    MonitoringEnabled = $true
    AutoRestart = $true
}
```

2. Configure as credenciais de email em `scripts/health_check.ps1`:

```powershell
$smtpServer = "smtp.wydbr2.com"
$smtpPort = 587
$smtpUser = "alerts@wydbr2.com"
$smtpPass = "your_smtp_password"
```

## Uso

### Deploy Completo

```powershell
.\scripts\deploy.ps1
```

O script irá:
1. Verificar dependências
2. Fazer backup do banco de dados
3. Build do projeto
4. Executar testes
5. Migrar banco de dados
6. Iniciar servidor
7. Iniciar monitoramento

### Backup Manual

```powershell
.\scripts\backup.ps1
```

O script irá:
1. Verificar espaço em disco
2. Fazer backup do banco de dados
3. Comprimir backup
4. Limpar backups antigos

### Verificação de Saúde

```powershell
.\scripts\health_check.ps1
```

O script irá verificar:
1. Processo do servidor
2. Uso de recursos (CPU, RAM, Disco)
3. Conexões com banco de dados
4. Portas
5. Logs
6. Enviar alertas por email se necessário

## Tarefas Agendadas

O script `deploy.ps1` pode configurar tarefas agendadas:

1. Backup diário às 3:00 AM
2. Verificação de saúde a cada 30 minutos

Para configurar:

```powershell
.\scripts\deploy.ps1
# Escolha opção 6 no menu
```

## Monitoramento

O sistema monitora:

- Status do servidor
- Uso de CPU
- Uso de memória
- Uso de disco
- Conexões com banco de dados
- Erros nos logs

Alertas são enviados por email quando:
- Servidor para de responder
- Uso de recursos excede limites
- Erros críticos são detectados
- Problemas com banco de dados

## Logs

Logs são mantidos em `logs/`:

- `deploy.log`: Logs de deploy
- `backup.log`: Logs de backup
- `health.log`: Logs de verificação de saúde

Logs antigos são automaticamente limpos após 7 dias.

## Segurança

- Senhas e credenciais devem ser alteradas
- Backups são comprimidos e criptografados
- Logs são rotacionados
- Alertas são enviados por email seguro

## Suporte

Para suporte:
1. Verifique os logs em `logs/`
2. Consulte a documentação
3. Entre em contato com a equipe de suporte

## Contribuição

1. Fork o repositório
2. Crie uma branch para sua feature
3. Commit suas mudanças
4. Push para a branch
5. Crie um Pull Request

## Licença

Este projeto é proprietário e confidencial.

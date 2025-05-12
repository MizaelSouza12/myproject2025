# WYDBR - Estrutura de Execução do Servidor

## Arquitetura Geral

O sistema do servidor WYDBR é composto por vários processos independentes que trabalham juntos para fornecer a experiência completa do jogo. Cada processo tem sua própria responsabilidade e se comunica com os outros através de uma camada de rede interna.

```
                                  +----------------+
                                  |   Jogadores    |
                                  +--------+-------+
                                           |
                                           v
+------------+    +------------+    +------+-------+    +------------+
|   DBSrv    |<-->|   NRSrv    |<-->|    TMSrv     |<-->|   LogSrv   |
+------------+    +------------+    +-------+------+    +------------+
       ^                 ^                 |
       |                 |                 v
       |                 |          +------+-------+
       +-----------------+--------->|    BISrv     |
                                    +------+-------+
                                           |
                                           v
                                    +------+-------+
                                    |    TSSSrv    |
                                    +--------------+
```

## Componentes Principais

### TMSrv (Servidor de Jogo)
- **Função**: Gerencia o mundo do jogo, conexões de jogadores, eventos e gameplay
- **Dependências**: DBSrv, NRSrv, BISrv, LogSrv
- **Arquivos Principais**: 
  - Executável: `bin/TMSrv`
  - Configuração: `config/TMSrv.conf`
  - Logs: `logs/tmsrv/`

### DBSrv (Servidor de Banco de Dados)
- **Função**: Gerencia o acesso ao banco de dados, transações e armazenamento persistente
- **Dependências**: Banco de dados PostgreSQL
- **Arquivos Principais**:
  - Executável: `bin/DBSrv`
  - Configuração: `config/DBSrv.conf`
  - Logs: `logs/dbsrv/`

### NRSrv (Servidor de Nomes e Registros)
- **Função**: Gerencia o registro de contas, autenticação e sessões
- **Dependências**: DBSrv
- **Arquivos Principais**:
  - Executável: `bin/NRSrv`
  - Configuração: `config/NRSrv.conf`
  - Logs: `logs/nrsrv/`

### BISrv (Servidor de Interface de Banco)
- **Função**: Fornece interface para operações bancárias e armazenamento de itens
- **Dependências**: DBSrv, TMSrv
- **Arquivos Principais**:
  - Executável: `bin/BISrv`
  - Configuração: `config/BISrv.conf`
  - Logs: `logs/bisrv/`

### TSSSrv (Servidor de Armazenamento Temporário)
- **Função**: Gerencia armazenamento temporário, trocas e leilões
- **Dependências**: BISrv
- **Arquivos Principais**:
  - Executável: `bin/TSSSrv`
  - Configuração: `config/TSSSrv.conf`
  - Logs: `logs/tsssrv/`

### LogSrv (Servidor de Logs)
- **Função**: Gerencia o registro centralizado de logs e eventos
- **Dependências**: Nenhuma
- **Arquivos Principais**:
  - Executável: `bin/LogSrv`
  - Configuração: `config/LogSrv.conf`
  - Logs: `logs/logsrv/`

## Fluxo de Execução

### Inicialização
1. O LogSrv é iniciado primeiro para configurar o sistema de logging
2. O DBSrv é iniciado para fornecer acesso ao banco de dados
3. O NRSrv é iniciado para gerenciar autenticação
4. O BISrv é iniciado para gerenciar o banco
5. O TSSSrv é iniciado para gerenciar armazenamento temporário
6. O TMSrv é iniciado por último e começa a aceitar conexões de jogadores

Este processo é gerenciado pelo script `scripts/startup.sh`.

### Comunicação Entre Processos
Os processos se comunicam através de uma camada de rede interna usando um protocolo proprietário. Cada processo possui:

- Um servidor que escuta em uma porta específica
- Clientes que se conectam a outros processos
- Um sistema de heartbeat para verificar a saúde dos outros processos
- Um sistema de reconexão em caso de falhas

### Fluxo de Dados Típico
1. Jogador conecta ao TMSrv
2. TMSrv autentica o jogador através do NRSrv
3. TMSrv carrega os dados do jogador do DBSrv
4. Jogador joga, gerando eventos e atualizações
5. TMSrv processa a lógica de jogo e envia atualizações ao jogador
6. Periodicamente, os dados são salvos no DBSrv
7. Logs são enviados ao LogSrv

## Gerenciamento de Recursos

### Memória
Cada processo gerencia sua própria memória com pools de alocação para otimizar desempenho.

### CPU
O sistema é multi-threaded, com diferentes threads para:
- Processamento de rede
- Lógica de jogo
- Operações de banco de dados
- Tarefas de manutenção

### Disco
Os dados são salvos em:
- Banco de dados (persistente)
- Arquivos de log (rotacionados)
- Arquivos de cache (temporários)

### Rede
- Comunicação interna: Protocolo proprietário TCP
- Comunicação com jogadores: Protocolo proprietário TCP/UDP
- Compressão e criptografia para economia de banda

## Monitoramento e Administração

### Ferramentas de Monitoramento
- Visualizador de logs em tempo real
- Painel de estatísticas do servidor
- Sistema de alerta para problemas

### Ferramentas de Administração
- Console de administração
- Comandos remotos
- API de administração

## Recuperação de Falhas

### Backups
- Backup completo: Diário
- Backup incremental: A cada hora
- Snapshot de estado do mundo: A cada 5 minutos

### Processo de Recuperação
1. Se um processo falha, o script de monitoramento tenta reiniciá-lo
2. Se a reinicialização falha, alertas são enviados
3. Em caso de corrupção de dados, os dados mais recentes de backup são restaurados

## Considerações de Escala

O sistema pode ser escalonado de duas formas:

### Escala Vertical
- Aumento de memória e CPU para cada processo
- Ajustes de configuração para usar mais recursos

### Escala Horizontal
- Múltiplos servidores de jogo (TMSrv) por mundo
- Sharding de banco de dados
- Sistemas distribuídos para logs e armazenamento temporário
# DBSrv - Servidor de Banco de Dados do WYDBR 2.0

## Descrição

O DBSrv (Database Server) é o componente responsável pelo gerenciamento de dados persistentes do jogo WYDBR 2.0. É ele quem se comunica diretamente com o banco de dados relacional (MySQL/MariaDB) e fornece uma camada de abstração e cache para os outros servidores do sistema.

## Características Principais

- Sistema de cache multinível para redução de carga no banco de dados
- Sharding de dados para distribuição de carga e escalabilidade horizontal
- Suporte a operações assíncronas para alta performance
- Recuperação automática em caso de falha de conexão com o banco de dados
- Sistema de logging detalhado para diagnóstico e auditoria
- Backup automático com rotação de arquivos
- Compressão de dados em transmissões de rede
- Validação e sanitização de dados para segurança
- Suporte a múltiplas conexões de servidores de jogo (TMSrv)

## Arquitetura

O DBSrv segue uma arquitetura modular dividida em:

1. **Core Engine**
   - Gerenciamento de threads e recursos
   - Processamento de pacotes
   - Sistema de logging

2. **Cache Manager**
   - Cache LRU (Least Recently Used) em memória
   - Cache persistente em disco para dados frequentes
   - Política de expiração e invalidação

3. **Database Manager**
   - Pool de conexões com o banco de dados
   - Transações atômicas
   - Queries parametrizadas para evitar injeção SQL

4. **Network Manager**
   - Gerenciamento de conexões com outros servidores
   - Protocolo de comunicação seguro
   - Autenticação e autorização

5. **Metrics Collector**
   - Monitoramento de performance
   - Coleta de estatísticas de uso
   - Sistema de alertas

## Requisitos de Sistema

- Sistema Operacional: Windows Server 2016+ ou Linux (Ubuntu 18.04+, CentOS 7+)
- CPU: 4+ cores, 3.0+ GHz
- RAM: 16+ GB (recomendado 32+ GB para servidores de produção)
- Armazenamento: SSD com 100+ GB (dependendo do tamanho do banco de dados)
- Banco de Dados: MySQL 5.7+ ou MariaDB 10.3+
- Dependências: Boost 1.70+, OpenSSL 1.1.1+, ZLib 1.2.11+

## Compilação

### Windows

1. Instale o Visual Studio 2019 ou superior com suporte a C++
2. Instale o CMake 3.15 ou superior
3. Clone o repositório do WYDBR
4. Configure as dependências utilizando vcpkg:
   ```
   vcpkg install boost:x64-windows openssl:x64-windows zlib:x64-windows
   ```
5. Configure o projeto:
   ```
   mkdir build
   cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=[caminho para vcpkg]/scripts/buildsystems/vcpkg.cmake
   ```
6. Compile o projeto:
   ```
   cmake --build . --config Release
   ```
7. O executável será gerado em `build/bin/Release/DBSrv.exe`

### Linux

1. Instale as dependências:
   ```
   sudo apt-get update
   sudo apt-get install build-essential cmake libboost-all-dev libssl-dev zlib1g-dev libmysqlclient-dev
   ```
2. Clone o repositório do WYDBR
3. Configure o projeto:
   ```
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```
4. Compile o projeto:
   ```
   make -j$(nproc)
   ```
5. O executável será gerado em `build/bin/DBSrv`

## Configuração

O servidor utiliza o arquivo `config/DBSrv.conf` para sua configuração. Os principais parâmetros são:

- **Database.Host**: Endereço do servidor de banco de dados
- **Database.Port**: Porta do servidor de banco de dados (padrão: 3306)
- **Database.User**: Usuário do banco de dados
- **Database.Password**: Senha do banco de dados
- **Database.Name**: Nome do banco de dados
- **Database.MaxConnections**: Número máximo de conexões ao banco de dados (padrão: 10)
- **Server.IP**: Endereço IP para bindar o servidor (padrão: 0.0.0.0)
- **Server.Port**: Porta para bindar o servidor (padrão: 7514)
- **Server.ThreadCount**: Número de threads de trabalho (padrão: número de cores)
- **Cache.Enabled**: Habilita o sistema de cache (padrão: true)
- **Cache.MaxMemoryMB**: Memória máxima para cache (padrão: 4096)
- **Cache.PersistentPath**: Caminho para cache persistente
- **Log.Level**: Nível de detalhamento dos logs (DEBUG, INFO, WARNING, ERROR)
- **Log.Path**: Caminho para os arquivos de log
- **Backup.Enabled**: Habilita backups automáticos (padrão: true)
- **Backup.Interval**: Intervalo em minutos entre backups (padrão: 360)
- **Backup.Path**: Caminho para backups
- **Backup.MaxFiles**: Número máximo de arquivos de backup (padrão: 24)
- **Sharding.Enabled**: Habilita sharding de dados (padrão: false)
- **Sharding.ShardCount**: Número de shards (padrão: 1)

## Inicialização

Para iniciar o servidor:

```
./DBSrv --config=/caminho/para/DBSrv.conf
```

Parâmetros adicionais de linha de comando:

- `--help`: Exibe ajuda sobre parâmetros
- `--version`: Exibe a versão do servidor
- `--log-level=[level]`: Define o nível de log (sobrescreve a configuração)
- `--console`: Exibe logs no console além de gravá-los em arquivo
- `--maintenance`: Inicia em modo de manutenção (apenas administradores podem conectar)

## Monitoramento

O DBSrv expõe métricas de monitoramento acessíveis via:

1. Interface de administração web: http://[server-ip]:7514/admin (protegida por senha)
2. API HTTP: http://[server-ip]:7514/metrics (formato compatível com Prometheus)
3. Log de métricas: `/logs/dbsrv_metrics_[date].log`

## Troubleshooting

### Problemas comuns e soluções:

1. **O servidor não inicia**
   - Verifique os logs em `/logs/dbsrv_error.log`
   - Confirme se as configurações de banco de dados estão corretas
   - Verifique se o banco de dados está acessível

2. **Alto uso de CPU**
   - Aumente o número de threads de trabalho
   - Verifique queries pesadas no log de SQL
   - Considere adicionar índices às tabelas frequentemente acessadas

3. **Alto uso de memória**
   - Reduza o tamanho máximo de cache
   - Verifique vazamentos de memória em logs detalhados
   - Aumente o limite de RAM se necessário

4. **Latência alta**
   - Verifique a conexão de rede com o banco de dados
   - Monitore o tempo de resposta de queries
   - Considere adicionar réplicas de leitura ao banco de dados

5. **Desconexões frequentes de outros servidores**
   - Verifique logs de rede para timeouts
   - Aumente limites de keepalive
   - Verifique se há problemas de rede entre os servidores

## Suporte e Contato

Para suporte técnico, entre em contato com a equipe de desenvolvimento do WYDBR através do email: support@wydbr.com

## Licença

Este software é proprietário e confidencial. Uso não autorizado é estritamente proibido. © 2023 WYDBR Team. 
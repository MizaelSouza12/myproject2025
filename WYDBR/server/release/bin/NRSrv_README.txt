# NRSrv - Servidor de Roteamento de Rede do WYDBR 2.0

## Descrição

O NRSrv (Network Routing Server) é um componente crítico da infraestrutura do WYDBR 2.0, responsável pelo roteamento inteligente de pacotes entre os diversos servidores de jogo e os clientes. Ele implementa um sistema avançado de balanceamento de carga, detecção de falhas e recuperação automática para garantir alta disponibilidade e distribuição eficiente de tráfego.

## Características Principais

- Balanceamento de carga adaptativo com vários algoritmos configuráveis
- Roteamento inteligente baseado em localização geográfica dos clientes
- Failover automático em caso de queda de servidores
- Agrupamento de jogadores por afinidade (clãs, grupos, etc.)
- Sistema de prevenção contra ataques DDoS
- Compressão e otimização de tráfego em tempo real
- Criptografia de ponta a ponta para comunicações seguras
- Análise de tráfego para detecção de bots e hacks
- Métricas detalhadas e sistema de alerta

## Arquitetura

O NRSrv segue uma arquitetura modular com:

1. **Core Router**
   - Gerenciamento de conexões de alto desempenho
   - Pipeline de processamento de pacotes
   - Sistema de filas priorizadas

2. **Load Balancer**
   - Múltiplos algoritmos de balanceamento (round-robin, least connections, weighted)
   - Sistema de health check para servidores
   - Distribuição geográfica inteligente

3. **Connection Manager**
   - Manutenção de conexões persistentes
   - Gerenciamento de sessões
   - Reconexão transparente

4. **Security Layer**
   - Filtro de pacotes e validação
   - Sistema anti-DDoS
   - Detecção de anomalias
   - Criptografia de tráfego

5. **Analytics Engine**
   - Coleta de métricas em tempo real
   - Detecção de padrões de uso
   - Geração de relatórios

## Requisitos de Sistema

- Sistema Operacional: Windows Server 2016+ ou Linux (Ubuntu 18.04+, CentOS 7+)
- CPU: 8+ cores, 3.0+ GHz (recomendado servidor dedicado)
- RAM: 16+ GB (32+ GB recomendado para servidores de alta carga)
- Rede: Interface de rede dedicada 1 Gbps+ com baixa latência
- Armazenamento: SSD 50+ GB para logs e configurações
- Dependências: Boost 1.70+, OpenSSL 1.1.1+, ZLib 1.2.11+, libpcap/WinPcap

## Compilação

### Windows

1. Instale o Visual Studio 2019 ou superior com suporte a C++
2. Instale o CMake 3.15 ou superior
3. Clone o repositório do WYDBR
4. Configure as dependências utilizando vcpkg:
   ```
   vcpkg install boost:x64-windows openssl:x64-windows zlib:x64-windows winpcap:x64-windows
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
7. O executável será gerado em `build/bin/Release/NRSrv.exe`

### Linux

1. Instale as dependências:
   ```
   sudo apt-get update
   sudo apt-get install build-essential cmake libboost-all-dev libssl-dev zlib1g-dev libpcap-dev libnuma-dev
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
5. O executável será gerado em `build/bin/NRSrv`

## Configuração

O servidor utiliza o arquivo `config/NRSrv.conf` para sua configuração. Os principais parâmetros são:

- **Server.IP**: Endereço IP para bindar o servidor (padrão: 0.0.0.0)
- **Server.Port**: Porta para bindar o servidor (padrão: 7517)
- **Server.ThreadCount**: Número de threads de trabalho (padrão: número de cores)
- **Performance.IOThreads**: Número de threads dedicadas a I/O (padrão: número de cores)
- **Performance.EnableNUMA**: Otimização para sistemas NUMA (padrão: true em sistemas suportados)
- **Performance.TCPNoDelay**: Habilita TCP_NODELAY (Nagle off) (padrão: true)
- **Performance.ReusePort**: Habilita SO_REUSEPORT para melhor balanceamento (padrão: true)
- **Servers.GameServers**: Lista de servidores de jogo no formato [ip:porta:peso:região]
- **Servers.DatabaseServer**: Endereço do servidor de banco de dados [ip:porta]
- **Servers.BillingServer**: Endereço do servidor de billing [ip:porta]
- **LoadBalancing.Algorithm**: Algoritmo de balanceamento (RoundRobin, LeastConnections, WeightedRR, GeoIP)
- **LoadBalancing.StickySessions**: Mantém jogador no mesmo servidor quando possível (padrão: true)
- **LoadBalancing.GroupAffinity**: Mantém grupos/clãs no mesmo servidor (padrão: true)
- **Security.DDoSProtection**: Habilita proteção contra DDoS (padrão: true)
- **Security.RateLimiting**: Limite de pacotes por segundo por IP (padrão: 1000)
- **Security.EnableEncryption**: Habilita criptografia de tráfego (padrão: true)
- **Security.AllowedIPs**: Lista de IPs confiáveis que ignoram limitações
- **Log.Level**: Nível de detalhamento dos logs (DEBUG, INFO, WARNING, ERROR)
- **Log.Path**: Caminho para os arquivos de log
- **Traffic.Compression**: Habilita compressão de tráfego (padrão: true)
- **Traffic.CompressionThreshold**: Tamanho mínimo em bytes para compressão (padrão: 1024)
- **Monitoring.StatsInterval**: Intervalo em segundos para coleta de estatísticas (padrão: 60)
- **Monitoring.HealthCheckInterval**: Intervalo em segundos para verificação de saúde dos servidores (padrão: 5)

## Inicialização

Para iniciar o servidor:

```
./NRSrv --config=/caminho/para/NRSrv.conf
```

Parâmetros adicionais de linha de comando:

- `--help`: Exibe ajuda sobre parâmetros
- `--version`: Exibe a versão do servidor
- `--log-level=[level]`: Define o nível de log (sobrescreve a configuração)
- `--console`: Exibe logs no console além de gravá-los em arquivo
- `--network-dump`: Habilita o dump de pacotes para análise (apenas desenvolvimento)
- `--maintenance`: Inicia em modo de manutenção

## Monitoramento

O NRSrv expõe métricas de monitoramento acessíveis via:

1. Interface de administração web: http://[server-ip]:7517/admin (protegida por senha)
2. API HTTP: http://[server-ip]:7517/metrics (formato compatível com Prometheus)
3. SNMP: Disponível na porta 161 (requer configuração adicional)
4. Log de métricas: `/logs/nrsrv_metrics_[date].log`

## Comandos de Administração

O NRSrv possui uma interface de comando acessível via console ou administração remota:

- `server status`: Mostra status geral do servidor
- `server connections`: Lista todas as conexões ativas
- `server stats`: Mostra estatísticas detalhadas
- `route list`: Lista todas as rotas disponíveis
- `route add [src] [dst] [priority]`: Adiciona uma nova rota
- `route remove [id]`: Remove uma rota existente
- `ban ip [address] [duration]`: Bane um endereço IP temporariamente
- `unban ip [address]`: Remove o banimento de um IP
- `reload config`: Recarrega a configuração sem reiniciar o servidor

## Troubleshooting

### Problemas comuns e soluções:

1. **Alta latência nos pacotes**
   - Verifique a carga de CPU e se o número de threads é suficiente
   - Desative a compressão para pacotes pequenos ajustando CompressionThreshold
   - Verifique a configuração da interface de rede

2. **Uso elevado de CPU**
   - Aumente o número de threads de I/O
   - Verifique se o sistema operacional está configurado corretamente para rede de alto desempenho
   - Em Linux, considere usar `tuned` com perfil network-latency

3. **Desconexões frequentes**
   - Verifique os logs de timeouts
   - Ajuste os parâmetros de keepalive
   - Verifique se não há problemas de rede entre os servidores

4. **Falhas de balanceamento de carga**
   - Verifique se todos os servidores no pool estão respondendo corretamente
   - Ajuste o algoritmo de balanceamento conforme padrões de uso
   - Verifique se o StickySessions não está causando desbalanceamento

5. **Ataques DDoS**
   - Ajuste os parâmetros de RateLimiting
   - Configure um servidor proxy reverso como Nginx ou redirecione para um serviço anti-DDoS
   - Implemente regras de firewall adicionais

## Suporte e Contato

Para suporte técnico, entre em contato com a equipe de desenvolvimento do WYDBR através do email: support@wydbr.com

## Licença

Este software é proprietário e confidencial. Uso não autorizado é estritamente proibido. © 2023 WYDBR Team. 
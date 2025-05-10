# WYDBR 2.0 - Visão Geral da Arquitetura

## Introdução

Este documento apresenta uma visão detalhada da arquitetura do WYDBR 2.0, descrevendo seus componentes principais, padrões de design, fluxos de dados, e decisões arquiteturais.

## Componentes Principais

1. **WYDStudio**: Interface administrativa centralizada que incorpora os servidores TMSrv e DBSrv como componentes internos.
2. **TMSrv**: Servidor de jogo responsável por gerenciar o mundo, entidades, combate e interações.
3. **DBSrv**: Servidor de banco de dados responsável pela persistência de dados.
4. **Sistema Integrado**: Camada que gerencia a comunicação entre componentes e centraliza operações.
5. **Cliente WYD**: Aplicativo cliente utilizado pelos jogadores.
6. **Interface de Rede**: Componente responsável pela comunicação entre cliente e servidor.

## Decisões Arquiteturais

### Integração TMSrv/DBSrv no WYDStudio

Em vez de manter o TMSrv e DBSrv como processos separados, optamos por integrá-los diretamente no WYDStudio para melhorar eficiência e gestão.

### Sistema de Mensagens Centralizado

Implementamos um barramento de mensagens central (MessageBus) para comunicação desacoplada entre componentes.

### Sistema de Segurança Integrado

Integramos mecanismos de segurança em todos os componentes em vez de tratá-los como uma camada separada.

## Fluxos de Dados Principais

### Fluxo de Login

1. Cliente envia credenciais para o servidor
2. TMSrv verifica credenciais com o DBSrv
3. TMSrv gera token de sessão com salt específico
4. TMSrv responde ao cliente com token de sessão
5. Cliente usa token nas requisições subsequentes

## Próximos Passos e Evolução

- Escalabilidade horizontal
- Sistema de monitoramento avançado 
- Evolução gradual para arquitetura de micro-serviços

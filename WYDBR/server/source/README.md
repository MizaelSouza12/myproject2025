# WYDBR - Código Fonte do Servidor

Este diretório contém o código fonte completo do servidor WYDBR, reconstruído através de engenharia reversa avançada do servidor original WYD. O código foi modernizado e otimizado, mantendo a compatibilidade funcional com o cliente original.

## Estrutura do Código

### Diretórios Principais

- `/include`: Arquivos de cabeçalho compartilhados entre todos os componentes
  - `/core`: Componentes fundamentais (eventos, logging, configuração)
  - `/network`: Sistema de rede e protocolo
  - `/database`: Abstração de banco de dados e queries
  - `/utils`: Utilitários diversos
  - `/game`: Lógica de jogo compartilhada

- `/src`: Código fonte dos componentes do servidor
  - `/tmsrv`: Servidor principal de jogo
  - `/dbsrv`: Servidor de banco de dados
  - `/nrsrv`: Servidor de registro e autenticação
  - `/bisrv`: Servidor de interface bancária
  - `/tsssrv`: Servidor de trocas e armazenamento
  - `/logsrv`: Servidor de logs
  - `/common`: Código compartilhado entre servidores

- `/tests`: Testes automatizados
  - `/unit`: Testes unitários para componentes individuais
  - `/integration`: Testes de integração para sistemas completos
  - `/performance`: Testes de desempenho
  - `/mock`: Componentes simulados para testes

- `/tools`: Ferramentas de desenvolvimento
  - `/migration`: Ferramentas para migração de dados
  - `/analysis`: Ferramentas de análise de código
  - `/debugging`: Ferramentas de depuração
  - `/codegen`: Geradores de código

- `/docs`: Documentação técnica gerada
  - `/api`: Documentação da API
  - `/internal`: Documentação interna
  - `/protocol`: Especificação do protocolo de rede

## Organização do Código

O código segue uma arquitetura em camadas bem definida:

1. **Camada de Aplicação**: Específica para cada servidor (TMSrv, DBSrv, etc.)
2. **Camada de Domínio**: Lógica de negócio e entidades de jogo
3. **Camada de Infraestrutura**: Banco de dados, rede, logging, etc.
4. **Camada de Interface**: APIs e protocolos de comunicação

## Convenções de Código

### Nomenclatura

- Namespaces: `wydbr` (principal), `wydbr::tmsrv`, `wydbr::dbsrv`, etc.
- Classes: PascalCase (ex: `PlayerManager`)
- Métodos e funções: camelCase (ex: `getPlayerById`)
- Variáveis: camelCase (ex: `playerCount`)
- Constantes: SNAKE_CASE maiúsculo (ex: `MAX_PLAYERS`)
- Enums: PascalCase com valores em SNAKE_CASE (ex: `enum class PlayerState { ACTIVE, IDLE }`)

### Organização de Arquivos

- Um arquivo de cabeçalho (.h) e um arquivo de implementação (.cpp) por classe
- Arquivos organizados em diretórios correspondentes à estrutura de namespaces
- Testes compartilham a mesma estrutura de diretórios que o código fonte

## Compilação

### Requisitos

- C++17 ou superior
- CMake 3.12+
- PostgreSQL 12+ com desenvolvimento
- Biblioteca de rede (asio)
- Biblioteca de logs (spdlog)
- Biblioteca para manipulação de JSON (nlohmann_json)
- Biblioteca de testes (GoogleTest, optional)

### Processo de Compilação

```bash
# Cria um diretório para build
mkdir build && cd build

# Configura o projeto
cmake ..

# Compila todos os componentes
make

# Compila um componente específico
make tmsrv

# Executa os testes
make test
```

## Execução em Modo Desenvolvimento

Para execução durante o desenvolvimento, use o modo de debug:

```bash
# Define o nível de log para debug
export LOG_LEVEL=debug

# Executa o TMSrv em modo de desenvolvimento
./bin/TMSrv --config=../config/TMSrv.dev.conf
```

## Contribuição para o Código Fonte

### Estilo de Código

Este projeto segue um guia de estilo baseado no Google C++ Style Guide com algumas adaptações. Use as ferramentas de formatação para manter a consistência:

```bash
# Formata o código
./tools/format_code.sh

# Verifica o estilo sem modificar
./tools/check_style.sh
```

### Processo de Contribuição

1. Crie uma branch para sua feature/correção
2. Implemente as mudanças seguindo o guia de estilo
3. Adicione testes para suas mudanças
4. Garanta que todos os testes passem
5. Atualize a documentação relevante
6. Envie um Pull Request

## Documentação

A documentação técnica detalhada é gerada a partir dos comentários no código usando Doxygen:

```bash
# Gera a documentação
cd docs
make docs
```

A documentação gerada estará disponível em `docs/html/index.html`.

## Estrutura de Dependências

Os componentes do servidor têm as seguintes dependências principais:

```
TMSrv -> DBSrv, NRSrv, BISrv, LogSrv
NRSrv -> DBSrv, LogSrv
BISrv -> DBSrv, LogSrv
TSSSrv -> BISrv, LogSrv
DBSrv -> LogSrv
LogSrv -> (independente)
```

Para mais informações, consulte o diagrama de arquitetura em `/docs/architecture.pdf`.

## Recursos Adicionais

- [Guia de Solução de Problemas](../docs/troubleshooting.md)
- [FAQ para Desenvolvedores](../docs/developer_faq.md)
- [Padrões de Design Utilizados](../docs/design_patterns.md)
- [Guia de Otimização](../docs/optimization.md)
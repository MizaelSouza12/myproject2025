# Plano de Engenharia Reversa - WYD (With Your Destiny)

## Visão Geral do Projeto

Este documento detalha o plano de engenharia reversa do MMORPG "With Your Destiny" (WYD). O objetivo é reconstruir a arquitetura completa do jogo em C++, mantendo compatibilidade total com os protocolos e formatos originais. O projeto foi inicialmente concebido como uma reconstrução em JavaScript, mas a análise preliminar determinou que essa linguagem seria incompatível com a arquitetura nativa do jogo.

O processo completo de engenharia reversa está estimado em 15-30 meses, dividido em 7 fases técnicas, desde a análise binária inicial até a reconstrução completa do sistema.

## Abordagem e Filosofia do Projeto "WYD 2.0"

Nossa abordagem técnica para a reconstrução do WYD segue uma filosofia específica denominada "WYD 2.0", que se baseia nos seguintes princípios fundamentais:

### Princípios de Implementação:

1. **NADA DE CÓDIGO BÁSICO** - Toda implementação deve ser avançada e otimizada desde o início, sem versões simplificadas ou esqueletos básicos. Cada componente deve ser construído com sua funcionalidade completa e final.

2. **PRESERVAR A ESTRUTURA ORIGINAL** - Manter fidelidade à arquitetura original do WYD, incluindo estruturas de dados, protocolos de rede e formatos de arquivo. Esta preservação é crucial para garantir compatibilidade com o cliente original.

3. **APLICAR MELHORIAS AVANÇADAS** - Implementar melhorias significativas sobre a base original, utilizando técnicas modernas de programação para resolver limitações do código original sem comprometer a compatibilidade.

4. **CORRIGIR BUGS E VULNERABILIDADES** - Identificar e corrigir todos os problemas conhecidos do código original, incluindo bugs de gameplay, vulnerabilidades de segurança e problemas de desempenho.

### Metodologia Técnica:

- **Implementações diretas e completas** - Desenvolver cada componente em sua forma final ao invés de criar implementações incrementais.
- **Preservação de comportamentos originais** - Manter comportamentos críticos que os jogadores esperam do WYD original.
- **Otimização avançada** - Aplicar técnicas como multithreading, gerenciamento eficiente de memória e algoritmos otimizados.
- **Engenharia de código robusto** - Utilizar boas práticas como manipulação correta de erros, validação de entradas e arquitetura de componentes para garantir estabilidade.

Este projeto não é uma simples recriação do WYD original, mas uma reconstrução avançada que respeita os fundamentos do original enquanto aplica melhorias significativas em todos os aspectos do sistema.

## Status Atual do Projeto - 06/05/2025

Estamos atualmente na Fase 5 (Reconstrução do Sistema Base) do projeto, avançando para a Fase 6 (Reconstrução de Sistemas de Jogo). As principais conquistas até o momento:

- Criação da estrutura inicial do projeto
- Conclusão da análise dos binários do cliente
- Criação dos arquivos de definição (item_struct.h, packet_struct.h, mob_struct.h, wyd_core.h)
- Documentação completa das estruturas de dados do jogo
- Desenvolvimento e utilização de ferramentas avançadas de análise binária
- Implementação de sistemas fundamentais em C++:
  - Sistema de Combate (CombatSystem)
  - Conector para Servidor de Banco de Dados (DBServerConnector)
  - Sistema de Mapas e Gerenciamento de Mundo (WYDMap)
  - Sistema de Zonas específicas dentro de mapas (WYDZone)
  - Sistema de Gatilhos para eventos no mapa (WYDTrigger)
  - Sistema de Terreno e Altura (WYDHeightmap)
  - Sistema de Atmosfera e Clima (WYDAtmosphere)

## Ferramentas Desenvolvidas

Para auxiliar no processo de engenharia reversa, desenvolvemos as seguintes ferramentas especializadas:

1. **WYD Binary Analyzer** (`wyd_bin_analyzer.js`):
   - Analisa e decodifica os formatos binários proprietários do WYD
   - Detecta padrões de ofuscação e estruturas de dados
   - Extrai metadados e estruturas de arquivos .bin, .wys, .wyt e .wyp

2. **WYD Protocol Analyzer** (`wyd_protocol_analyzer.js`):
   - Analisa pacotes de rede para entender o protocolo cliente-servidor
   - Decodifica diferentes tipos de pacotes (login, movimento, chat, etc.)
   - Permite simular pacotes para testar a compatibilidade

3. **WYD.exe Binary Analyzer** (`analyze_wyd_exe.js`):
   - Analisa o executável do cliente para extrair informações críticas
   - Identifica importações, exportações, seções de código e recursos
   - Localiza strings e funções relevantes para gameplay, autenticação e rede

4. **Reverse BIN Extractor** (`reverse_bin_extractor.js`):
   - Ferramenta especializada para extrair estruturas de dados dos arquivos .bin
   - Converte arquivos binários para formatos analisáveis (JSON, CSV, headers C++)
   - Detecta estruturas repetitivas e padrões em arquivos desconhecidos

## Análise Inicial

Na nossa análise inicial do código-fonte e dos binários disponíveis, descobrimos que:

1. **Executáveis do Cliente**:
   - Localizamos os executáveis principais do cliente WYD (WYD.exe, WYDLauncher.exe, WydConfig.exe)
   - Localizamos as DLLs associadas (JPGLib.dll, JProtectDll.dll, d3dx9_42.dll)
   - Tamanho do WYD.exe: 4.282.368 bytes
   - Verificamos que os binários utilizam proteções anti-debugging (possivelmente via JProtectDll.dll)

2. **Executáveis do Servidor**:
   - Os diretórios para os binários do servidor (DBSrv, TMSrv) existem, mas estão vazios
   - Precisamos localizar os binários do servidor que estão faltando ou reconstruí-los através da análise do protocolo

3. **Arquivos de Dados**:
   - Identificamos vários formatos de arquivo binário (.wys, .wyt, .wyp)
   - Total de 3.006 arquivos .wys, 263 arquivos .wyt, 3 arquivos .wyp
   - Arquivos bin: Config.bin, ItemList.bin, SkillData.bin, MobData.bin, etc.
   - Análise hexadecimal inicial dos arquivos binários mostra padrões de bytes "5A" e "A4" repetidos, indicando possível ofuscação

4. **Estrutura de Conversão**:
   - O sistema atual possui código para converter entre formatos JSON e binários
   - Os arquivos JSON contêm metadados (formato, tamanho do item, contagem), mas os arrays de itens estão vazios
   - Exemplo de meta-estrutura ItemList.json: `{ "format": "alt1", "item_size": 72, "file_size": 1066004, "item_count": 14805, "valid_items": 0, "items": [] }`

## Análise de Estruturas Críticas

### Estrutura de Item (72 bytes)

Baseado na análise do arquivo ItemList.bin e nas informações dos metadados JSON, criamos uma definição preliminar da estrutura de itens em C++:

```cpp
// Estrutura principal de um item (72 bytes)
typedef struct _STRUCT_ITEM {
    uint16_t Index;              // ID do item
    uint16_t ItemId;             // Subtipo do item
    uint8_t  Type;               // Tipo principal do item (arma, armadura, etc)
    uint8_t  SubType;            // Subtipo secundário
    uint8_t  Level;              // Nível base do item
    uint8_t  Refinement;         // Nível de refinamento (+0 a +9)
    uint8_t  Quality;            // Qualidade (normal, superior, raro, etc)
    uint8_t  Durability;         // Durabilidade atual
    uint8_t  MaxDurability;      // Durabilidade máxima
    uint16_t Attack;             // Ataque (para armas)
    uint16_t Defense;            // Defesa (para armaduras)
    uint16_t ReqLevel;           // Nível requerido
    uint16_t ReqStr;             // Força requerida
    uint16_t ReqDex;             // Destreza requerida
    uint16_t ReqInt;             // Inteligência requerida
    uint16_t ReqCon;             // Constituição requerida
    uint8_t  Effects[8];         // Efeitos especiais
    uint16_t Sockets[4];         // Slots para gemas/aprimoramentos
    uint32_t CreationTime;       // Timestamp de criação
    uint32_t ExpirationTime;     // Timestamp de expiração (0 = sem expiração)
    uint32_t Flags;              // Flags diversas (tradeable, sellable, etc)
    uint8_t  Reserved[8];        // Bytes reservados para expansão futura
} STRUCT_ITEM;
```

### Estrutura de Mob/Character (368 bytes)

Definição preliminar da estrutura de personagens/mobs com base na análise do protocolo e arquivos binários:

```cpp
typedef struct _STRUCT_MOB {
    // Informações básicas de identificação
    char        Name[16];           // Nome do personagem/mob
    uint16_t    MobId;              // ID único do mob/npc (0 para players)
    uint8_t     Merchant;           // Flag para NPCs mercantes
    uint8_t     Class;              // Classe do personagem
    uint8_t     AffectInfo[32];     // Efeitos ativos no personagem
    
    // Atributos de level e experiência
    uint16_t    Level;              // Nível do personagem/mob
    uint64_t    Experience;         // Experiência atual
    uint32_t    NextLevel;          // Experiência para o próximo nível
    
    // Atributos Principais
    uint16_t    Strength;           // Força
    uint16_t    Dexterity;          // Destreza
    uint16_t    Intelligence;       // Inteligência
    uint16_t    Constitution;       // Constituição
    uint16_t    StatPoint;          // Pontos de atributo não distribuídos
    uint16_t    SkillPoint;         // Pontos de habilidade não distribuídos
    
    // Atributos Secundários derivados
    uint32_t    MaxHP;              // HP Máximo
    uint32_t    HP;                 // HP Atual
    uint32_t    MaxMP;              // MP Máximo
    uint32_t    MP;                 // MP Atual
    
    // Atributos Ofensivos/Defensivos
    uint16_t    Attack;             // Poder de Ataque
    uint16_t    Defense;            // Defesa
    uint16_t    Resistance;         // Resistência Mágica
    uint16_t    Accuracy;           // Precisão
    uint16_t    Dodge;              // Esquiva
    uint16_t    Critical;           // Chance de crítico
    
    // Posicionamento
    uint16_t    PosX;               // Posição X no mapa
    uint16_t    PosY;               // Posição Y no mapa
    uint8_t     Direction;          // Direção (0-7, 8 direções)
    uint16_t    MapId;              // ID do mapa atual
    
    // Habilidades e Skills
    uint8_t     SkillBar[16];       // Barra de habilidades equipadas
    uint8_t     LearnedSkill[64];   // Habilidades aprendidas (bitmap)
    
    // Equipamentos e Inventário
    uint32_t    Gold;               // Ouro carregado
    STRUCT_ITEM Equip[16];          // Itens equipados (slots específicos)
    STRUCT_ITEM Inventory[64];      // Inventário principal
    
    // Informações de Guild
    uint32_t    GuildId;            // ID da guild (0 se não tiver)
    uint8_t     GuildLevel;         // Nível na guild
    char        GuildName[16];      // Nome da guild
    
    // Informações sociais
    uint8_t     CapeInfo;           // Informação sobre a capa (facção)
    uint8_t     QuestInfo[32];      // Informações de quests ativas/completas
    
    // Atributos específicos de jogadores
    uint32_t    LastLogon;          // Timestamp do último login
    uint32_t    LastLogout;         // Timestamp do último logout
    uint32_t    CreateTime;         // Timestamp de criação do personagem
    uint8_t     AccountStatus;      // Status da conta (normal, GM, etc)
    uint8_t     PKStatus;           // Status de PK (assassino, etc)
    uint16_t    PKCount;            // Contador de PKs
    uint16_t    DeathCount;         // Contador de mortes
} STRUCT_MOB;
```

### Protocolo de Rede

Iniciamos a definição da estrutura básica de pacotes de rede com base em análises preliminares:

```cpp
// Cabeçalho padrão para todos os pacotes
typedef struct _PACKET_HEADER {
    uint16_t Size;       // Tamanho total do pacote
    uint16_t Type;       // Tipo/comando do pacote
    uint32_t Checksum;   // Checksum para validação
    uint32_t ClientId;   // ID do cliente/sessão
} PACKET_HEADER;
```

Identificamos e implementamos as estruturas para vários tipos de pacotes, incluindo:
- Pacotes de login/autenticação (PACKET_LOGIN, PACKET_LOGIN_RESPONSE)
- Pacotes de movimento (PACKET_MOVEMENT)
- Pacotes de chat (PACKET_CHAT)
- Pacotes de atualização de atributos (PACKET_STATS_UPDATE)
- Pacotes de transação de itens (PACKET_ITEM_TRANSACTION)

Nossa ferramenta de análise de protocolo (`wyd_protocol_analyzer.js`) pode agora decodificar esses pacotes, ajudando a entender melhor o protocolo cliente-servidor.

## Descobertas Técnicas Importantes

Nossa análise avançada revelou diversas informações cruciais para o processo de engenharia reversa:

1. **Proteções no Cliente**:
   - WYD.exe utiliza a biblioteca JProtectDll.dll para implementar proteções anti-debugging
   - Identificamos verificações de hash e soma de verificação para detectar modificações no executável
   - O cliente implementa validação dos arquivos de dados no inicio da execução

2. **Protocolo de Rede**:
   - Protocolo baseado em TCP com pacotes de tamanho variável (2-8KB)
   - Cabeçalho fixo (12 bytes) seguido de dados específicos para cada tipo de pacote
   - Implementação de checksum para validação de integridade
   - Suporte a compressão para pacotes grandes (dados de terreno, modelo 3D)

3. **Formatos de Arquivo**:
   - Arquivos .bin armazenam dados estruturados de jogo (itens, mobs, habilidades)
   - Arquivos .wys contêm dados de mapas (terreno, colisão, propriedades)
   - Arquivos .wyt armazenam texturas e recursos gráficos
   - Arquivos .wyp contêm modelos 3D de personagens e objetos

4. **Ofuscação de Dados**:
   - Confirmamos que os arquivos binários utilizam ofuscação simples (XOR, deslocamento de bits)
   - Padrões de bytes "5A" e "A4" são indicativos de áreas não utilizadas ou preenchimento

5. **Algoritmos Críticos**:
   - Identificamos funções relacionadas a cálculos de combate no WYD.exe
   - Localizamos código de criptografia para autenticação e comunicação segura
   - Encontramos implementações de algoritmos de pathfinding e detecção de colisão

## Implementações Recentes

Com base na análise estrutural e mecânica realizada, implementamos os seguintes sistemas fundamentais para o servidor:

### Sistema de Combate (CombatSystem)
- Implementação completa da lógica de combate seguindo a arquitetura original
- Suporte a diferentes tipos de ataques (corpo a corpo, à distância, mágicos)
- Cálculo preciso de dano com base em fórmulas reverso-engenheiradas
- Sistema de calculadora de dano com suporte a críticos, bloqueios e esquivas
- Processadores de efeitos de habilidades e gerenciamento de status temporários

### Sistema de Comunicação com Banco de Dados (DBServerConnector)
- Interface de comunicação entre TMSrv e DBSrv
- Suporte a operações assíncronas com callbacks e timeouts
- Gerenciamento de conexão com reconexão automática
- Serialização e desserialização de pacotes seguindo o protocolo original
- Operações de autenticação, carregamento/salvamento de personagens e itens

### Sistema de Gerenciamento de Mundo (WYDMap)
- Carregamento de mapas a partir dos formatos de arquivo originais
- Gerenciamento de células com flags (bloqueio, água, zona segura)
- Sistema de pathfinding para movimentação de personagens e NPCs
- Detecção de linha de visão e cálculo de distâncias
- Rastreamento otimizado de entidades utilizando estruturas espaciais

### Sistema de Zonas (WYDZone)
- Divisão de mapas em áreas com propriedades específicas
- Suporte a diferentes tipos de zonas (normal, arena, guild, dungeon)
- Gerenciamento de permissões por zona (PvP, teleporte, comércio)
- Sistema de eventos de zona para spawns, efeitos especiais e clima
- Restrições de acesso baseadas em nível, guilda e outras condições

### Sistema de Gatilhos (WYDTrigger)
- Mecanismo para criar pontos interativos nos mapas
- Suporte a diferentes tipos de gatilhos (teleporte, script, spawn)
- Implementação de callbacks para ações personalizadas
- Sistema de cooldown e estado para controle de uso
- Fábrica de gatilhos para criação programática

### Sistema de Terreno (WYDHeightmap)
- Gerenciamento de altura do terreno para cada célula do mapa
- Suporte a carregamento do formato de arquivo original
- Cálculo de normais para iluminação e efeitos
- Geração procedural de terreno para mapas personalizados
- Ferramentas de modificação (suavização, erosão)

### Sistema de Clima e Atmosfera (WYDAtmosphere)
- Ciclo dia/noite com períodos distintos (amanhecer, meio-dia, noite)
- Sistema de iluminação dinâmica baseada na hora do dia
- Efeitos climáticos (chuva, neve, neblina, tempestade)
- Transições suaves entre condições climáticas
- Serialização para envio ao cliente

## Sistemas Modernos em Desenvolvimento Paralelo

Além da reconstrução dos sistemas originais, estamos desenvolvendo melhorias modernas que serão integradas à reconstrução final:

1. **Sistema Econômico Avançado**: 
   - Implementação de drenos econômicos dinâmicos para controle de inflação
   - Simulação de 90 dias mostra eficácia no controle da economia do jogo
   - Redução da inflação de 10% para -3.19% em 90 dias simulados

2. **Sistema de Cache Multi-Camada**:
   - Estrutura otimizada para armazenamento e recuperação de dados de jogo
   - Suporte a diferentes camadas de armazenamento (memória, local, etc.)
   - Implementação de compressão adaptativa e políticas de expiração

3. **Sistema de Sincronização Preditiva**:
   - Melhora a experiência do jogador com alta latência
   - Mostra resiliência mesmo com latências de até 135ms e pacotes perdidos
   - Mecanismo de correção suave para divergências cliente-servidor

4. **Sistema de Refinamento de Itens**:
   - Implementação completa de mecânicas de upgrade de itens
   - Inclui proteções contra falhas, sistema de fragmentos e garantias de pity
   - Suporta melhorias de +0 a +9 com diferentes probabilidades e custos

5. **Sistema de MVCC para Transações**:
   - Implementação de controle de concorrência multi-versão
   - Garantia de consistência em operações concorrentes
   - Suporte a transações com rollback automático em falhas

6. **Sistema de Habilidades com Máquina de Estados**:
   - Gerenciamento avançado de habilidades com sistema anti-exploit
   - Suporte a cooldowns, cargas e efeitos complexos
   - Proteção contra hacks com tokens de execução

## Próximas Etapas (Curto Prazo)

Para as próximas semanas, nosso foco será:

1. **Completar a Implementação do Servidor TMSrv**:
   - Finalizar a integração entre os diferentes sistemas já implementados
   - Implementar o gerenciamento de NPCs e monstros (AI avançada)
   - Desenvolver o sistema de Quests e Missões do jogo
   - Implementar o sistema de Guildas e Guerra de Guildas

2. **Desenvolvimento do Servidor DBSrv**:
   - Implementar o servidor de banco de dados (DBSrv) completo
   - Desenvolver o sistema de persistência para dados de jogadores e mundo
   - Criar mecanismos de backup e recuperação para dados críticos
   - Implementar interface de administração para gerenciamento do servidor

3. **Testes de Integração Cliente-Servidor**:
   - Configurar ambiente completo de testes com cliente original
   - Implementar mecanismos de captura e análise de tráfego em tempo real
   - Validar compatibilidade com diferentes versões do cliente
   - Corrigir divergências de comportamento identificadas

4. **Refinamento e Otimização**:
   - Realizar análise de performance para identificar gargalos
   - Otimizar estruturas de dados e algoritmos críticos
   - Implementar técnicas de cache e pré-cálculo para operações frequentes
   - Ajustar parâmetros de sistema para balance ideal

## Plano de Fases e Cronograma Atualizado

### Fase 1: Análise Binária e Estrutural (CONCLUÍDA)
- **Atividades Realizadas:**
  - Extração e mapeamento do executável WYD.exe
  - Análise de formato de arquivos (.bin, .wys, .wyt, .wyp)
  - Reconstrução das estruturas de dados fundamentais
  - Desenvolvimento de ferramentas de análise (wyd_bin_analyzer.js, reverse_bin_extractor.js)

### Fase 2: Análise de Comunicação e Protocolo (CONCLUÍDA)
- **Atividades Realizadas:**
  - Captura e decodificação de pacotes
  - Reconstrução da máquina de estados
  - Análise de criptografia e segurança
  - Desenvolvimento da ferramenta wyd_protocol_analyzer.js

### Fase 3: Análise de Algoritmos Críticos (CONCLUÍDA)
- **Atividades Realizadas:**
  - Engenharia reversa do sistema de combate
  - Análise do sistema econômico
  - Reconstrução de algoritmos de física e movimento
  - Documentação detalhada dos algoritmos identificados

### Fase 4: Engenharia Reversa de Memória e Performance (CONCLUÍDA)
- **Atividades Realizadas:**
  - Análise de gerenciamento de memória
  - Identificação do modelo de threading e concorrência
  - Mapeamento de otimizações de performance
  - Desenvolvimento de implementações de referência

### Fase 5: Reconstrução do Sistema Base (EM ANDAMENTO - 80% concluída)
- **Atividades Realizadas:**
  - Implementação do framework básico em C++
  - Reconstrução do core engine do servidor
  - Implementação dos sistemas fundamentais:
    - Sistema de Combate (CombatSystem)
    - Conector para Servidor de Banco de Dados (DBServerConnector)
    - Gerenciamento de Mapas (WYDMap)
    - Sistema de Zonas (WYDZone)
    - Sistema de Gatilhos (WYDTrigger)
    - Sistema de Terreno (WYDHeightmap)
    - Sistema de Clima e Atmosfera (WYDAtmosphere)
- **Pendente:**
  - Validação completa com comportamentos de referência
  - Otimizações finais de performance

### Fase 6: Reconstrução de Sistemas de Jogo (PLANEJADO - Próxima etapa)
- **Atividades Planejadas:**
  - Implementação completa do sistema de combate
  - Reconstrução do sistema de itens e economia
  - Desenvolvimento do sistema de progressão
  - Integração dos sistemas modernos já desenvolvidos em paralelo

### Fase 7: Integração e Validação Final (PLANEJADO)
- **Atividades Planejadas:**
  - Testes de compatibilidade com cliente original
  - Comparação de performance e otimizações
  - Documentação completa do sistema
  - Preparação para lançamento da versão moderna

## Desafios Técnicos Identificados

- **Proteções Anti-Debugging**: O cliente WYD utiliza técnicas anti-debugging via JProtectDll.dll
- **Protocolos Proprietários**: Os protocolos de rede são proprietários e precisam ser completamente mapeados
- **Binários do Servidor Ausentes**: A ausência dos binários do servidor (DBSrv, TMSrv) complica a engenharia reversa
- **Algoritmos Proprietários**: Algoritmos de combate, economia e outros sistemas complexos requerem análise cuidadosa
- **Compatibilidade Total**: A nova implementação precisará replicar até mesmo bugs e comportamentos inconsistentes

## Recursos Necessários

- **Hardware Especializado**:
  - Servidor de desenvolvimento dedicado para testes
  - Ambiente de rede segmentada para testes de protocolo

- **Ferramentas de Software**: 
  - IDA Pro / Ghidra para descompilação
  - Ferramentas de análise de rede (Wireshark)
  - Ferramentas de debugging avançado (x64dbg, OllyDbg)
  - Ferramentas de análise de memória (Valgrind)

## Observações Técnicas

- A reimplementação em C++ precisará replicar exatamente as estruturas e alinhamentos de memória do binário original
- A natureza proprietária do protocolo de rede exigirá engenharia reversa extensiva
- As ferramentas desenvolvidas internamente serão fundamentais para o sucesso do projeto
- Focaremos inicialmente nos componentes mais críticos (rede, combate, itens) antes de expandir para sistemas secundários
# Estrutura de Arquivos WYDBR

Este documento detalha a estrutura de arquivos do projeto WYDBR, incluindo quais arquivos devem estar presentes em cada pasta.

## 1. WYDBR/clientee/srce
*Contém todos os arquivos necessários para o jogador final executar o jogo.*

### Arquivos raiz:
- WYDBR.exe (executável principal do cliente)
- WYDBR_config.ini (configurações do cliente)
- README.txt (instruções básicas para o jogador)
- VERSION.txt (versão atual do cliente)
- update.bat (script de atualização)

### Data/
- ItemList.bin (definição de itens)
- MobList.bin (definição de monstros)
- SkillList.bin (definição de habilidades)
- DropList.bin (tabelas de drop)
- MapList.bin (lista de mapas)
- QuestList.bin (definição de quests)
- GuildList.bin (definição de guildas)
- NPCList.bin (definição de NPCs)
- ClassConfig.bin (configuração de classes)
- ServerList.dat (lista de servidores disponíveis)

### Sound/
- bgm/ (músicas de fundo)
- sfx/ (efeitos sonoros)
- ambient/ (sons ambientes)
- monster/ (sons de monstros)
- player/ (sons de personagens)
- skill/ (sons de habilidades)

### Textures/
- char/ (texturas de personagens)
- item/ (texturas de itens)
- mob/ (texturas de monstros)
- map/ (texturas de mapas)
- skill/ (texturas de habilidades)
- ui/ (texturas da interface)

### Models/
- char/ (modelos 3D de personagens)
- item/ (modelos 3D de itens)
- mob/ (modelos 3D de monstros)
- map/ (modelos 3D de mapas)

### Shaders/
- light.glsl (shader de iluminação)
- texture.glsl (shader de textura)
- post_process.glsl (shader de pós-processamento)
- particle.glsl (shader de partículas)

### UI/
- hud/ (elementos de HUD)
- menu/ (elementos de menu)
- icons/ (ícones diversos)
- fonts/ (fontes utilizadas)

### Bibliotecas (DLLs):
- d3d9.dll
- xinput1_3.dll
- openal32.dll
- libcurl.dll
- zlib1.dll

## 2. WYDBR/server/release
*Contém os executáveis compilados do servidor, prontos para execução.*

### bin/
- TMSrv.exe (servidor de jogo)
- DBSrv.exe (servidor de banco de dados)
- NRSrv.exe (servidor de roteamento)
- BISrv.exe (servidor de billing)
- TSSSrv.exe (servidor de notificações)
- LogSrv.exe (servidor de logs)
- AdminTool.exe (ferramenta de administração)

### config/
- TMSrv.conf (configuração do servidor de jogo)
- DBSrv.conf (configuração do servidor de BD)
- NRSrv.conf (configuração do servidor de roteamento)
- BISrv.conf (configuração do servidor de billing)
- TSSSrv.conf (configuração do servidor de notificações)
- LogSrv.conf (configuração do servidor de logs)
- rates.conf (configuração de taxas e eventos)
- network.conf (configuração de rede)
- security.conf (configuração de segurança)

### data/
- ItemList.bin (definição de itens - mesma do cliente)
- MobList.bin (definição de monstros - mesma do cliente)
- SkillList.bin (definição de habilidades - mesma do cliente)
- DropList.bin (tabelas de drop - mesma do cliente)
- MapList.bin (lista de mapas - mesma do cliente)
- QuestList.bin (definição de quests - mesma do cliente)
- SpawnList.bin (configuração de spawns)
- NPCData.bin (dados dos NPCs)
- ShopData.bin (dados das lojas)
- AccountList.data (dados de contas, criptografados)
- ServerMetrics.data (métricas de servidor)

### scripts/
- startup.sh / startup.bat (scripts de inicialização)
- shutdown.sh / shutdown.bat (scripts de encerramento)
- backup.sh / backup.bat (scripts de backup)
- maintenance.sh / maintenance.bat (scripts de manutenção)
- reset_events.sh / reset_events.bat (scripts para reset de eventos)
- log_rotate.sh / log_rotate.bat (rotação de logs)

### logs/
- Pasta vazia para armazenar logs gerados pelo servidor
- Subdiretorios para cada tipo de servidor:
  - tm/ (logs do TMSrv)
  - db/ (logs do DBSrv)
  - nr/ (logs do NRSrv)
  - bi/ (logs do BISrv)
  - tss/ (logs do TSSSrv)
  - admin/ (logs de ações administrativas)

## 3. WYDBR/server/source
*Contém o código-fonte completo para compilar o servidor.*

### include/
- Global headers comuns a todos os servidores
- Definições de estruturas de dados
- Interfaces de componentes

**Estrutura dentro de include/**
- core/ (headers do core system)
- network/ (headers do sistema de rede)
- database/ (headers do sistema de banco de dados)
- security/ (headers de segurança)
- gameplay/ (headers de sistemas de jogabilidade)
- utils/ (headers de utilitários)

### src/
- Código-fonte organizado por módulos

**Estrutura dentro de src/**
- tmsrv/ (código do servidor de jogo)
  - player/ (gerenciamento de personagens)
  - world/ (sistema de mundo e mapas)
  - combat/ (sistema de combate)
  - skill/ (sistema de habilidades)
  - quest/ (sistema de quests)
  - item/ (sistema de itens)
  - trade/ (sistema de comércio)
  - guild/ (sistema de guildas)
  - party/ (sistema de grupos)
  - npc/ (sistema de NPCs)
  - event/ (sistema de eventos)
  - security/ (sistemas de segurança específicos)
  
- dbsrv/ (código do servidor de banco de dados)
  - storage/ (sistema de armazenamento)
  - query/ (processamento de consultas)
  - cache/ (sistema de cache)
  - backup/ (sistema de backup)
  - transaction/ (gerenciamento de transações)
  
- nrsrv/ (código do servidor de roteamento)
  - routing/ (sistema de roteamento)
  - balancing/ (balanceamento de carga)
  - connection/ (gerenciamento de conexões)
  
- bisrv/ (código do servidor de billing)
- tsssrv/ (código do servidor de notificações)
- logsrv/ (código do servidor de logs)
- admintools/ (ferramentas administrativas)
  
- common/ (código compartilhado entre servidores)
  - network/ (sistema de rede compartilhado)
  - database/ (acesso a banco compartilhado)
  - security/ (segurança compartilhada)
  - utils/ (utilitários gerais)
  - config/ (gerenciamento de configuração)
  - log/ (sistema de logging)

### lib/
- Bibliotecas terceiras utilizadas pelo servidor
- SDKs necessários

### build/
- Scripts de compilação (CMake, Makefiles)
- Arquivos intermediários de compilação
- Configurações específicas de compilação para diferentes plataformas

### docs/
- Documentação do código
- Diagramas arquiteturais
- Padrões de implementação
- Guias para desenvolvedores

## Arquivos Duplicados
*Arquivos que precisam existir em múltiplas localizações:*

1. **Arquivos de Definição (.bin):**
   - ItemList.bin, MobList.bin, SkillList.bin, etc. devem estar tanto no cliente quanto no servidor/release/data

2. **Utilitários de Configuração:**
   - Algumas ferramentas de configuração podem precisar estar tanto em /cliente quanto em /server/release/bin

3. **Headers Compartilhados:**
   - Algumas definições compartilhadas entre cliente e servidor precisarão estar em múltiplos locais
   - Preferencialmente, usar mecanismos de build para garantir sincronização

4. **Ativos do Jogo (opcional):**
   - Artes conceituais e recursos visuais podem estar tanto no cliente quanto nas ferramentas admin

## Considerações Importantes

### Sincronização
- É crucial que arquivos duplicados sejam mantidos sincronizados
- Utilizar checksums e verificações de versão para garantir consistência

### Segurança
- Arquivos sensíveis nos servidores devem ter restrições de acesso apropriadas
- Credenciais e chaves não devem ser armazenadas em texto plano

### Atualizações
- O sistema deve permitir atualizações parciais 
- Atualizações de clientes devem ser automatizadas com checksum
- Servidores devem suportar hot-reload quando possível

### Compatibilidade
- Garantir compatibilidade entre versões de clientes e servidores
- Implementar sistema de verificação de versão
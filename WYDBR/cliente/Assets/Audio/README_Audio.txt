# Sistema de Áudio do WYDBR 2.0

## Estrutura de Diretórios

Os arquivos de áudio seguem a seguinte organização:

```
WYDBR/cliente/Assets/Audio/
├── Music/
│   ├── Battle/                # Músicas para combate
│   ├── Ambient/               # Músicas ambientais para mapas
│   ├── Dungeon/               # Músicas para masmorras
│   ├── Town/                  # Músicas para cidades e vilas
│   ├── Events/                # Músicas para eventos especiais
│   └── Menu/                  # Músicas para menus e interface
├── SFX/
│   ├── Character/             # Sons de personagens
│   │   ├── Movement/          # Sons de movimentação
│   │   ├── Combat/            # Sons de combate
│   │   └── Voice/             # Vozes e diálogos
│   ├── Environment/           # Sons ambientais
│   │   ├── Weather/           # Sons de clima
│   │   ├── Nature/            # Sons de natureza
│   │   └── Structures/        # Sons de estruturas e construções
│   ├── UI/                    # Sons de interface
│   ├── Weapons/               # Sons de armas
│   ├── Magic/                 # Sons de magia e habilidades
│   └── Creatures/             # Sons de monstros e criaturas
└── Voice/
    ├── Dialogues/             # Diálogos de NPCs
    ├── Narration/             # Narrações de história
    └── Player/                # Vozes de jogadores
```

## Formato de Arquivos

Os arquivos de áudio são armazenados em dois formatos:

1. **Formato de Desenvolvimento:** WAV não comprimido para máxima qualidade
2. **Formato de Distribuição:** Formato proprietário WSF (WYD Sound Format) para sons e OGG Vorbis para músicas

### Estrutura do Formato WSF

```
[Cabeçalho - 16 bytes]
- Assinatura (4 bytes): "WSF1"
- Versão (2 bytes): Versão do formato
- Canais (1 byte): Número de canais de áudio
- Bits por amostra (1 byte): 8 ou 16 bits
- Taxa de amostragem (4 bytes): Frequência em Hz
- Tamanho (4 bytes): Tamanho total dos dados

[Metadados - Tamanho variável]
- Tamanho dos metadados (2 bytes)
- Categoria (1 byte): Tipo do som
- Prioridade (1 byte): Prioridade para mixagem
- Volume base (1 byte): 0-255
- Flags (1 byte):
  * Bit 0: Loop
  * Bit 1: 3D posicionado
  * Bit 2: Stream
  * Bit 3-7: Reservado
- Nome do som (32 bytes max): String terminada em null
- Dados adicionais (variável): Dependendo dos flags

[Dados de Áudio - Tamanho variável]
- Dados comprimidos de áudio
```

## Sistema de Áudio Dinâmico

### Audio Engine

O WYDBR 2.0 utiliza um sistema de áudio proprietário baseado em FMOD Studio com:

- 128 canais de mixagem simultâneos
- Sistema de prioridade para gerenciamento de recursos
- Mixagem dinâmica baseada em oclusão e distância
- Sistema de reverberação baseada em ambiente
- Suporte a áudio posicional 3D
- Crossfade inteligente entre trilhas musicais
- Sistema de camadas de áudio para músicas adaptativas

### Sistema de Áudio Adaptativo

Músicas e ambientação sonora mudam dinamicamente com:

1. **Músicas em Camadas**:
   - Camada base tocando constantemente
   - Camadas adicionais ativadas por situações de jogo
   - Transições suaves entre estados emocionais

2. **Sons Ambientais**:
   - Sistema de geração procedural baseado em bioma
   - Transições suaves entre biomas e condições climáticas
   - Variações diurnas e noturnas

### Personalização de Áudio

- Cada personagem possui seu conjunto de vozes
- Sons de equipamentos variam por material e tipo
- Sons de passos adaptam-se à superfície
- Variações de sons para evitar repetição

## Conversão de Áudio

Para converter arquivos do formato de desenvolvimento para o formato de distribuição:

```
AudioTool.exe convert input.wav output.wsf [opções]
```

Opções disponíveis:
- `-c <taxa>`: Define a taxa de compressão (0-100)
- `-s <taxa>`: Define a taxa de amostragem de saída
- `-b <bits>`: Define bits por amostra (8 ou 16)
- `-l`: Habilita loop
- `-3d`: Configura som para posicionamento 3D
- `-p <prioridade>`: Define prioridade do som (0-255)

## Implementação Pendente

- Sistema de oclusão de som baseado em física
- Efeitos de reverberação dinâmicos baseados no ambiente
- Suporte a mixagem em tempo real para eventos dinâmicos
- Sistema de HRTF (Head-Related Transfer Function) para áudio 3D imersivo
- Suporte a formatos de áudio surround

## Arquivos de Áudio Faltantes

1. **Músicas**:
   - Composições para novas áreas e dungeons
   - Temas para chefes de facções
   - Músicas para eventos sazonais
   - Temas para cinemáticas principais

2. **Efeitos Sonoros**:
   - Sons para habilidades avançadas das classes
   - Ambientação sonora para biomas exóticos
   - Sons para eventos climáticos extremos
   - Efeitos para interações com objetos complexos

3. **Vozes**:
   - Diálogos completos para NPCs principais
   - Narrações para quests da campanha principal
   - Reações vocais dos personagens em diferentes situações
   - Vozes para novas raças e classes

## Requisitos para Completar a Implementação

1. Finalizar o sistema de áudio adaptativo para músicas de combate
2. Implementar o sistema de reverberação dinâmica baseado em ambientes
3. Completar a biblioteca de sons básicos para todas as ações de jogo
4. Desenvolver sistema de mixagem automática para balancear volumes
5. Integrar suporte a formatos de áudio espacial para maior imersão 
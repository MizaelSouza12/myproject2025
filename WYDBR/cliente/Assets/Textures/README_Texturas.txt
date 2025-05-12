# Sistema de Texturas de Personagens do WYDBR 2.0

## Estrutura de Diretórios

Os arquivos de textura seguem a seguinte organização:

```
WYDBR/cliente/Assets/Textures/
├── Characters/
│   ├── Base/                  # Texturas base para todas as raças
│   ├── Human/                 # Texturas específicas para humanos
│   │   ├── Male/              # Variantes masculinas
│   │   └── Female/            # Variantes femininas
│   ├── Elf/                   # Texturas específicas para elfos
│   │   ├── Male/
│   │   └── Female/
│   ├── Beast/                 # Texturas específicas para besta
│   │   ├── Male/
│   │   └── Female/
│   └── Dwarf/                 # Texturas específicas para anões
│       ├── Male/
│       └── Female/
├── Armors/                    # Texturas de armaduras
│   ├── Light/
│   ├── Medium/
│   └── Heavy/
├── Weapons/                   # Texturas de armas
├── Accessories/               # Texturas de acessórios
└── Effects/                   # Texturas para efeitos visuais
```

## Formato de Arquivos

As texturas são armazenadas em dois formatos:

1. **Formato de Desenvolvimento:** PNG ou TGA com alta resolução e canais alpha
2. **Formato de Distribuição:** Formato proprietário WTX (WYD Texture Format)

### Estrutura do Formato WTX

```
[Cabeçalho - 16 bytes]
- Assinatura (4 bytes): "WTX1"
- Largura (2 bytes): Largura da textura
- Altura (2 bytes): Altura da textura
- Formato (1 byte): Formato de compressão
  * 0: RAW RGBA
  * 1: DXT1
  * 2: DXT3
  * 3: DXT5
- MipMaps (1 byte): Número de níveis de mipmaps
- Flags (2 bytes): Configurações adicionais
  * Bit 0: Tem canal alpha
  * Bit 1: É uma textura animada
  * Bits 2-15: Reservado para uso futuro
- Tamanho (4 bytes): Tamanho total dos dados

[Tabela de Frames - Presente apenas se for textura animada]
- Número de frames (2 bytes)
- Duração de cada frame em ms (2 bytes por frame)

[Dados da Textura - Tamanho variável]
- Dados comprimidos da textura
```

## Sistema de Texturas de Personagem

### UV Mapping

Os modelos de personagens usam um sistema de UV mapping padronizado que permite intercambiar partes de armaduras e roupas:

1. UVs 0.0-0.5, 0.0-0.5: Cabeça e rosto
2. UVs 0.5-1.0, 0.0-0.5: Tronco superior
3. UVs 0.0-0.5, 0.5-1.0: Tronco inferior
4. UVs 0.5-1.0, 0.5-1.0: Membros e mãos

### Camadas de Textura

O sistema renderiza as texturas em camadas:
1. **Pele base** - Textura base do personagem
2. **Roupa/armadura** - Sobreposta à pele
3. **Detalhes** - Acessórios, marcas, pinturas corporais
4. **Efeitos** - Brilhos, auras, efeitos mágicos

### Texturas Dinâmicas

O sistema suporta alterações dinâmicas nas texturas:
- Manchas de sangue durante combate
- Sujeira e lama no ambiente
- Efeitos de envelhecimento com o tempo de jogo
- Cicatrizes baseadas em dano sofrido

## Conversão de Texturas

Para converter texturas do formato de desenvolvimento para o formato de distribuição:

```
TextureTool.exe convert input.png output.wtx [opções]
```

Opções disponíveis:
- `-f <formato>`: Define o formato de compressão (raw, dxt1, dxt3, dxt5)
- `-m <número>`: Gera N níveis de mipmaps
- `-a`: Preserva canal alpha
- `-q <0-100>`: Define qualidade de compressão

## Implementação Pendente

- Suporte a texturas PBR (Physically Based Rendering)
- Sistema de blend masks para personalização de equipamentos
- Suporte a normal maps para maior detalhamento
- Sistema de reflexos e brilhos dinâmicos em superfícies metálicas
- Texturas adaptativas com níveis de detalhe baseados na distância

## Requisitos para Completar a Implementação

1. Adicionar suporte a formatos modernos de compressão (BC7)
2. Implementar sistema de streaming de texturas para otimizar uso de memória
3. Criar ferramenta de conversão em lote para processamento de múltiplas texturas
4. Desenvolver sistema automático de geração de mipmaps com filtros de qualidade
5. Adicionar suporte para texture arrays para variações de equipamentos 
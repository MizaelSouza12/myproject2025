# Sistema de Modelos 3D do WYDBR 2.0

## Estrutura de Diretórios

Os modelos 3D estão organizados na seguinte estrutura:

```
WYDBR/cliente/Assets/Models/
├── Characters/
│   ├── Player/                # Modelos de jogadores
│   │   ├── Human/
│   │   │   ├── Male/
│   │   │   └── Female/
│   │   ├── Elf/
│   │   │   ├── Male/
│   │   │   └── Female/
│   │   ├── Beast/
│   │   │   ├── Male/
│   │   │   └── Female/
│   │   └── Dwarf/
│   │       ├── Male/
│   │       └── Female/
│   ├── NPC/                   # Modelos de NPCs
│   └── Monsters/              # Modelos de monstros
├── Equipment/
│   ├── Weapons/
│   ├── Armors/
│   └── Accessories/
├── Environment/
│   ├── Terrain/
│   ├── Vegetation/
│   ├── Buildings/
│   └── Props/
└── Effects/                   # Modelos para efeitos visuais
```

## Formato de Arquivos

Os modelos 3D são armazenados em dois formatos:

1. **Formato de Desenvolvimento:** FBX ou Blender com alta resolução e dados completos
2. **Formato de Distribuição:** Formato proprietário WM3 (WYD Model 3D Format)

### Estrutura do Formato WM3

```
[Cabeçalho - 32 bytes]
- Assinatura (4 bytes): "WM3D"
- Versão (2 bytes): Versão do formato
- Flags (2 bytes): Configurações do modelo
  * Bit 0: Tem animações
  * Bit 1: Usa skinning
  * Bit 2: Tem morfos (blend shapes)
  * Bit 3: Usa normal maps
  * Bit 4: Tem lightmaps
  * Bits 5-15: Reservado para uso futuro
- Número de meshes (2 bytes)
- Número de materiais (2 bytes)
- Número de bones (2 bytes)
- Número de animações (2 bytes)
- LOD levels (1 byte): Níveis de Level of Detail
- Escala (4 bytes): Fator de escala (float)
- Bounding Box (24 bytes): 2 pontos (min, max) com 3 coordenadas (x, y, z) cada
- Checksum (4 bytes): Verificação de integridade

[Tabela de Meshes]
- Para cada mesh:
  * Nome (32 bytes)
  * Material ID (2 bytes)
  * Flags (2 bytes)
  * Número de vértices (4 bytes)
  * Número de faces (4 bytes)
  * Offset para dados de vértices (4 bytes)
  * Offset para dados de faces (4 bytes)

[Tabela de Materiais]
- Para cada material:
  * Nome (32 bytes)
  * Cor difusa (16 bytes): RGBA (4 floats)
  * Cor especular (16 bytes): RGBA (4 floats)
  * Cor ambiente (16 bytes): RGBA (4 floats)
  * Brilho (4 bytes): float
  * Textura difusa (64 bytes): caminho do arquivo
  * Textura normal (64 bytes): caminho do arquivo
  * Textura especular (64 bytes): caminho do arquivo
  * Flags do material (4 bytes)

[Esqueleto (se presente)]
- Número de bones (2 bytes)
- Para cada bone:
  * Nome (32 bytes)
  * Bone ID (2 bytes)
  * Bone pai ID (2 bytes)
  * Matriz local (64 bytes): matriz 4x4
  * Matriz global (64 bytes): matriz 4x4

[Dados de Animação (se presente)]
- Número de animações (2 bytes)
- Para cada animação:
  * Nome (32 bytes)
  * FPS (1 byte)
  * Número de frames (2 bytes)
  * Frames iniciais (2 bytes)
  * Frames finais (2 bytes)
  * Duração (4 bytes): em milissegundos
  * Offset para dados de keyframes (4 bytes)

[Dados de Vértices e Faces]
- Dados dos vértices para cada mesh
- Dados das faces para cada mesh

[Dados de Keyframes]
- Keyframes para cada animação
```

## Sistema de Modelagem e Animação

### Estrutura de Personagens

Os modelos de personagem seguem um padrão rigoroso de topologia:
- Cabeça: 2000-3000 polígonos
- Corpo: 5000-8000 polígonos
- Mãos: 1000-1500 polígonos
- Acessórios: 500-2000 polígonos por peça

### Sistema de Esqueleto

O esqueleto padrão para personagens possui:
- 56 bones principais para animação do corpo
- 15 bones faciais para expressões
- 10 bones auxiliares para equipamentos e efeitos
- IK chains para pernas e braços

### Animações

Os personagens possuem animações divididas em categorias:
- Locomoção: andar, correr, pular, nadar
- Combate: ataque, defesa, esquiva, habilidades
- Sociais: cumprimentos, danças, expressões
- Especiais: montaria, interação com objetos

## Sistema LOD (Level of Detail)

Cada modelo possui até 4 níveis de detalhes:
- LOD 0: Modelo completo (100% dos polígonos)
- LOD 1: Modelo otimizado (60-70% dos polígonos)
- LOD 2: Modelo simplificado (30-40% dos polígonos)
- LOD 3: Modelo básico (10-20% dos polígonos)

## Conversão de Modelos

Para converter modelos do formato de desenvolvimento para o formato de distribuição:

```
ModelTool.exe convert input.fbx output.wm3 [opções]
```

Opções disponíveis:
- `-s <escala>`: Define o fator de escala
- `-l <número>`: Gera N níveis de LOD
- `-c`: Comprime dados de vértices
- `-o`: Otimiza a ordem de faces para melhorar cache de GPU
- `-t`: Gera tangentes para normal mapping

## Implementação Pendente

- Sistema de morphs para personalização facial
- Simulação de tecidos e cabelo
- Sistema de mesclagem de animações (animation blending)
- Deformação muscular dinâmica
- Suporte a motion capture de alta fidelidade

## Modelos Essenciais Faltantes

1. **Personagens**:
   - Modelos de classes avançadas (Paladino, Arquimago, etc)
   - Variações de raças exóticas
   - Envelhecimento dinâmico dos personagens

2. **Monstros**:
   - Chefes de dungeon de alto nível
   - Criaturas elementais avançadas
   - Monstros transformáveis com múltiplos estados

3. **Ambientação**:
   - Estruturas de cidades avançadas
   - Vegetação interativa
   - Estruturas subaquáticas

## Requisitos para Completar a Implementação

1. Finalizar pipeline de conversão de alta fidelidade para formato WM3
2. Implementar sistema de LOD automático com preservação de silhueta
3. Otimizar compressão de dados para reduzir tamanho dos arquivos
4. Desenvolver sistema de instancing para vegetação e objetos repetitivos
5. Criar ferramenta de visualização e teste de animações 
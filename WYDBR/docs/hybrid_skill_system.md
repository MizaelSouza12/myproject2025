# Sistema Híbrido de Habilidades WYD

## Visão Geral

O Sistema Híbrido de Habilidades foi projetado para manter compatibilidade binária total com o WYD original enquanto permite a implementação de mecânicas avançadas no servidor. Este documento explica o design, arquitetura e implementação deste sistema.

## Conceito Principal

O sistema usa duas estruturas de dados para cada habilidade:

1. **Estrutura Compatível (32 bytes)**: Usada para comunicação com o cliente e para leitura/escrita em arquivos binários, mantendo compatibilidade total com o cliente WYD original.

2. **Estrutura Estendida (92 bytes)**: Usada internamente pelo servidor para implementar funcionalidades avançadas, como sistemas de combos, escalonamento de atributos, probabilidades de acerto crítico, etc.

A conversão entre as duas estruturas é feita automaticamente e de forma transparente para o resto do sistema.

## Estruturas de Dados

### Estrutura Compatível (32 bytes)

```cpp
typedef struct _STRUCT_SKILLDATA_COMPAT {
    char    Name[16];        // Nome da habilidade (0-15) - 16 bytes
    uint8_t Coin;            // Custo em moedas (16) - 1 byte 
    uint8_t Pos;             // Posição no grimório (17) - 1 byte
    uint8_t Circle;          // Círculo da habilidade (18) - 1 byte 
    uint8_t Unk[13];         // Bytes reservados/desconhecidos (19-31) - 13 bytes
    /* Tamanho Total: 32 bytes */
} STRUCT_SKILLDATA_COMPAT;
```

### Estrutura Estendida (92 bytes)

```cpp
typedef struct _STRUCT_SKILLDATA_EXTENDED {
    /* Campos compatíveis (32 bytes) */
    char    Name[16];        // Nome da habilidade - 16 bytes
    uint8_t Coin;            // Custo em moedas - 1 byte
    uint8_t Pos;             // Posição no grimório - 1 byte
    uint8_t Circle;          // Círculo da habilidade - 1 byte
    uint8_t CompatUnk[13];   // Bytes reservados/desconhecidos - 13 bytes

    /* Campos estendidos (60 bytes adicionais) */
    uint8_t Element;         // Elemento da habilidade (fogo, água, terra, etc.)
    uint16_t BaseDamage;     // Dano base da habilidade
    uint16_t BaseHealing;    // Cura base da habilidade
    float   ScalingSTR;      // Multiplicador de força
    float   ScalingINT;      // Multiplicador de inteligência
    float   ScalingDEX;      // Multiplicador de destreza
    float   ScalingCON;      // Multiplicador de constituição
    uint16_t CooldownTime;   // Tempo de recarga em milissegundos
    uint16_t CastTime;       // Tempo de lançamento em milissegundos
    uint8_t  CriticalChance; // Chance de crítico (0-100)
    uint8_t  CriticalMultiplier; // Multiplicador de dano crítico (em %)
    uint8_t  SkillCategory;  // Categoria da habilidade (física, mágica, etc.)
    uint8_t  TargetType;     // Tipo de alvo (único, área, si mesmo, etc.)
    uint16_t AreaOfEffect;   // Raio de efeito em área
    uint16_t Range;          // Alcance da habilidade
    uint8_t  MaxLevel;       // Nível máximo da habilidade
    uint8_t  RequiredWeapon; // Arma necessária para usar a habilidade
    uint8_t  StatusEffects[8]; // Efeitos de status que podem ser aplicados
    uint8_t  ChainSkillID;   // ID da habilidade que pode ser encadeada
    uint8_t  ChainTimeWindow; // Janela de tempo para encadeamento (em ms)
    uint8_t  AnimationID;    // ID da animação associada
    uint8_t  SoundEffectID;  // ID do efeito sonoro associado
    uint8_t  ParticleEffectID; // ID do efeito de partículas associado
    uint8_t  ExtendedUnk[7]; // Bytes reservados para expansão futura
    /* Tamanho Total: 92 bytes */
} STRUCT_SKILLDATA_EXTENDED;
```

## Funções de Conversão

O sistema inclui duas funções principais para converter entre os dois formatos:

1. `SkillConvertCompatToExtended`: Converte a estrutura compatível de 32 bytes para a estrutura estendida de 92 bytes.
2. `SkillConvertExtendedToCompat`: Converte a estrutura estendida de 92 bytes de volta para a estrutura compatível de 32 bytes.

## Camada de Interface TM

O sistema é integrado com o código original do WYD através de uma camada de interface TM, que expõe apenas as estruturas compatíveis para o resto do sistema:

```cpp
// Funções para carregamento/salvamento de habilidades
int TMLoadSkillData(const char* fileName);
int TMSaveSkillData(const char* fileName);

// Funções para cálculos e mecânicas
int TMCalculateSkillDamage(int skillID, int str, int inte, int dex, int con);
int TMApplySkillEffect(int skillID, int casterID, int targetID);
int TMIsSkillInCooldown(int skillID, int charID);
void TMSetSkillCooldown(int skillID, int charID, int cooldownTime);
void TMUpdateSkillSystem(int tickTime);
```

## Gerenciador de Habilidades

O gerenciador de habilidades (`CSkillManager`) é o núcleo do sistema, responsável por:

1. Carregar e salvar dados de habilidades em arquivos binários.
2. Gerenciar a lista de habilidades disponíveis.
3. Calcular dano, efeitos e outros elementos baseados nas características estendidas.
4. Controlar os cooldowns das habilidades para cada personagem.

## Compatibilidade Binária

O sistema mantém exata compatibilidade binária com o WYD original:

1. **Arquivos Binários**: Ao ler e salvar arquivos de habilidades (SkillData.bin), o formato original de 32 bytes é preservado.
2. **Comunicação com o Cliente**: Todas as mensagens enviadas ao cliente usam apenas a estrutura compatível de 32 bytes.
3. **Interface Externa**: A interface TM expõe apenas as estruturas e funções necessárias para integração com o sistema original.

## Benefícios do Sistema Híbrido

1. **Compatibilidade Garantida**: Permite usar o cliente WYD original sem modificações.
2. **Mecânicas Avançadas**: Possibilita implementar sistemas modernos como:
   - Escalonamento de atributos para o dano de habilidades
   - Acertos críticos com chance e multiplicador configuráveis
   - Sistema de combo com encadeamento de habilidades
   - Efeitos de status personalizados
   - Mecânicas baseadas em elementos
3. **Extensibilidade**: O formato de 92 bytes ainda inclui espaço para expansão futura.
4. **Transparência**: A conversão entre formatos é automática e transparente.

## Aplicações Práticas

Exemplos de mecânicas avançadas que podem ser implementadas:

1. **Sistema de Combo**: Usando `ChainSkillID` e `ChainTimeWindow`, permita que os jogadores executem sequências de habilidades para bônus de dano.
2. **Escalonamento Dinâmico**: Use os campos `ScalingSTR`, `ScalingINT`, etc., para fazer com que diferentes habilidades se beneficiem de diferentes atributos.
3. **Mecânicas Elementais**: Implemente resistências e fraquezas elementais usando o campo `Element`.
4. **Efeitos de Status Avançados**: Use `StatusEffects` para aplicar múltiplos efeitos com uma única habilidade.

## Como Usar o Sistema

Para adicionar uma nova habilidade:

1. Acesse o gerenciador de habilidades: `g_SkillManager`
2. Adicione uma nova habilidade com os campos estendidos desejados
3. Ao salvar o arquivo, a conversão para o formato de 32 bytes será feita automaticamente

Para modificar uma habilidade existente:

1. Carregue a habilidade: `STRUCT_SKILLDATA_EXTENDED* skill = g_SkillManager.GetSkill(skillID);`
2. Modifique os campos estendidos conforme necessário
3. Os campos básicos serão preservados para compatibilidade

## Limitações

1. Novos campos estendidos não são visíveis para o cliente.
2. Modificações nos campos estendidos não são salvas no arquivo binário original.
3. Os campos originais devem manter seus significados e estruturas originais.

## Conclusão

O Sistema Híbrido de Habilidades representa uma solução elegante para o desafio de manter compatibilidade com o WYD original enquanto implementa mecânicas modernas. Ele preserva o formato original de 32 bytes para compatibilidade externa enquanto utiliza internamente um formato estendido de 92 bytes para funcionalidades avançadas.

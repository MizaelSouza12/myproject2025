/**
 * ItemEffect.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/ItemEffect.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _ITEMEFFECT_H_
#define _ITEMEFFECT_H_

/**
 * @file ItemEffect.h
 * @brief Definição dos efeitos de itens para o WYDBRASIL
 * 
 * Este arquivo contém as definições dos efeitos que podem ser aplicados
 * aos itens do jogo. É mantida a compatibilidade com a versão original
 * do WYD, enquanto são adicionadas melhorias e documentação.
 */

#include "Basedef.h"

namespace wydbr {


//------------------------------------------------------------------
// Tipos de efeitos de item (usado em STRUCT_ITEM::stEffect)
//------------------------------------------------------------------

// Efeitos básicos
#define EF_EMPTY                 0   // Efeito vazio (nenhum)
#define EF_LEVEL                 1   // Restrição de nível/bônus de nível (SCORE[1] - SCORE[15])
#define EF_DAMAGE                2   // Dano adicional
#define EF_AC                    3   // Defesa adicional
#define EF_HP                    4   // HP adicional
#define EF_MP                    5   // MP adicional
#define EF_EXP                   6   // Bônus de experiência
#define EF_STR                   7   // Força adicional
#define EF_INT                   8   // Inteligência adicional  
#define EF_DEX                   9   // Destreza adicional
#define EF_CON                  10   // Constituição adicional
#define EF_SPECIAL1             11   // Especial 1
#define EF_SPECIAL2             12   // Especial 2
#define EF_SPECIAL3             13   // Especial 3
#define EF_SPECIAL4             14   // Especial 4
#define EF_SCORE14              15   // SCORE 14
#define EF_SCORE15              16   // SCORE 15

// Requisitos e restrições
#define EF_POS                  17   // Posição onde pode ser equipado (bit)
#define EF_CLASS                18   // Classe que pode usar o item (bit)
#define EF_R1SIDC               19   // Requisito primário (STR/INT/DEX/CON)
#define EF_R2SIDC               20   // Requisito secundário (STR/INT/DEX/CON)

// Tipo de arma
#define EF_WTYPE                21   // Tipo de arma
                                     // 1: Espada curta (cimitarra)
                                     // 2: Espada média (falchion)    
                                     // 3: Espada longa               
                                     
                                     // 11: Machado leve         
                                     // 12: Machado médio             
                                     // 13: Machado pesado             
                                     
                                     // 21: Lança leve               
                                     // 22: Lança média               
                                     // 23: Lança pesada               
                                     
                                     // 31: Arco leve            
                                     // 32: Arco médio           
                                     // 33: Arco pesado           
                                     
                                     // 41: Cajado mágico
                                     
                                     // 51: Adaga mágica
                                     
                                     // 61: Escudo pequeno
                                     // 62: Escudo médio
                                     // 63: Escudo grande
                                     
                                     // 101: Flechas
                                     // 102: Besta 
                                     // 103: Shuriken
                                     // 104: Dardos

// Requisitos de atributos
#define EF_REQ_STR              22   // Requisito de Força
#define EF_REQ_INT              23   // Requisito de Inteligência
#define EF_REQ_DEX              24   // Requisito de Destreza
#define EF_REQ_CON              25   // Requisito de Constituição

// Efeitos adicionais
#define EF_ATTSPEED             26   // Velocidade de ataque
#define EF_RANGE                27   // Alcance de ataque
#define EF_CITIZEN              28   // Cidadania
#define EF_RUNSPEED             29   // Velocidade de movimento
#define EF_SPELL                30   // Spell que o item pode conjurar
#define EF_DURATION             31   // Duração do item (tempo de efeito)
#define EF_PARM2                32   // Parâmetro adicional 2
#define EF_GRID                 33   // Tamanho no inventário
#define EF_GROUND               34   // Tamanho no chão
#define EF_CLAN                 35   // Restrição de clan
#define EF_HWORDCOIN            36   // Valor em moeda (high word)
#define EF_LWORDCOIN            37   // Valor em moeda (low word)
#define EF_VOLATILE             38   // Item volátil (0:normal, 1:temporário, 2:desaparece, 3:?, 4:1 uso, 5:2 usos, 6:3 usos)
#define EF_KEYID                39   // ID de chave
#define EF_PARRY                40   // Chance de esquiva/bloqueio
#define EF_HITRATE              41   // Taxa de acerto
#define EF_CRITICAL             42   // Chance de crítico
#define EF_SANC                 43   // Nível de encantamento (Sanctuary)
#define EF_SAVEMANA             44   // Economia de mana (1 -> 99%)
#define EF_HPADD                45   // Bônus de HP (1 -> 101%)
#define EF_MPADD                46   // Bônus de MP (1 -> 101%)
#define EF_REGENHP              47   // Regeneração de HP
#define EF_REGENMP              48   // Regeneração de MP
#define EF_RESIST1              49   // Resistência elemental 1
#define EF_RESIST2              50   // Resistência elemental 2
#define EF_RESIST3              51   // Resistência elemental 3
#define EF_RESIST4              52   // Resistência elemental 4
#define EF_ACADD                53   // Bônus adicional de defesa
#define EF_RESISTALL            54   // Resistência a todos elementos
#define EF_BONUS                55   // Efeito bônus adicional
#define EF_HWORDGUILD           56   // ID de guild (high word) - para itens exclusivos de guild
#define EF_LWORDGUILD           57   // ID de guild (low word) - para itens exclusivos de guild
#define EF_QUEST                58   // Item de quest (valor 1)
#define EF_UNIQUE               59   // Item único
#define EF_MAGIC                60   // Dano mágico adicional (1 -> 1%)
#define EF_AMOUNT               61   // Quantidade
#define EF_HWORDINDEX           62   // Índice do item (high word)
#define EF_LWORDINDEX           63   // Índice do item (low word)
#define EF_INIT1                64   // Atributo a ser inicializado 1
#define EF_INIT2                65   // Atributo a ser inicializado 2
#define EF_INIT3                66   // Atributo a ser inicializado 3
#define EF_DAMAGEADD            67   // Dano adicional
#define EF_MAGICADD             68   // Dano mágico adicional
#define EF_HPADD2               69   // Bônus secundário de HP
#define EF_MPADD2               70   // Bônus secundário de MP
#define EF_CRITICAL2            71   // Chance de crítico secundária
#define EF_ACADD2               72   // Bônus secundário de defesa
#define EF_DAMAGE2              73   // Dano adicional secundário
#define EF_SPECIALALL           74   // Bônus para todos atributos especiais

#define EF_CURKILL              75   // Não utilizado
#define EF_LTOTKILL             76   // Não utilizado
#define EF_HTOTKILL             77   // Não utilizado
#define EF_INCUBATE             78   // Valor de incubação

// Efeitos para montarias
#define EF_MOUNTLIFE            79   // Vida da montaria
#define EF_MOUNTHP              80   // HP da montaria
#define EF_MOUNTSANC            81   // Nível de encantamento da montaria
#define EF_MOUNTFEED            82   // Alimentação da montaria
#define EF_MOUNTKILL            83   // Kills da montaria

#define EF_INCUDELAY            84   // Delay de incubação
#define EF_SUBGUILD             85   // Sub-guild (0: nenhuma, 1,2,3: nível)
#define EF_PREVBONUS            86   // Bônus de personagem anterior

#define EF_REFLEVEL             87   // Nível de refinamento (número)
#define EF_GAMEROOM            88    // Sala de jogo

#define EF_ABSDAM               89   // Absorção de dano físico
#define EF_ABSAC                90   // Absorção de defesa

// Grades de item (qualidade)
#define EF_GRADE0              100   // Grade 0
#define EF_GRADE1              101   // Grade 1
#define EF_GRADE2              102   // Grade 2
#define EF_GRADE3              103   // Grade 3
#define EF_GRADE4              104   // Grade 4
#define EF_GRADE5              105   // Grade 5

// Tempo de expiração
#define EF_DATE                106   // Data de expiração (dia)
#define EF_HOUR                107   // Data de expiração (hora)
#define EF_MIN                 108   // Data de expiração (minuto)
#define EF_YEAR                109   // Data de expiração (ano)
#define EF_MONTH               110   // Data de expiração (mês)

#define EF_NOTRADE             111   // Item não comercializável
#define EF_TRANS               112   // Transformação (0:humano, 1:animal, 2:polimorfismo, 3:transformação)

#define EF_FAME                113   // Fama

// Cores de itens
#define EF_STARTCOL            115   // Início de bloco de cores
#define EF_COLOR0              116   // Cor 0
#define EF_COLOR1              117   // Cor 1
#define EF_COLOR2              118   // Cor 2
#define EF_COLOR3              119   // Cor 3
#define EF_COLOR4              120   // Cor 4
#define EF_COLOR5              121   // Cor 5
#define EF_COLOR6              122   // Cor 6
#define EF_COLOR7              123   // Cor 7
#define EF_COLOR8              124   // Cor 8
#define EF_COLOR9              125   // Cor 9
#define EF_MAXCOL              126   // Fim de bloco de cores

// Customizações cosméticas
#define EF_STARTCOS            130   // Início de customização cosmética
#define EF_T_COS               131   // Customização superior
#define EF_F_COS               140   // Customização frontal
#define EF_B_COS               150   // Customização traseira
#define EF_H_COS               160   // Customização de cabeça
#define EF_MAXCOS              170   // Fim de customização cosmética

/**
 * @class ItemEffectManager
 * @brief Gerenciador de efeitos de itens
 * 
 * Esta classe fornece métodos para aplicar, calcular e gerenciar
 * os efeitos dos itens, mantendo compatibilidade com o WYD original
 * enquanto adiciona funcionalidades avançadas.
 */
/**
 * Classe WYDItemEffectManager
 * 
 * Esta classe implementa a funcionalidade WYDItemEffectManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ItemEffectManager {
public:
    /**
     * @brief Construtor padrão
     */
    ItemEffectManager();
    
    /**
     * @brief Destrutor
     */
    ~ItemEffectManager();
    
    /**
     * @brief Calcula o valor real de um efeito
     * 
     * @param effectType Tipo do efeito (EF_*)
     * @param effectValue Valor base do efeito
     * @param itemIndex Índice do item
     * @param itemSanc Nível de encantamento do item
     * @return int Valor calculado do efeito
     */
    int CalculateEffectValue(int effectType, int effectValue, int itemIndex, int itemSanc);
    
    /**
     * @brief Aplica os efeitos de um item a um personagem
     * 
     * @param item Item a ser aplicado
     * @param mob Personagem que receberá os efeitos
     * @param isEquiping True se está equipando, False se está desequipando
     */
    void ApplyItemEffects(const STRUCT_ITEM* item, STRUCT_MOB* mob, bool isEquiping);
    
    /**
     * @brief Verifica se um personagem atende aos requisitos para usar um item
     * 
     * @param item Item a ser verificado
     * @param mob Personagem a ser verificado
     * @return bool True se o personagem pode usar o item, False caso contrário
     */
    bool CheckItemRequirements(const STRUCT_ITEM* item, const STRUCT_MOB* mob);
    
    /**
     * @brief Adiciona um efeito a um item
     * 
     * @param item Item a ser modificado
     * @param effectType Tipo do efeito (EF_*)
     * @param effectValue Valor do efeito
     * @return bool True se o efeito foi adicionado com sucesso, False caso contrário
     */
    bool AddEffect(STRUCT_ITEM* item, int effectType, int effectValue);
    
    /**
     * @brief Remove um efeito de um item
     * 
     * @param item Item a ser modificado
     * @param effectType Tipo do efeito (EF_*)
     * @return bool True se o efeito foi removido com sucesso, False caso contrário
     */
    bool RemoveEffect(STRUCT_ITEM* item, int effectType);
    
    /**
     * @brief Obtém o valor de um efeito específico em um item
     * 
     * @param item Item a ser verificado
     * @param effectType Tipo do efeito (EF_*)
     * @return int Valor do efeito ou 0 se não encontrado
     */
    int GetEffectValue(const STRUCT_ITEM* item, int effectType);
    
    /**
     * @brief Verifica se um item possui um determinado efeito
     * 
     * @param item Item a ser verificado
     * @param effectType Tipo do efeito (EF_*)
     * @return bool True se o item possui o efeito, False caso contrário
     */
    bool HasEffect(const STRUCT_ITEM* item, int effectType);
    
    /**
     * @brief Calcula o dano adicional de um item
     * 
     * @param item Item a ser verificado
     * @return int Dano adicional
     */
    int CalculateAdditionalDamage(const STRUCT_ITEM* item);
    
    /**
     * @brief Calcula a defesa adicional de um item
     * 
     * @param item Item a ser verificado
     * @return int Defesa adicional
     */
    int CalculateAdditionalDefense(const STRUCT_ITEM* item);
    
    /**
     * @brief Calcula o bônus de HP de um item
     * 
     * @param item Item a ser verificado
     * @param baseHP HP base do personagem
     * @return int HP adicional
     */
    int CalculateAdditionalHP(const STRUCT_ITEM* item, int baseHP);
    
    /**
     * @brief Calcula o bônus de MP de um item
     * 
     * @param item Item a ser verificado
     * @param baseMP MP base do personagem
     * @return int MP adicional
     */
    int CalculateAdditionalMP(const STRUCT_ITEM* item, int baseMP);
    
    /**
     * @brief Calcula a resistência adicional de um item
     * 
     * @param item Item a ser verificado
     * @param resistType Tipo de resistência (0-3)
     * @return int Resistência adicional
     */
    int CalculateAdditionalResistance(const STRUCT_ITEM* item, int resistType);
    
    /**
     * @brief Verifica se um item está expirado
     * 
     * @param item Item a ser verificado
     * @param currentTime Tempo atual (unix timestamp)
     * @return bool True se o item está expirado, False caso contrário
     */
    bool IsItemExpired(const STRUCT_ITEM* item, time_t currentTime);
    
    /**
     * @brief Define o tempo de expiração de um item
     * 
     * @param item Item a ser modificado
     * @param expirationTime Tempo de expiração (unix timestamp)
     * @return bool True se a expiração foi definida com sucesso, False caso contrário
     */
    bool SetItemExpiration(STRUCT_ITEM* item, time_t expirationTime);
    
    /**
     * @brief Retorna o tempo de expiração de um item
     * 
     * @param item Item a ser verificado
     * @return time_t Tempo de expiração (unix timestamp) ou 0 se não expirar
     */
    time_t GetItemExpiration(const STRUCT_ITEM* item);
    
    /**
     * @brief Verifica se um item é comercializável
     * 
     * @param item Item a ser verificado
     * @return bool True se o item pode ser comercializado, False caso contrário
     */
    bool IsItemTradeable(const STRUCT_ITEM* item);
    
    /**
     * @brief Define se um item é comercializável
     * 
     * @param item Item a ser modificado
     * @param tradeable True para permitir comércio, False para proibir
     */
    void SetItemTradeable(STRUCT_ITEM* item, bool tradeable);
    
private:
    /**
     * @brief Encontra um slot livre para adicionar um efeito
     * 
     * @param item Item a ser verificado
     * @return int Índice do slot ou -1 se não houver slots livres
     */
    int FindFreeEffectSlot(const STRUCT_ITEM* item);
    
    /**
     * @brief Encontra um efeito específico em um item
     * 
     * @param item Item a ser verificado
     * @param effectType Tipo do efeito (EF_*)
     * @return int Índice do efeito ou -1 se não encontrado
     */
    int FindEffectIndex(const STRUCT_ITEM* item, int effectType);
    
    /**
     * @brief Ajusta o valor de um efeito de acordo com regras especiais
     * 
     * @param effectType Tipo do efeito
     * @param value Valor original
     * @param itemIndex Índice do item
     * @return int Valor ajustado
     */
    int AdjustEffectValue(int effectType, int value, int itemIndex);
};

#endif // _ITEMEFFECT_H_

} // namespace wydbr

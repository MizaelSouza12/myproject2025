/**
 * ItemTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/item/ItemTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef ITEMTYPES_H
#define ITEMTYPES_H

#include <vector>
#include <string>
#include <unordered_map>
#include <cstring>

#include "../../../core/WYDTypes.h"

namespace wydbr {


/**
 * @file ItemTypes.h
 * @brief Definição dos tipos de itens e estruturas relacionadas
 * 
 * Este arquivo contém as definições das estruturas de itens e constantes relacionadas,
 * seguindo exatamente a estrutura do código original do WYD, mas com melhorias
 * de design, comentários e correções de bugs conhecidos.
 */

namespace wyd {
namespace server {

// Constantes de sistema de itens (exatamente como no WYD original)
#define MAX_ITEM_EFFECT 3       // Número máximo de efeitos por item
#define MAX_EQUIP_SLOT 16       // Número de slots de equipamento
#define MAX_INVENTORY 64        // Tamanho do inventário
#define MAX_TRADE_ITEM 15       // Máximo de itens em uma janela de comércio
#define MAX_AFFECT 32           // Máximo de afetadores por personagem
#define MAX_CARGO 120           // Máximo de itens no cargo
#define MAX_STORE_ITEM 27       // Máximo de itens em uma loja de NPC

// Definições de tipos de itens (exatamente como no WYD original)
// No WYD original, esses eram defines, mas aqui usamos enum para maior segurança
/**
 * @brief Categoria principal do item
 * 
 * No WYD original: ITEM_PLACE
 */
enum ITEM_CATEGORY : BYTE {
    ITEM_PLACE_EQUIP = 0,       // Item de equipamento
    ITEM_PLACE_INVENTORY = 1,   // Item de inventário
    ITEM_PLACE_CARGO = 2,       // Item de cargo (armazenamento)
    ITEM_PLACE_TRADE = 3,       // Item em janela de comércio
    ITEM_PLACE_NPC = 4,         // Item em loja de NPC
    ITEM_PLACE_EVENT = 5,       // Item de evento (especial)
    ITEM_PLACE_GROUND = 6,      // Item no chão
    ITEM_PLACE_CREATE = 7,      // Item em criação (temporário)
    ITEM_PLACE_AUCTION = 8,     // Item em leilão
    ITEM_PLACE_MAIL = 9,        // Item em correio
};

/**
 * @brief Tipo de equipamento
 * 
 * No WYD original: ITEMTYPE
 */
enum ITEM_TYPE : BYTE {
    ITEMTYPE_NONE = 0,          // Nenhum tipo
    ITEMTYPE_WEAPON = 1,        // Arma
    ITEMTYPE_ARMOR = 2,         // Armadura
    ITEMTYPE_SHIELD = 3,        // Escudo
    ITEMTYPE_HELM = 4,          // Elmo
    ITEMTYPE_AMULET = 5,        // Amuleto
    ITEMTYPE_RING = 6,          // Anel
    ITEMTYPE_BELT = 7,          // Cinto
    ITEMTYPE_BOOTS = 8,         // Botas
    ITEMTYPE_GLOVE = 9,         // Luvas
    ITEMTYPE_INVENTORY = 10,    // Item de inventário
    ITEMTYPE_MOUNT = 11,        // Montaria
    ITEMTYPE_RELIC = 12,        // Relíquia
    ITEMTYPE_MANTLE = 13,       // Capa
    ITEMTYPE_COSTUME = 14,      // Traje
    ITEMTYPE_ARTIFACT = 15,     // Artefato
};

/**
 * @brief Slot de equipamento
 * 
 * No WYD original: EQUIPPOS
 */
enum EQUIP_POSITION : BYTE {
    EQUIPPOS_WEAPON = 0,        // Arma
    EQUIPPOS_SHIELD = 1,        // Escudo
    EQUIPPOS_HELM = 2,          // Elmo
    EQUIPPOS_ARMOR = 3,         // Armadura
    EQUIPPOS_PANTS = 4,         // Calças
    EQUIPPOS_GLOVE = 5,         // Luvas
    EQUIPPOS_BOOTS = 6,         // Botas
    EQUIPPOS_AMULET = 7,        // Amuleto
    EQUIPPOS_RING1 = 8,         // Anel 1
    EQUIPPOS_RING2 = 9,         // Anel 2
    EQUIPPOS_BELT = 10,         // Cinto
    EQUIPPOS_RELIC = 11,        // Relíquia
    EQUIPPOS_MOUNT = 12,        // Montaria
    EQUIPPOS_MANTLER = 13,      // Capa lado direito
    EQUIPPOS_MANTLEL = 14,      // Capa lado esquerdo
    EQUIPPOS_COSTUME = 15,      // Traje
};

/**
 * @brief Tipo de consumível
 * 
 * No WYD original: PotionType
 */
enum POTION_TYPE : BYTE {
    POTION_NONE = 0,            // Não é poção
    POTION_LIFE = 1,            // Poção de vida
    POTION_MANA = 2,            // Poção de mana
    POTION_STAMINA = 3,         // Poção de stamina
    POTION_RESURRECTION = 4,    // Poção de ressurreição
    POTION_ANTIDOTE = 5,        // Antídoto
    POTION_SPECIALWATER = 6,    // Água especial (para criar itens)
    POTION_VOLCANICWATER = 7,   // Água vulcânica (para criar itens)
    POTION_HOLLYWATER = 8,      // Água sagrada (para criar itens)
    POTION_DETAIL = 9,          // Detalhe (para criar itens)
    POTION_GRAIN = 10,          // Grão (para criar itens)
    POTION_BALLONDRUG = 11,     // Droga do Balão (para guerra de torre?)
    POTION_DAM_INCREASE = 12,   // Poção de aumento de dano
    POTION_MOLE_DRUG = 13,      // Droga da Toupeira (para guerra de torre?)
    POTION_EXP_INCREASE = 14,   // Poção de aumento de experiência
    POTION_TOWN_PORTAL = 15,    // Portal para cidade
    POTION_COMPLEX = 16,        // Poção complexa
    POTION_HP_REGENERATION = 17,// Poção de regeneração de HP
    POTION_MP_REGENERATION = 18,// Poção de regeneração de MP
};

/**
 * @brief Tipo de efeito de item
 * 
 * No WYD original: ITEMEFFECTTYPE
 */
enum ITEM_EFFECT_TYPE : BYTE {
    ITEM_EFFECT_NONE = 0,               // Nenhum efeito
    ITEM_EFFECT_ATTR = 1,               // Efeito em atributo (STR, INT, etc)
    ITEM_EFFECT_LEVEL = 2,              // Efeito de nível
    ITEM_EFFECT_DAMAGE = 3,             // Efeito em dano 
    ITEM_EFFECT_DEFENSE = 4,            // Efeito em defesa
    ITEM_EFFECT_BONUSDAMAGE = 5,        // Efeito em dano bônus
    ITEM_EFFECT_RESISTANCE = 6,         // Efeito em resistência
    ITEM_EFFECT_HP = 7,                 // Efeito em HP
    ITEM_EFFECT_MP = 8,                 // Efeito em MP
    ITEM_EFFECT_ACCSPEED = 9,           // Efeito em velocidade de ataque
    ITEM_EFFECT_CRITICAL = 10,          // Efeito em dano crítico
    ITEM_EFFECT_ABSORB = 11,            // Efeito em absorção de dano
    ITEM_EFFECT_HPREGEN = 12,           // Efeito em regeneração de HP
    ITEM_EFFECT_MPREGEN = 13,           // Efeito em regeneração de MP
    ITEM_EFFECT_MOVE = 14,              // Efeito em velocidade de movimento
    ITEM_EFFECT_RESISTBUFF = 15,        // Efeito em resistência a buffs negativos
    ITEM_EFFECT_IMMUNESTUN = 16,        // Efeito de imunidade a stun
    ITEM_EFFECT_ATTACKRANGE = 17,       // Efeito em alcance de ataque
    ITEM_EFFECT_SIGHT = 18,             // Efeito em visão
    ITEM_EFFECT_MOUNTSPEED = 19,        // Efeito em velocidade de montaria
    ITEM_EFFECT_DOUBLEEXP = 20,         // Efeito de experiência dobrada
    ITEM_EFFECT_SKILLSPEED = 21,        // Efeito em velocidade de skill
    ITEM_EFFECT_UNIQUE = 22,            // Efeito único (depende do id do item)
    ITEM_EFFECT_WIZARDRY = 23,          // Efeito de feitiçaria (para magos)
    ITEM_EFFECT_STEALTH = 24,           // Efeito de furtividade
    ITEM_EFFECT_PERFECTDODGE = 25,      // Efeito de esquiva perfeita
    ITEM_EFFECT_BLOCK = 26,             // Efeito de bloqueio
    ITEM_EFFECT_IGNOREDEFENSE = 27,     // Efeito de ignorar defesa
    ITEM_EFFECT_IMMUNITY = 28,          // Efeito de imunidade
    ITEM_EFFECT_ATTACKPOWER = 29,       // Efeito em poder de ataque
    ITEM_EFFECT_MAGICDAMAGE = 30,       // Efeito em dano mágico
    ITEM_EFFECT_GOLDDROP = 31,          // Efeito em drop de gold
    ITEM_EFFECT_ITEMDROP = 32,          // Efeito em drop de itens
    ITEM_EFFECT_REFLECTION = 33,        // Efeito de reflexão de dano
    ITEM_EFFECT_IGNORERESIST = 34,      // Efeito de ignorar resistência
    ITEM_EFFECT_ABSORBHP = 35,          // Efeito de absorção de HP
    ITEM_EFFECT_ABSORBMP = 36,          // Efeito de absorção de MP
    ITEM_EFFECT_IGNOREMAGICDEFENSE = 37,// Efeito de ignorar defesa mágica
    ITEM_EFFECT_KILLHP = 38,            // Efeito de recuperar HP ao matar
    ITEM_EFFECT_KILLMP = 39,            // Efeito de recuperar MP ao matar
    ITEM_EFFECT_HITMOVE = 40,           // Efeito de knockback
    ITEM_EFFECT_REDUCEDAMAGE = 41,      // Efeito de redução de dano
    ITEM_EFFECT_ITEMDROPRATE = 42,      // Efeito em taxa de drop de itens
    ITEM_EFFECT_EXPERIENCE = 43,        // Efeito em taxa de experiência
    ITEM_EFFECT_CRAFTCHANCE = 44,       // Efeito em chance de craft
    ITEM_EFFECT_MOUNTSLOT = 45,         // Efeito em slot de montaria
    ITEM_EFFECT_POTIONEFFECT = 46,      // Efeito em eficácia de poções
    ITEM_EFFECT_SUMMONTIME = 47,        // Efeito em tempo de invocação
    ITEM_EFFECT_MAXWEIGHT = 48,         // Efeito em peso máximo
    ITEM_EFFECT_POTIONTIME = 49,        // Efeito em duração de poções
    ITEM_EFFECT_MANA_COST = 50,         // Efeito em custo de mana
    ITEM_EFFECT_VAMP = 51,              // Efeito vampirismo
    ITEM_EFFECT_HEALAMOUNT = 52,        // Efeito em quantidade de cura
    ITEM_EFFECT_IGNORESTUN = 53,        // Efeito de ignorar stun
    ITEM_EFFECT_CRITICALINC = 54,       // Efeito em % de crítico
    ITEM_EFFECT_PERFORATION = 55,       // Efeito de perfuração
    ITEM_EFFECT_POTION_STR = 56,        // Efeito de poção em STR
    ITEM_EFFECT_POTION_INT = 57,        // Efeito de poção em INT
    ITEM_EFFECT_POTION_DEX = 58,        // Efeito de poção em DEX
    ITEM_EFFECT_POTION_CON = 59,        // Efeito de poção em CON
    ITEM_EFFECT_SPECIALMOVE = 60,       // Efeito de movimento especial
};

/**
 * @brief Tipo de raridade de item
 * 
 * No WYD original: ITEMGRADE
 */
enum ITEM_GRADE : BYTE {
    ITEMGRADE_NORMAL = 0,       // Normal
    ITEMGRADE_NORMAL_PLUS = 1,  // Normal+
    ITEMGRADE_RARE = 2,         // Raro
    ITEMGRADE_RARE_PLUS = 3,    // Raro+
    ITEMGRADE_UNIQUE = 4,       // Único
    ITEMGRADE_UNIQUE_PLUS = 5,  // Único+
    ITEMGRADE_LEGENDARY = 6,    // Lendário
    ITEMGRADE_LEGENDARY_PLUS = 7,// Lendário+
    ITEMGRADE_ARTIFACT = 8,     // Artefato
    ITEMGRADE_SET = 9,          // Item de conjunto
};

/**
 * @brief Tipo de requisito de item
 * 
 * No WYD original: ITEMREQTYPE
 */
enum ITEM_REQ_TYPE : BYTE {
    ITEMREQ_LEVEL = 0,          // Requisito de nível
    ITEMREQ_STR = 1,            // Requisito de STR
    ITEMREQ_INT = 2,            // Requisito de INT
    ITEMREQ_DEX = 3,            // Requisito de DEX
    ITEMREQ_CON = 4,            // Requisito de CON
    ITEMREQ_SPECIAL = 5,        // Requisito especial
    ITEMREQ_QUEST = 6,          // Requisito de missão
    ITEMREQ_GUILD = 7,          // Requisito de guilda
    ITEMREQ_CLASS = 8,          // Requisito de classe
    ITEMREQ_FAME = 9,           // Requisito de fama
    ITEMREQ_ACTIVITY = 10,      // Requisito de atividade
    ITEMREQ_TITLE = 11,         // Requisito de título
};

/**
 * @brief Classe de personagem
 * 
 * No WYD original: ITEMCLASS
 */
enum CHAR_CLASS : BYTE {
    CHARCLASS_ALL = 0,          // Todas as classes
    CHARCLASS_TK = 1,           // Transknight
    CHARCLASS_FM = 2,           // Foema
    CHARCLASS_BM = 3,           // Barbarian
    CHARCLASS_HT = 4,           // Huntress
};

/**
 * @brief Estado de um item
 * 
 * No WYD original: ITEMSTATUS
 */
enum ITEM_STATUS : BYTE {
    ITEMSTATUS_NORMAL = 0,      // Normal
    ITEMSTATUS_LOCKED = 1,      // Bloqueado
    ITEMSTATUS_EQUIPPING = 2,   // Sendo equipado
    ITEMSTATUS_REMOVING = 3,    // Sendo removido
    ITEMSTATUS_SELLING = 4,     // Sendo vendido
    ITEMSTATUS_BUYING = 5,      // Sendo comprado
    ITEMSTATUS_DROPPING = 6,    // Sendo dropado
    ITEMSTATUS_USING = 7,       // Sendo usado
    ITEMSTATUS_MOVING = 8,      // Sendo movido
    ITEMSTATUS_EXCHANGING = 9,  // Sendo trocado
    ITEMSTATUS_REFINING = 10,   // Em refinação
    ITEMSTATUS_COMPOSING = 11,  // Em composição
    ITEMSTATUS_SPLITTING = 12,  // Sendo dividido
    ITEMSTATUS_TRADING = 13,    // Em comércio
    ITEMSTATUS_MERGING = 14,    // Sendo mesclado
    ITEMSTATUS_AUCTIONING = 15, // Em leilão
    ITEMSTATUS_MAILING = 16,    // Em correio
    ITEMSTATUS_STORED = 17,     // Armazenado
};

/**
 * @brief Estrutura de efeito de item (original: ITEMEFFECT)
 * 
 * No WYD original, esta estrutura armazenava os efeitos adicionais dos itens.
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com a estrutura original
struct STRUCT_ITEM_EFFECT {
    BYTE byType;      // Tipo de efeito (ITEM_EFFECT_TYPE)
    BYTE byValue;     // Valor do efeito
    
    STRUCT_ITEM_EFFECT() 
        : byType(ITEM_EFFECT_NONE)
        , byValue(0)
    {
    }
    
    STRUCT_ITEM_EFFECT(BYTE type, BYTE value)
        : byType(type)
        , byValue(value)
    {
    }
    
    /**
     * @brief Verifica se o efeito é válido
     * @return true se o efeito for válido
     */
    bool IsValid() const {
        return byType != ITEM_EFFECT_NONE && byValue > 0;
    }
    
    /**
     * @brief Calcula o valor real do efeito
     * 
     * Alguns efeitos no WYD original tinham valores que precisavam de conversão
     * específica dependendo do tipo (porcentagem, valor absoluto, etc.).
     * 
     * @return Valor real do efeito
     */
    int GetRealValue() const {
        // No WYD original, alguns efeitos tinham escala especí/**
 * switch
 * 
 * Implementa a funcionalidade switch conforme especificação original.
 * @param byType Parâmetro byType
 * @return Retorna fica
 */
fica
        switch(byType){
            case ITEM_EFFECT_CRITICALINC:
            case ITEM_EFFECT_EXPERIENCE:
            case ITEM_EFFECT_GOLDDROP:
            case ITEM_EFFECT_ITEMDROPRATE:
                return byValue; // Direto em porcentagem
                
            case ITEM_EFFECT_CRITICAL:
            case ITEM_EFFECT_DOUBLEEXP:
            case ITEM_EFFECT_VAMP:
                return byValue * 10; // Escala de 10
                
            case ITEM_EFFECT_HP:
            case ITEM_EFFECT_MP:
            case ITEM_EFFECT_DAMAGE:
            case ITEM_EFFECT_DEFENSE:
            case ITEM_EFFECT_BONUSDAMAGE:
                return byValue * 5; // Escala de 5
                
            case ITEM_EFFECT_HEALAMOUNT:
            case ITEM_EFFECT_POTIONEFFECT:
                return byValue * 3; // Escala de 3
                
            case ITEM_EFFECT_IMMUNITY:
            case ITEM_EFFECT_IGNORESTUN:
            case ITEM_EFFECT_IMMUNESTUN:
            case ITEM_EFFECT_PERFECTDODGE:
                return byValue > 0 ? 1 : 0; // Binário (0 ou 1)
                
            default:
                return static_cast<int>(byValue);
        }
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura de item (original: STRUCT_ITEM)
 * 
 * No WYD original, esta era a estrutura central que representava
 * qualquer item no jogo.
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com a estrutura original
struct STRUCT_ITEM {
    WORD wIndex;                              // ID do item no ItemList.bin
    WORD wAmount;                             // Quantidade (para itens stackáveis)
    WORD wTemporary;                          // Flag para itens temporários e duração
    
    // Campos específicos de refinamento (gems etc.)
    BYTE byRefine;                            // Nível de refinamento
    
    // Campos estendidos de refinamento
    BYTE byCompose;                           // Resultado de composição (se refinado)
    BYTE byFacet;                             // Facetas (gemas etc.)
    BYTE byHiddenPower;                       // Poder oculto (adicional se refinado)
    BYTE byEncantIndex;                       // Índice de encantamento (Celestia)
    
    // Campos de customização e características
    BYTE byOption;                            // Opção adicional
    BYTE byLuck;                              // Flag de sorte
    BYTE byMaxDurability;                     // Durabilidade máxima (original)
    BYTE byDurability;                        // Durabilidade atual
    BYTE byItemStatus;                        // Status do item (bloqueado, etc.)
    
    // Efeitos adicionais (até 3 por item)
    STRUCT_ITEM_EFFECT stEffect[MAX_ITEM_EFFECT]; // Efeitos adicionais (até 3)
    
    STRUCT_ITEM() 
        : wIndex(0)
        , wAmount(0)
        , wTemporary(0)
        , byRefine(0)
        , byCompose(0)
        , byFacet(0)
        , byHiddenPower(0)
        , byEncantIndex(0)
        , byOption(0)
        , byLuck(0)
        , byMaxDurability(0)
        , byDurability(0)
        , byItemStatus(ITEMSTATUS_NORMAL)
    {
    }
    
    /**
     * @brief Verifica se o item está vazio
     * @return true se o item estiver vazio
     */
    bool IsEmpty() const {
        return wIndex == 0;
    }
    
    /**
     * @brief Verifica se o item é stackável (pode ser agrupado)
     * @return true se o item for stackável
     */
    bool IsStackable() const {
        // No WYD original, itens com ID < 2330 não eram stackáveis
        // ID de 2330 a 2390 são poções, pedras, etc (stackáveis)
        // Aqui corrigimos para usar uma função em vez de hardcoded
        // Obs: implementação completa requer acesso a dados do ItemList.bin
        return (wIndex >= 2330 && wIndex <= 3000) || (wIndex >= 5000 && wIndex <= 7000);
    }
    
    /**
     * @brief Verifica se o item está bloqueado
     * @return true se o item estiver bloqueado
     */
    bool IsLocked() const {
        return (byItemStatus & ITEMSTATUS_LOCKED) != 0;
    }
    
    /**
     * @brief Bloqueia ou desbloqueia o item
     * @param locked true para bloquear, false para desbloquear
     */
    void SetLocked(bool locked) {
        if (locked) {
            byItemStatus |= ITEMSTATUS_LOCKED;
        } else {
            byItemStatus &= ~ITEMSTATUS_LOCKED;
        }
    }
    
    /**
     * @brief Verifica se o item é refinável
     * @return true se o item for refinável
     */
    bool IsRefinable() const {
        // No WYD original, apenas equipamentos eram refináveis
        // Aqui expandimos a verificação para ser mais precisa
        return wIndex > 0 && wIndex < 2330 && wIndex != 747 && wIndex != 3980 && wIndex != 3981 && !IsEmpty();
    }
    
    /**
     * @brief Obtém o nível de refinamento do item
     * @return Nível de refinamento
     */
    BYTE GetRefineLevel() const {
        return byRefine;
    }
    
    /**
     * @brief Define o nível de refinamento do item
     * @param level Nível de refinamento
     * @return true se definido com sucesso
     */
    bool SetRefineLevel(BYTE level) {
        if (!IsRefinable() || level > 15) { // Máximo de +15 no WYD original
            return false;
        }
        
        byRefine = level;
        return true;
    }
    
    /**
     * @brief Verifica se o item possui sorte
     * @return true se o item possuir sorte
     */
    bool HasLuck() const {
        return byLuck != 0;
    }
    
    /**
     * @brief Define o atributo de sorte do item
     * @param hasLuck true para ter sorte, false caso contrário
     */
    void SetLuck(bool hasLuck) {
        byLuck = hasLuck ? 1 : 0;
    }
    
    /**
     * @brief Obtém o valor do atributo adicional
     * @return Valor do atributo adicional
     */
    BYTE GetAdditionalValue() const {
        return byOption;
    }
    
    /**
     * @brief Define o valor do atributo adicional
     * @param value Valor do atributo adicional
     */
    void SetAdditionalValue(BYTE value) {
        byOption = value;
    }
    
    /**
     * @brief Verifica se o item está danificado
     * @return true se o item estiver danificado
     */
    bool IsDamaged() const {
        return byDurability < byMaxDurability;
    }
    
    /**
     * @brief Verifica se o item está quebrado (durabilidade 0)
     * @return true se o item estiver quebrado
     */
    bool IsBroken() const {
        return byDurability == 0 && byMaxDurability > 0;
    }
    
    /**
     * @brief Repara o item para durabilidade máxima
     */
    void Repair() {
        byDurability = byMaxDurability;
    }
    
    /**
     * @brief Reduz a durabilidade do item
     * @param amount Quantidade a reduzir
     * @return true se a durabilidade foi reduzida, false se já estava 0
     */
    bool ReduceDurability(BYTE amount = 1) {
        if (byDurability == 0) {
            return false;
        }
        
        // Evita underflow (bug no WYD original)
        if (amount > byDurability) {
            byDurability = 0;
        } else {
            byDurability -= amount;
        }
        
        return true;
    }
    
    /**
     * @brief Define um efeito para o item
     * @param slot Slot do efeito (0-2)
     * @param type Tipo do efeito
     * @param value Valor do efeito
     * @return true se definido com sucesso
     */
    bool SetEffect(BYTE slot, BYTE type, BYTE value) {
        if (slot >= MAX_ITEM_EFFECT) {
            return false;
        }
        
        stEffect[slot].byType = type;
        stEffect[slot].byValue = value;
        
        return true;
    }
    
    /**
     * @brief Verifica se o item tem um efeito específico
     * @param type Tipo do efeito
     * @return true se o item tiver o efeito
     */
    bool HasEffect(BYTE type) const {
        for (int i = 0; i < MAX_ITEM_EFFECT; i++) {
            if (stEffect[i].byType == type && stEffect[i].byValue > 0) {
                return true;
            }
        }
        
        return false;
    }
    
    /**
     * @brief Obtém o valor de um efeito específico
     * @param type Tipo do efeito
     * @return Valor do efeito, ou 0 se não tiver
     */
    BYTE GetEffectValue(BYTE type) const {
        for (int i = 0; i < MAX_ITEM_EFFECT; i++) {
            if (stEffect[i].byType == type) {
                return stEffect[i].byValue;
            }
        }
        
        return 0;
    }
    
    /**
     * @brief Compara este item com outro
     * @param other Outro item
     * @return 0 se iguais, -1 se este < outro, 1 se este > outro
     */
    int CompareTo(const STRUCT_ITEM& other) const {
        // Se algum é vazio, o não vazio é maior
        if (IsEmpty() && !other.IsEmpty()) {
            return -1;
        }
        
        if (!IsEmpty() && other.IsEmpty()) {
            return 1;
        }
        
        // Se ambos são vazios, são iguais
        if (IsEmpty() && other.IsEmpty()) {
            return 0;
        }
        
        // Compara por /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param other.wIndex Parâmetro other.wIndex
 * @return Retorna ID
 */
 ID
        if(wIndex < other.wIndex){
            return -1;
        }
        
        if (wIndex > other.wIndex) {
            return 1;
        }
        
        // Mesmo ID, compara /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param other.byRefine Parâmetro other.byRefine
 * @return Retorna refinamento
 */
 refinamento
        if(byRefine < other.byRefine){
            return -1;
        }
        
        if (byRefine > other.byRefine) {
            return 1;
        }
        
        // Mesmo refinamento, compara /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param other.byLuck Parâmetro other.byLuck
 * @return Retorna sorte
 */
 sorte
        if(byLuck < other.byLuck){
            return -1;
        }
        
        if (byLuck > other.byLuck) {
            return 1;
        }
        
        // Mesma sorte, compara opção /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param other.byOption Parâmetro other.byOption
 * @return Retorna adicional
 */
 adicional
        if(byOption < other.byOption){
            return -1;
        }
        
        if (byOption > other.byOption) {
            return 1;
        }
        
        // Considera igual para comparação básica
        return 0;
    }
    
    /**
     * @brief Copia dados de outro item
     * @param other Outro item
     */
    void CopyFrom(const STRUCT_ITEM& other) {
        wIndex = other.wIndex;
        wAmount = other.wAmount;
        wTemporary = other.wTemporary;
        byRefine = other.byRefine;
        byCompose = other.byCompose;
        byFacet = other.byFacet;
        byHiddenPower = other.byHiddenPower;
        byEncantIndex = other.byEncantIndex;
        byOption = other.byOption;
        byLuck = other.byLuck;
        byMaxDurability = other.byMaxDurability;
        byDurability = other.byDurability;
        byItemStatus = other.byItemStatus;
        
        // Copia os /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna efeitos
 */
 efeitos
        for(int i = 0; i < MAX_ITEM_EFFECT; i++){
            stEffect[i] = other.stEffect[i];
        }
    }
    
    /**
     * @brief Limpa o item (torna vazio)
     */
    void Clear() {
        wIndex = 0;
        wAmount = 0;
        wTemporary = 0;
        byRefine = 0;
        byCompose = 0;
        byFacet = 0;
        byHiddenPower = 0;
        byEncantIndex = 0;
        byOption = 0;
        byLuck = 0;
        byMaxDurability = 0;
        byDurability = 0;
        byItemStatus = ITEMSTATUS_NORMAL;
        
        // Limpa os /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna efeitos
 */
 efeitos
        for(int i = 0; i < MAX_ITEM_EFFECT; i++){
            stEffect[i].byType = ITEM_EFFECT_NONE;
            stEffect[i].byValue = 0;
        }
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura de item base (definição de ItemList.bin)
 * 
 * No WYD original, esta estrutura definia os dados base dos itens
 * carregados do arquivo ItemList.bin.
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com a estrutura original
struct STRUCT_ITEMLIST {
    char     szName[64];        // Nome do item
    char     szDescription[128]; // Descrição do item
    
    // Informações básicas
    WORD     wIndex;            // ID do item
    WORD     wPrice;            // Preço base
    WORD     wSellPrice;        // Preço de venda para NPC
    
    // Tipo e categoria
    BYTE     byType;            // Tipo de item (ITEM_TYPE)
    BYTE     bySubType;         // Subtipo
    BYTE     byImportance;      // Importância (0: comum, 1: incomum, 2: raro, etc)
    BYTE     byGrade;           // Raridade (ITEM_GRADE)
    
    // Requisitos para uso
    BYTE     byReqLevel;        // Nível requerido
    BYTE     byReqStr;          // STR requerido
    BYTE     byReqInt;          // INT requerido
    BYTE     byReqDex;          // DEX requerido
    BYTE     byReqCon;          // CON requerido
    
    // Limitações
    BYTE     byLevel;           // Nível do item
    BYTE     byLevel2;          // Nível secundário (para certos cálculos)
    BYTE     bySet;             // Conjunto a que pertence (0: nenhum)
    BYTE     byReqClass;        // Classe requerida (CHAR_CLASS)
    
    // Slots e uso
    BYTE     bySlot;            // Slot onde equipa
    BYTE     byUse;             // Tipo de uso (0: não usável, etc)
    
    // Valores base
    WORD     wAttackMin;        // Dano mínimo
    WORD     wAttackMax;        // Dano máximo
    WORD     wRange;            // Alcance
    WORD     wDamage;           // Dano base (para outras fórmulas)
    WORD     wMagicDamage;      // Dano mágico
    WORD     wAttackSpeed;      // Velocidade de ataque
    WORD     wDefense;          // Defesa
    WORD     wMagicDefense;     // Defesa mágica
    WORD     wHpIncrease;       // Aumento de HP
    WORD     wMpIncrease;       // Aumento de MP
    
    // Resistências
    BYTE     byResist[4];       // Resistências elementais [0-3]
    
    // Duração e uso
    BYTE     byDuration;        // Duração base
    BYTE     byDurability;      // Durabilidade padrão
    
    // Informações específicas de consumíveis
    BYTE     byPotionType;      // Tipo de poção (POTION_TYPE)
    BYTE     byPotionValue;     // Valor de poção
    
    // Atributos base
    WORD     wStr;              // Bônus de STR
    WORD     wInt;              // Bônus de INT
    WORD     wDex;              // Bônus de DEX
    WORD     wCon;              // Bônus de CON
    
    // Atributos avançados
    BYTE     bySpecial[8];      // Atributos especiais diversos
    
    // Flags e comportamento
    BYTE     byItemTradeble;    // Se pode ser trocado (0: não, 1: sim)
    BYTE     byItemThrowable;   // Se pode ser dropado (0: não, 1: sim)
    BYTE     byItemVisible;     // Se aparece no inventário (0: não, 1: sim)
    BYTE     byItemDropRate;    // Taxa de drop /**
 * STRUCT_ITEMLIST
 * 
 * Implementa a funcionalidade STRUCT_ITEMLIST conforme especificação original.
 * @return Retorna base
 */
 base
    
    STRUCT_ITEMLIST(){
        memset(this, 0, sizeof(STRUCT_ITEMLIST));
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura de inventário (original: STRUCT_INVENTORY)
 * 
 * No WYD original, esta estrutura representava o inventário de um personagem.
 * Possuía bugs conhecidos relacionados a overflow.
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com a estrutura original
struct STRUCT_INVENTORY {
    STRUCT_ITEM Items[MAX_INVENTORY]; // Itens no inventário
    DWORD Gold;                        // Quantidade de gold
    
    STRUCT_INVENTORY() 
        /**
 * Gold
 * 
 * Implementa a funcionalidade Gold conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna :
 */
 
        : Gold(0){
        // Inicializa todos os slots como /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna vazios
 */
 vazios
        for(int i = 0; i < MAX_INVENTORY; i++){
            Items[i].Clear();
        }
    }
    
    /**
     * @brief Obtém um item pelo slot
     * @param slot Slot do item (0-63)
     * @return Referência para o item, ou nullptr se slot inválido
     */
    STRUCT_ITEM* GetItem(int slot) {
        if (slot < 0 || slot >= MAX_INVENTORY) {
            return nullptr;
        }
        
        return &Items[slot];
    }
    
    /**
     * @brief Obtém um item pelo slot (const)
     * @param slot Slot do item (0-63)
     * @return Referência const para o item, ou nullptr se slot inválido
     */
    const STRUCT_ITEM* GetItem(int slot) const {
        if (slot < 0 || slot >= MAX_INVENTORY) {
            return nullptr;
        }
        
        return &Items[slot];
    }
    
    /**
     * @brief Verifica se um slot está vazio
     * @param slot Slot a verificar
     * @return true se o slot estiver vazio
     */
    bool IsSlotEmpty(int slot) const {
        const STRUCT_ITEM* item = GetItem(slot);
        return item == nullptr || item->IsEmpty();
    }
    
    /**
     * @brief Procura um item pelo ID
     * @param itemId ID do item
     * @return Slot do item, ou -1 se não encontrado
     */
    int FindItemById(WORD itemId) const {
        for (int i = 0; i < MAX_INVENTORY; i++) {
            if (Items[i].wIndex == itemId) {
                return i;
            }
        }
        
        return -1;
    }
    
    /**
     * @brief Conta quantos itens de um ID específico existem
     * @param itemId ID do item
     * @return Quantidade total
     */
    int CountItemsById(WORD itemId) const {
        int count = 0;
        
        for (int i = 0; i < MAX_INVENTORY; i++) {
            if (Items[i].wIndex == itemId) {
                count += Items[i].wAmount;
            }
        }
        
        return count;
    }
    
    /**
     * @brief Procura um slot vazio
     * @return Slot vazio, ou -1 se não houver
     */
    int FindEmptySlot() const {
        for (int i = 0; i < MAX_INVENTORY; i++) {
            if (Items[i].IsEmpty()) {
                return i;
            }
        }
        
        return -1;
    }
    
    /**
     * @brief Adiciona gold ao inventário
     * @param amount Quantidade a adicionar
     * @return true se adicionado com sucesso
     */
    bool AddGold(DWORD amount) {
        // Verifica overflow (bug do WYD original corrigido)
        if (amount > 0 && (Gold + amount) < Gold) {
            Gold = 2000000000; // Limite de 2 bilhões (cap)
            return false;
        }
        
        Gold += amount;
        return true;
    }
    
    /**
     * @brief Remove gold do inventário
     * @param amount Quantidade a remover
     * @return true se removido com sucesso
     */
    bool RemoveGold(DWORD amount) {
        if (amount > Gold) {
            return false;
        }
        
        Gold -= amount;
        return true;
    }
    
    /**
     * @brief Adiciona um item ao inventário
     * @param item Item a adicionar
     * @param autoStack Auto-agrupar com itens similares se stackável
     * @return Slot onde foi adicionado, ou -1 se falhou
     */
    int AddItem(const STRUCT_ITEM& item, bool autoStack = true) {
        // Verifica se o item é válido
        if (item.IsEmpty()) {
            return -1;
        }
        
        // Se for stackável e autoStack estiver habilitado
        if (item.IsStackable() && autoStack) {
            // Procura por um item igual para /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna stack
 */
 stack
            for(int i = 0; i < MAX_INVENTORY; i++){
                if (Items[i].wIndex == item.wIndex && Items[i].wAmount < 999) {
                    // No WYD original, havia um bug de overflow aqui
                    // Corrigido para verificar se excede o limite de stack
                    WORD newAmount = Items[i].wAmount + item.wAmount;
                    if (newAmount > 999) {
                        // Limite excedido - fill
                        WORD remaining = newAmount - 999;
                        Items[i].wAmount = 999;
                        
                        // Tenta adicionar o restante em outro slot
                        STRUCT_ITEM remainingItem = item;
                        remainingItem.wAmount = remaining;
                        /**
 * AddItem
 * 
 * Implementa a funcionalidade AddItem conforme especificação original.
 * @param remainingItem Parâmetro remainingItem
 * @param true Parâmetro true
 * @return Retorna return
 */

                        return AddItem(remainingItem, true);
                    } else {
                        // Cabe no stack
                        Items[i].wAmount = newAmount;
                        return i;
                    }
                }
            }
        }
        
        // Não stackou, procura slot vazio
        int emptySlot = FindEmptySlot();
        if (emptySlot == -1) {
            return -1; // Inventário cheio
        }
        
        // Adiciona o item no slot vazio
        Items[emptySlot].CopyFrom(item);
        return emptySlot;
    }
    
    /**
     * @brief Remove um item do inventário
     * @param slot Slot do item
     * @param amount Quantidade a remover (para stackáveis)
     * @return true se removido com sucesso
     */
    bool RemoveItem(int slot, WORD amount = 0) {
        if (slot < 0 || slot >= MAX_INVENTORY) {
            return false;
        }
        
        // Se o slot estiver vazio
        if (Items[slot].IsEmpty()) {
            return false;
        }
        
        // Se for stackável e a quantidade for especificada
        if (Items[slot].IsStackable() && amount > 0) {
            if (amount >= Items[slot].wAmount) {
                // Remove o item inteiro
                Items[slot].Clear();
            } else {
                // Remove apenas parte da quantidade
                Items[slot].wAmount -= amount;
            }
        } else {
            // Remove o item inteiro
            Items[slot].Clear();
        }
        
        return true;
    }
    
    /**
     * @brief Move um item dentro do inventário
     * @param srcSlot Slot de origem
     * @param dstSlot Slot de destino
     * @param amount Quantidade a mover (para stackáveis)
     * @return true se movido com sucesso
     */
    bool MoveItem(int srcSlot, int dstSlot, WORD amount = 0) {
        // Verifica limites de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param dstSlot Parâmetro dstSlot
 * @return Retorna slots
 */
 slots
        if(srcSlot < 0 || srcSlot >= MAX_INVENTORY || 
            dstSlot < 0 || dstSlot >= MAX_INVENTORY || 
            srcSlot == dstSlot){
            return false;
        }
        
        // Verifica se o slot de origem tem item
        if (Items[srcSlot].IsEmpty()) {
            return false;
        }
        
        // Se for stackável e a quantidade for especificada
        if (Items[srcSlot].IsStackable() && amount > 0 && amount < Items[srcSlot].wAmount) {
            // Movimento parcial (split stack)
            
            // Se o destino estiver vazio
            if (Items[dstSlot].IsEmpty()) {
                // Cria um novo stack no destino
                Items[dstSlot].CopyFrom(Items[srcSlot]);
                Items[dstSlot].wAmount = amount;
                Items[srcSlot].wAmount -= amount;
                return true;
            }
            
            // Se o destino tiver o mesmo /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param ItemssrcSlot.wIndex Parâmetro ItemssrcSlot.wIndex
 * @return Retorna item
 */
 item
            if(Items[dstSlot].wIndex == Items[srcSlot].wIndex){
                // Stack com outro item igual
                
                // Verifica se excede o limite de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 999 Parâmetro 999
 * @return Retorna stack
 */
 stack
                if(Items[dstSlot].wAmount + amount > 999){
                    // Move apenas o que cabe
                    WORD moveAmount = 999 - Items[dstSlot].wAmount;
                    Items[dstSlot].wAmount = 999;
                    Items[srcSlot].wAmount -= moveAmount;
                    return true;
                } else {
                    // Move toda a quantidade
                    Items[dstSlot].wAmount += amount;
                    Items[srcSlot].wAmount -= amount;
                    
                    // Se o source ficou vazio, /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna limpa
 */
 limpa
                    if(Items[srcSlot].wAmount == 0){
                        Items[srcSlot].Clear();
                    }
                    
                    return true;
                }
            }
            
            // Destino tem item diferente, não pode mover parcialmente
            return false;
        }
        
        // Movimento total (todo o item)
        
        // Se o destino estiver vazio
        if (Items[dstSlot].IsEmpty()) {
            // Move o item inteiro
            Items[dstSlot].CopyFrom(Items[srcSlot]);
            Items[srcSlot].Clear();
            return true;
        }
        
        // Se ambos forem do mesmo tipo e stackáveis
        if (Items[srcSlot].wIndex == Items[dstSlot].wIndex && Items[srcSlot].IsStackable()) {
            // Verifica se excede o limite de stack
            WORD newAmount = Items[dstSlot].wAmount + Items[srcSlot].wAmount;
            if (newAmount > 999) {
                // Move apenas o que cabe
                Items[srcSlot].wAmount = newAmount - 999;
                Items[dstSlot].wAmount = 999;
                return true;
            } else {
                // Move toda a quantidade
                Items[dstSlot].wAmount = newAmount;
                Items[srcSlot].Clear();
                return true;
            }
        }
        
        // Tipos diferentes, troca os itens
        STRUCT_ITEM temp;
        temp.CopyFrom(Items[dstSlot]);
        Items[dstSlot].CopyFrom(Items[srcSlot]);
        Items[srcSlot].CopyFrom(temp);
        
        return true;
    }
    
    /**
     * @brief Verifica se o inventário está cheio
     * @return true se o inventário estiver cheio
     */
    bool IsFull() const {
        return FindEmptySlot() == -1;
    }
    
    /**
     * @brief Conta quantos slots estão vazios
     * @return Número de slots vazios
     */
    int CountEmptySlots() const {
        int count = 0;
        
        for (int i = 0; i < MAX_INVENTORY; i++) {
            if (Items[i].IsEmpty()) {
                count++;
            }
        }
        
        return count;
    }
    
    /**
     * @brief Limpa todos os itens do inventário
     */
    void Clear() {
        for (int i = 0; i < MAX_INVENTORY; i++) {
            Items[i].Clear();
        }
        
        Gold = 0;
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura de equipamento (original: STRUCT_AFFECT)
 * 
 * No WYD original, esta estrutura representava efeitos temporários
 * aplicados a um personagem.
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com a estrutura original
struct STRUCT_AFFECT {
    BYTE byType;        // Tipo de efeito
    BYTE byValue;       // Valor do efeito
    DWORD dwTime;       // Tempo restante (em ms)
    
    STRUCT_AFFECT() 
        : byType(0)
        , byValue(0)
        , dwTime(0)
    {
    }
    
    STRUCT_AFFECT(BYTE type, BYTE value, DWORD time)
        : byType(type)
        , byValue(value)
        , dwTime(time)
    {
    }
    
    /**
     * @brief Verifica se o efeito é válido
     * @return true se o efeito for válido
     */
    bool IsValid() const {
        return byType != 0 && dwTime > 0;
    }
    
    /**
     * @brief Atualiza o tempo restante
     * @param elapsed Tempo decorrido (em ms)
     * @return true se o efeito ainda estiver ativo
     */
    bool Update(DWORD elapsed) {
        // Evita underflow (corrige bug do WYD original)
        if (elapsed >= dwTime) {
            dwTime = 0;
            return false;
        }
        
        dwTime -= elapsed;
        return dwTime > 0;
    }
    
    /**
     * @brief Obtém o tempo restante em segundos
     * @return Tempo restante em segundos
     */
    int GetRemainingSeconds() const {
        return static_cast<int>(dwTime / 1000);
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura de mercado (original: STRUCT_ITEM_SHOP)
 * 
 * No WYD original, esta estrutura representava um item em uma loja de NPC.
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com a estrutura original
struct STRUCT_ITEMSHOP {
    STRUCT_ITEM stItem;         // Item
    DWORD dwPrice;              // Preço
    BYTE byStatus;              // Status (0: disponível, 1: vendido)
    BYTE byIndex;               // Índice na loja
    
    STRUCT_ITEMSHOP() 
        : dwPrice(0)
        , byStatus(0)
        , byIndex(0)
    {
    }
    
    /**
     * @brief Verifica se o item está disponível
     * @return true se o item estiver disponível
     */
    bool IsAvailable() const {
        return byStatus == 0 && !stItem.IsEmpty();
    }
};
#pragma pack(pop)

/**
 * @brief Gerenciador de itens (não existia no WYD original)
 * 
 * Esta classe foi adicionada para facilitar o gerenciamento de itens,
 * implementando funcionalidades que no WYD original estavam espalhadas.
 */
/**
 * Classe WYDItemManager
 * 
 * Esta classe implementa a funcionalidade WYDItemManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CItemManager {
public:
    /**
     * @brief Obtém a instância do singleton
     * @return Referência para a instância
     */
    static CItemManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador de itens
     * @param itemListPath Caminho para o arquivo ItemList.bin
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool Initialize(const char* itemListPath = "Data/ItemList.bin");
    
    /**
     * @brief Finaliza o gerenciador de itens
     */
    void Shutdown();
    
    /**
     * @brief Obtém informações base de um item pelo ID
     * @param itemId ID do item
     * @return Ponteiro para as informações do item, ou nullptr se não encontrado
     */
    const STRUCT_ITEMLIST* GetItemInfo(WORD itemId) const;
    
    /**
     * @brief Obtém o nome de um item pelo ID
     * @param itemId ID do item
     * @return Nome do item, ou string vazia se não encontrado
     */
    std::string GetItemName(WORD itemId) const;
    
    /**
     * @brief Cria um item com base em seu ID
     * @param itemId ID do item
     * @param amount Quantidade (para stackáveis)
     * @param refine Nível de refinamento
     * @param hasLuck Se tem sorte
     * @param additionalValue Valor adicional
     * @return Item criado
     */
    STRUCT_ITEM CreateItem(WORD itemId, WORD amount = 1, BYTE refine = 0, 
                          bool hasLuck = false, BYTE additionalValue = 0);
    
    /**
     * @brief Obtém o preço de um item
     * @param item Item
     * @param isSelling true se estiver vendendo, false se estiver comprando
     * @return Preço do item
     */
    DWORD GetItemPrice(const STRUCT_ITEM& item, bool isSelling = false) const;
    
    /**
     * @brief Verifica se um item pode ser equipado por uma classe
     * @param itemId ID do item
     * @param charClass Classe do personagem
     * @return true se o item puder ser equipado
     */
    bool CanEquipItem(WORD itemId, BYTE charClass) const;
    
    /**
     * @brief Verifica se um item atende aos requisitos
     * @param itemId ID do item
     * @param level Nível do personagem
     * @param str STR do personagem
     * @param intl INT do personagem
     * @param dex DEX do personagem
     * @param con CON do personagem
     * @return true se o item atender aos requisitos
     */
    bool MeetsRequirements(WORD itemId, BYTE level, BYTE str, BYTE intl, BYTE dex, BYTE con) const;
    
    /**
     * @brief Cria uma definição de item
     * @param itemList Definição de item
     */
    void RegisterItemDefinition(const STRUCT_ITEMLIST& itemList);
    
    /**
     * @brief Converte um item para string (para depuração)
     * @param item Item
     * @return String representando o item
     */
    std::string ItemToString(const STRUCT_ITEM& item) const;
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    CItemManager();
    
    /**
     * @brief Destrutor
     */
    ~CItemManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    CItemManager(const CItemManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    CItemManager& operator=(const CItemManager&) = delete;
    
    /**
     * @brief Carrega definições de itens do arquivo ItemList.bin
     * @param filePath Caminho para o arquivo
     * @return true se carregado com sucesso, false caso contrário
     */
    bool LoadItemDefinitions(const char* filePath);
    
    /**
     * @brief Calcula a durabilidade máxima para um item
     * @param itemId ID do item
     * @return Durabilidade máxima
     */
    BYTE CalculateMaxDurability(WORD itemId) const;
    
    /**
     * @brief Calcula o bônus de atributo para um item refinado
     * @param baseValue Valor base
     * @param refineLevel Nível de refinamento
     * @return Valor com bônus
     */
    WORD CalculateRefinedValue(WORD baseValue, BYTE refineLevel) const;
    
    bool initialized_;                                 // Se o gerenciador está inicializado
    std::unordered_map<WORD, STRUCT_ITEMLIST> items_; // Mapa de ID para definição de item
};

// Definição global para facilitar o acesso
#define g_ItemManager CItemManager::GetInstance()

} // namespace server
} // namespace wyd

#endif // ITEMTYPES_H

} // namespace wydbr

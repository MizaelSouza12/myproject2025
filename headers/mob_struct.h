/**
 * @file mob_struct.h
 * @brief Definição das estruturas de mobs/personagens do WYD
 * 
 * Este arquivo contém as declarações completas para mobs e personagens
 * no sistema WYDBR, com correções de segurança e melhorias.
 */

#ifndef WYDBR_MOB_STRUCT_H
#define WYDBR_MOB_STRUCT_H

#include <cstdint>
#include <string>
#include <vector>
#include "wyd_core.h"
#include "item_struct.h"

namespace WYDBR {

// Tipo de entidade
enum class EntityType : uint8_t {
    Player = 0,
    NPC = 1,
    Monster = 2,
    Pet = 3,
    Mount = 4,
    Object = 5
};

// Tipo de ataque
enum class AttackType : uint8_t {
    Melee = 0,
    Ranged = 1,
    Magic = 2,
    Special = 3
};

// Tipo de dano
enum class DamageType : uint8_t {
    Physical = 0,
    Fire = 1,
    Ice = 2,
    Lightning = 3,
    Poison = 4,
    Holy = 5,
    Dark = 6
};

// Informações de skill
struct SkillInfo {
    uint16_t skillId = 0;
    uint8_t level = 0;
    uint16_t mpCost = 0;
    uint16_t cooldown = 0;
    uint32_t lastUseTime = 0;
    
    // Verificação de segurança
    bool CanUse(uint32_t currentTime, uint16_t currentMp) const {
        // Verificar cooldown
        if (currentTime - lastUseTime < cooldown) {
            return false;
        }
        
        // Verificar MP
        if (currentMp < mpCost) {
            return false;
        }
        
        return true;
    }
};

// Status de efeito ativo
struct AffectInfo {
    uint16_t effectId = 0;
    int16_t value = 0;
    uint32_t duration = 0;
    uint32_t startTime = 0;
    
    // Verificação de segurança
    bool IsActive(uint32_t currentTime) const {
        if (effectId == 0) {
            return false;
        }
        
        return currentTime - startTime < duration;
    }
};

// Status de atributos
struct Stats {
    uint16_t strength = 0;
    uint16_t dexterity = 0;
    uint16_t intelligence = 0;
    uint16_t constitution = 0;
    
    // Status derivados
    uint16_t accuracy = 0;
    uint16_t evasion = 0;
    uint16_t criticalRate = 0;
    uint16_t criticalDamage = 0;
    
    // Resistências
    uint8_t resistPhysical = 0;
    uint8_t resistFire = 0;
    uint8_t resistIce = 0;
    uint8_t resistLightning = 0;
    uint8_t resistPoison = 0;
    uint8_t resistHoly = 0;
    uint8_t resistDark = 0;
    
    // Calcular ataque base
    uint16_t CalculateBaseAttack() const {
        // Fórmula melhorada para calcular ataque base com base em atributos
        return strength * 2 + dexterity / 2;
    }
    
    // Calcular defesa base
    uint16_t CalculateBaseDefense() const {
        // Fórmula melhorada para calcular defesa base com base em atributos
        return constitution * 2 + strength / 2;
    }
    
    // Calcular valor de resistência
    uint8_t GetResistance(DamageType type) const {
        switch (type) {
            case DamageType::Physical: return resistPhysical;
            case DamageType::Fire: return resistFire;
            case DamageType::Ice: return resistIce;
            case DamageType::Lightning: return resistLightning;
            case DamageType::Poison: return resistPoison;
            case DamageType::Holy: return resistHoly;
            case DamageType::Dark: return resistDark;
            default: return 0;
        }
    }
};

// Estrutura para mobs
struct Mob : public EntityBase {
    char name[16] = {0};  // Nome com terminador nulo garantido
    EntityType entityType = EntityType::Monster;
    CharacterClass characterClass = CharacterClass::TK;
    uint8_t level = 1;
    
    // Atributos
    Stats stats;
    
    // Equipamentos
    Item equips[MAX_EQUIP];
    bool equippedSlots[MAX_EQUIP] = {false};
    
    // Inventário
    Inventory inventory;
    
    // Skills
    SkillInfo skills[MAX_SKILL];
    
    // Efeitos ativos
    AffectInfo affects[MAX_AFFECT];
    
    // Informações adicionais para jogadores
    struct PlayerInfo {
        uint32_t experience = 0;
        uint32_t nextLevelExp = 1000;
        uint32_t gold = 0;
        uint32_t karma = 0;
        uint8_t guildId = 0;
        uint8_t partyId = 0;
        uint32_t lastLogin = 0;
        uint32_t lastLogout = 0;
        uint32_t playTime = 0;
        uint8_t adminLevel = 0;
        
        // Flags de segurança (correções)
        bool isMuted = false;
        bool isStuck = false;
        bool isTrading = false;
        bool isOnline = false;
    } playerInfo;
    
    // Obter equipamento com verificação de segurança
    const Item* GetEquip(int slot) const {
        if (slot < 0 || slot >= MAX_EQUIP || !equippedSlots[slot]) {
            return nullptr;
        }
        return &equips[slot];
    }
    
    // Equipar item com verificação de segurança
    bool EquipItem(int slot, const Item& item) {
        if (slot < 0 || slot >= MAX_EQUIP || equippedSlots[slot]) {
            return false;
        }
        
        // Verificar requisitos do item
        if (stats.strength < item.reqStr ||
            stats.dexterity < item.reqDex ||
            stats.intelligence < item.reqInt ||
            stats.constitution < item.reqCon) {
            return false;
        }
        
        equips[slot] = item;
        equippedSlots[slot] = true;
        
        // Atualizar estatísticas
        UpdateStats();
        
        return true;
    }
    
    // Remover equipamento com verificação de segurança
    bool UnequipItem(int slot) {
        if (slot < 0 || slot >= MAX_EQUIP || !equippedSlots[slot]) {
            return false;
        }
        
        // Verificar se há espaço no inventário
        if (inventory.CountOccupied() >= Inventory::MAX_ITEMS) {
            return false;
        }
        
        // Adicionar ao inventário
        bool added = inventory.AddItem(equips[slot]);
        if (!added) {
            return false;
        }
        
        // Limpar slot do equipamento
        equips[slot] = Item();
        equippedSlots[slot] = false;
        
        // Atualizar estatísticas
        UpdateStats();
        
        return true;
    }
    
    // Atualizar estatísticas com base nos equipamentos
    void UpdateStats() {
        // Resetar bônus
        stats.accuracy = stats.dexterity / 2;
        stats.evasion = stats.dexterity / 4;
        stats.criticalRate = 5; // Base 5%
        stats.criticalDamage = 150; // Base 150%
        
        // Adicionar bônus dos equipamentos
        for (int i = 0; i < MAX_EQUIP; ++i) {
            if (equippedSlots[i]) {
                const Item& item = equips[i];
                stats.strength += item.bonusStr;
                stats.dexterity += item.bonusDex;
                stats.intelligence += item.bonusInt;
                stats.constitution += item.bonusCon;
                
                maxHp += item.bonusHP;
                maxMp += item.bonusMP;
            }
        }
        
        // Recalcular valores derivados
        maxHp = 100 + (level * 10) + (stats.constitution * 10);
        maxMp = 100 + (level * 5) + (stats.intelligence * 8);
        
        // Limitar HP/MP atual aos novos máximos
        if (hp > maxHp) hp = maxHp;
        if (mp > maxMp) mp = maxMp;
    }
    
    // Lidar com dano recebido (segurança)
    bool TakeDamage(uint16_t damage, DamageType type) {
        // Aplicar resistência
        uint8_t resistance = stats.GetResistance(type);
        float reduction = resistance / 100.0f;
        uint16_t reducedDamage = static_cast<uint16_t>(damage * (1.0f - reduction));
        
        // Garantir dano mínimo
        if (reducedDamage < 1) reducedDamage = 1;
        
        // Verificar se está morto
        if (hp <= reducedDamage) {
            hp = 0;
            state = CharacterState::Dead;
            return true; // Morreu
        }
        
        // Aplicar dano
        hp -= reducedDamage;
        return false; // Sobreviveu
    }
    
    // Verificar se skill está disponível
    bool CanUseSkill(uint16_t skillId, uint32_t currentTime) const {
        for (int i = 0; i < MAX_SKILL; ++i) {
            if (skills[i].skillId == skillId) {
                return skills[i].CanUse(currentTime, mp);
            }
        }
        return false;
    }
    
    // Obter skill pelo ID
    const SkillInfo* GetSkill(uint16_t skillId) const {
        for (int i = 0; i < MAX_SKILL; ++i) {
            if (skills[i].skillId == skillId) {
                return &skills[i];
            }
        }
        return nullptr;
    }
    
    // Verificar se tem efeito ativo
    bool HasActiveEffect(uint16_t effectId, uint32_t currentTime) const {
        for (int i = 0; i < MAX_AFFECT; ++i) {
            if (affects[i].effectId == effectId && affects[i].IsActive(currentTime)) {
                return true;
            }
        }
        return false;
    }
    
    // Verificar nome (sanitização)
    bool HasValidName() const {
        // Verificar comprimento
        size_t len = 0;
        while (len < sizeof(name) && name[len] != '\0') len++;
        
        if (len == 0 || len >= sizeof(name)) {
            return false;
        }
        
        // Verificar caracteres
        for (size_t i = 0; i < len; ++i) {
            if (!isalnum(name[i]) && name[i] != '_' && name[i] != '-') {
                return false;
            }
        }
        
        return true;
    }
};

} // namespace WYDBR

#endif // WYDBR_MOB_STRUCT_H

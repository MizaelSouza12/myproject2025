#!/bin/bash

echo "Iniciando geração dos arquivos do Sistema de Combate Avançado para WYDBR 2.0..."

# Criando diretórios necessários
mkdir -p src/combat_system/core
mkdir -p src/combat_system/abilities
mkdir -p src/combat_system/damage
mkdir -p src/combat_system/effects
mkdir -p src/combat_system/targeting
mkdir -p include/combat_system

# Gerando arquivos de cabeçalho
cat > include/combat_system/CombatSystem.h << 'EOL'
#pragma once

#include <memory>
#include <string>
#include <vector>
#include "AbilityManager.h"
#include "DamageCalculator.h"
#include "EffectSystem.h"
#include "TargetingSystem.h"

namespace WYDBR {
namespace CombatSystem {

class CombatSystem {
public:
    CombatSystem();
    ~CombatSystem();

    bool Initialize();
    void Update(float deltaTime);
    void Shutdown();

    std::shared_ptr<AbilityManager> GetAbilityManager() const;
    std::shared_ptr<DamageCalculator> GetDamageCalculator() const;
    std::shared_ptr<EffectSystem> GetEffectSystem() const;
    std::shared_ptr<TargetingSystem> GetTargetingSystem() const;

    bool ExecuteAbility(uint64_t sourceId, uint64_t targetId, uint32_t abilityId);
    float CalculateDamage(uint64_t sourceId, uint64_t targetId, uint32_t damageType, float baseDamage);
    bool ApplyEffect(uint64_t targetId, uint32_t effectId, float duration, float intensity);

private:
    std::shared_ptr<AbilityManager> m_abilityManager;
    std::shared_ptr<DamageCalculator> m_damageCalculator;
    std::shared_ptr<EffectSystem> m_effectSystem;
    std::shared_ptr<TargetingSystem> m_targetingSystem;
    
    bool m_initialized;
};

} // namespace CombatSystem
} // namespace WYDBR
EOL

cat > include/combat_system/AbilityManager.h << 'EOL'
#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "Ability.h"

namespace WYDBR {
namespace CombatSystem {

class AbilityManager {
public:
    AbilityManager();
    ~AbilityManager();

    bool Initialize();
    void Update(float deltaTime);
    void Shutdown();

    std::shared_ptr<Ability> GetAbility(uint32_t abilityId);
    bool RegisterAbility(std::shared_ptr<Ability> ability);
    bool UnregisterAbility(uint32_t abilityId);
    
    std::vector<std::shared_ptr<Ability>> GetAbilitiesForClass(uint32_t classId);
    bool IsAbilityOnCooldown(uint64_t entityId, uint32_t abilityId);
    float GetAbilityCooldownRemaining(uint64_t entityId, uint32_t abilityId);

private:
    std::unordered_map<uint32_t, std::shared_ptr<Ability>> m_abilities;
    std::unordered_map<uint64_t, std::unordered_map<uint32_t, float>> m_cooldowns;
    bool m_initialized;
};

} // namespace CombatSystem
} // namespace WYDBR
EOL

cat > include/combat_system/DamageCalculator.h << 'EOL'
#pragma once

#include <unordered_map>
#include <memory>

namespace WYDBR {
namespace CombatSystem {

enum class DamageType {
    Physical,
    Fire,
    Ice,
    Lightning,
    Poison,
    Arcane,
    Holy,
    Shadow
};

class DamageCalculator {
public:
    DamageCalculator();
    ~DamageCalculator();

    bool Initialize();
    void Update(float deltaTime);
    void Shutdown();

    float CalculateDamage(uint64_t sourceId, uint64_t targetId, DamageType type, float baseDamage);
    float CalculateCriticalHit(uint64_t sourceId, float baseDamage);
    float CalculateResistance(uint64_t targetId, DamageType type);
    
    void SetDamageMultiplier(DamageType type, float multiplier);
    float GetDamageMultiplier(DamageType type) const;

private:
    std::unordered_map<DamageType, float> m_damageMultipliers;
    bool m_initialized;
};

} // namespace CombatSystem
} // namespace WYDBR
EOL

cat > include/combat_system/EffectSystem.h << 'EOL'
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "Effect.h"

namespace WYDBR {
namespace CombatSystem {

class EffectSystem {
public:
    EffectSystem();
    ~EffectSystem();

    bool Initialize();
    void Update(float deltaTime);
    void Shutdown();

    bool ApplyEffect(uint64_t targetId, uint32_t effectId, float duration, float intensity);
    bool RemoveEffect(uint64_t targetId, uint32_t effectId);
    bool HasEffect(uint64_t targetId, uint32_t effectId);
    
    std::vector<std::shared_ptr<Effect>> GetActiveEffects(uint64_t targetId);
    std::shared_ptr<Effect> GetEffectTemplate(uint32_t effectId);
    
    void RegisterEffectTemplate(std::shared_ptr<Effect> effectTemplate);

private:
    std::unordered_map<uint32_t, std::shared_ptr<Effect>> m_effectTemplates;
    std::unordered_map<uint64_t, std::vector<std::shared_ptr<Effect>>> m_activeEffects;
    bool m_initialized;
};

} // namespace CombatSystem
} // namespace WYDBR
EOL

cat > include/combat_system/TargetingSystem.h << 'EOL'
#pragma once

#include <memory>
#include <vector>

namespace WYDBR {
namespace CombatSystem {

enum class TargetingType {
    Single,
    Area,
    Cone,
    Line
};

class TargetingSystem {
public:
    TargetingSystem();
    ~TargetingSystem();

    bool Initialize();
    void Update(float deltaTime);
    void Shutdown();

    std::vector<uint64_t> GetValidTargets(uint64_t sourceId, TargetingType type, 
                                         float x, float y, float z, 
                                         float dirX, float dirY, float dirZ,
                                         float range, float angle);
                                         
    bool IsValidTarget(uint64_t sourceId, uint64_t targetId);
    float GetDistance(uint64_t entityId1, uint64_t entityId2);
    bool IsInLineOfSight(uint64_t entityId1, uint64_t entityId2);

private:
    bool m_initialized;
};

} // namespace CombatSystem
} // namespace WYDBR
EOL

# Gerando arquivos de implementação
cat > src/combat_system/core/CombatSystem.cpp << 'EOL'
#include "CombatSystem.h"
#include <iostream>

namespace WYDBR {
namespace CombatSystem {

CombatSystem::CombatSystem() : m_initialized(false) {
}

CombatSystem::~CombatSystem() {
    if (m_initialized) {
        Shutdown();
    }
}

bool CombatSystem::Initialize() {
    std::cout << "Inicializando Sistema de Combate Avançado..." << std::endl;
    
    m_abilityManager = std::make_shared<AbilityManager>();
    if (!m_abilityManager->Initialize()) {
        std::cerr << "Falha ao inicializar AbilityManager" << std::endl;
        return false;
    }
    
    m_damageCalculator = std::make_shared<DamageCalculator>();
    if (!m_damageCalculator->Initialize()) {
        std::cerr << "Falha ao inicializar DamageCalculator" << std::endl;
        return false;
    }
    
    m_effectSystem = std::make_shared<EffectSystem>();
    if (!m_effectSystem->Initialize()) {
        std::cerr << "Falha ao inicializar EffectSystem" << std::endl;
        return false;
    }
    
    m_targetingSystem = std::make_shared<TargetingSystem>();
    if (!m_targetingSystem->Initialize()) {
        std::cerr << "Falha ao inicializar TargetingSystem" << std::endl;
        return false;
    }
    
    m_initialized = true;
    std::cout << "Sistema de Combate Avançado inicializado com sucesso!" << std::endl;
    return true;
}

void CombatSystem::Update(float deltaTime) {
    if (!m_initialized) return;
    
    m_abilityManager->Update(deltaTime);
    m_damageCalculator->Update(deltaTime);
    m_effectSystem->Update(deltaTime);
    m_targetingSystem->Update(deltaTime);
}

void CombatSystem::Shutdown() {
    if (!m_initialized) return;
    
    std::cout << "Desligando Sistema de Combate Avançado..." << std::endl;
    
    m_targetingSystem->Shutdown();
    m_effectSystem->Shutdown();
    m_damageCalculator->Shutdown();
    m_abilityManager->Shutdown();
    
    m_initialized = false;
    std::cout << "Sistema de Combate Avançado desligado com sucesso!" << std::endl;
}

std::shared_ptr<AbilityManager> CombatSystem::GetAbilityManager() const {
    return m_abilityManager;
}

std::shared_ptr<DamageCalculator> CombatSystem::GetDamageCalculator() const {
    return m_damageCalculator;
}

std::shared_ptr<EffectSystem> CombatSystem::GetEffectSystem() const {
    return m_effectSystem;
}

std::shared_ptr<TargetingSystem> CombatSystem::GetTargetingSystem() const {
    return m_targetingSystem;
}

bool CombatSystem::ExecuteAbility(uint64_t sourceId, uint64_t targetId, uint32_t abilityId) {
    if (!m_initialized) return false;
    
    auto ability = m_abilityManager->GetAbility(abilityId);
    if (!ability) {
        std::cerr << "Habilidade não encontrada: " << abilityId << std::endl;
        return false;
    }
    
    if (m_abilityManager->IsAbilityOnCooldown(sourceId, abilityId)) {
        std::cerr << "Habilidade em cooldown: " << abilityId << std::endl;
        return false;
    }
    
    if (!m_targetingSystem->IsValidTarget(sourceId, targetId)) {
        std::cerr << "Alvo inválido: " << targetId << std::endl;
        return false;
    }
    
    // Executa a lógica da habilidade
    ability->Execute(sourceId, targetId);
    
    return true;
}

float CombatSystem::CalculateDamage(uint64_t sourceId, uint64_t targetId, uint32_t damageType, float baseDamage) {
    if (!m_initialized) return 0.0f;
    
    return m_damageCalculator->CalculateDamage(sourceId, targetId, static_cast<DamageType>(damageType), baseDamage);
}

bool CombatSystem::ApplyEffect(uint64_t targetId, uint32_t effectId, float duration, float intensity) {
    if (!m_initialized) return false;
    
    return m_effectSystem->ApplyEffect(targetId, effectId, duration, intensity);
}

} // namespace CombatSystem
} // namespace WYDBR
EOL

echo "Arquivos do Sistema de Combate Avançado gerados com sucesso na pasta src/combat_system e include/combat_system!"
echo "Use 'make combat_system' para compilar o sistema." 
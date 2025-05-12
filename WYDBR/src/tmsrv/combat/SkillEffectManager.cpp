/**
 * @file SkillEffectManager.cpp
 * @brief Implementação do gerenciador de efeitos de habilidades
 */

#include "SkillEffectManager.h"
#include <algorithm>
#include <functional>

namespace wydbr {
namespace tmsrv {
namespace combat {

SkillEffectManager::SkillEffectManager(common::utils::TimerManager& timerManager)
    : timerManager_(timerManager), nextEffectId_(1) {
}

SkillEffectManager::~SkillEffectManager() {
    // Limpar todos os efeitos ao destruir
    std::lock_guard<std::mutex> lock(effectsMutex_);
    activeEffects_.clear();
}

uint32_t SkillEffectManager::ApplyEffect(
    const common::types::ActorId& targetId,
    const common::types::SkillEffect& effect,
    const common::types::CharacterStats* casterStats,
    bool stackable) {
    
    std::lock_guard<std::mutex> lock(effectsMutex_);
    
    // Verificar se já existe um efeito do mesmo tipo caso não seja stackable
    if (!stackable) {
        auto& targetEffects = activeEffects_[targetId];
        auto it = std::find_if(targetEffects.begin(), targetEffects.end(),
            [&effect](const ActiveEffect& activeEffect) {
                return activeEffect.effect.effect_type == effect.effect_type;
            });
            
        // Se encontrar um efeito similar, substitui se o novo for mais forte
        if (it != targetEffects.end()) {
            // Comparar potência dos efeitos (pode ser valor, duração, etc.)
            if (effect.value > it->effect.value || 
                effect.duration > it->effect.duration) {
                
                // Remove o efeito anterior e aplica o novo
                uint32_t oldEffectId = it->id;
                targetEffects.erase(it);
                
                // Continua para aplicar o novo efeito
            } else {
                // Mantém o efeito existente por ser mais forte
                return 0;
            }
        }
    }
    
    // Criar novo efeito ativo
    uint32_t effectId = nextEffectId_++;
    ActiveEffect activeEffect;
    activeEffect.id = effectId;
    activeEffect.effect = effect;
    activeEffect.casterStats = casterStats;
    
    // Configurar timers
    auto now = std::chrono::steady_clock::now();
    activeEffect.endTime = now + effect.duration;
    
    if (effect.tick_interval.count() > 0) {
        // Para efeitos com ticks (DoT/HoT)
        activeEffect.nextTickTime = now + effect.tick_interval;
    } else {
        // Para efeitos instantâneos ou contínuos sem ticks
        activeEffect.nextTickTime = activeEffect.endTime;
    }
    
    // Adicionar à lista de efeitos ativos
    activeEffects_[targetId].push_back(activeEffect);
    
    // Agendar remoção automática quando o efeito expirar
    ScheduleEffectRemoval(targetId, effectId, effect.duration);
    
    return effectId;
}

bool SkillEffectManager::RemoveEffect(
    const common::types::ActorId& targetId,
    uint32_t effectId) {
    
    std::lock_guard<std::mutex> lock(effectsMutex_);
    
    auto it = FindEffect(targetId, effectId);
    if (it == activeEffects_[targetId].end()) {
        return false; // Efeito não encontrado
    }
    
    activeEffects_[targetId].erase(it);
    
    // Se não houver mais efeitos para este alvo, remover a entrada do mapa
    if (activeEffects_[targetId].empty()) {
        activeEffects_.erase(targetId);
    }
    
    return true;
}

int SkillEffectManager::RemoveEffectsByType(
    const common::types::ActorId& targetId,
    uint8_t effectType) {
    
    std::lock_guard<std::mutex> lock(effectsMutex_);
    
    auto targetIt = activeEffects_.find(targetId);
    if (targetIt == activeEffects_.end()) {
        return 0; // Alvo não possui efeitos
    }
    
    auto& targetEffects = targetIt->second;
    int removedCount = 0;
    
    targetEffects.erase(
        std::remove_if(targetEffects.begin(), targetEffects.end(),
            [effectType, &removedCount](const ActiveEffect& effect) {
                bool shouldRemove = effect.effect.effect_type == effectType;
                if (shouldRemove) {
                    removedCount++;
                }
                return shouldRemove;
            }),
        targetEffects.end());
    
    // Se não houver mais efeitos para este alvo, remover a entrada do mapa
    if (targetEffects.empty()) {
        activeEffects_.erase(targetId);
    }
    
    return removedCount;
}

bool SkillEffectManager::HasEffect(
    const common::types::ActorId& targetId,
    uint8_t effectType) const {
    
    std::lock_guard<std::mutex> lock(effectsMutex_);
    
    auto targetIt = activeEffects_.find(targetId);
    if (targetIt == activeEffects_.end()) {
        return false; // Alvo não possui efeitos
    }
    
    const auto& targetEffects = targetIt->second;
    return std::any_of(targetEffects.begin(), targetEffects.end(),
        [effectType](const ActiveEffect& effect) {
            return effect.effect.effect_type == effectType;
        });
}

std::vector<common::types::SkillEffect> SkillEffectManager::GetActiveEffects(
    const common::types::ActorId& targetId) const {
    
    std::lock_guard<std::mutex> lock(effectsMutex_);
    
    std::vector<common::types::SkillEffect> effects;
    
    auto targetIt = activeEffects_.find(targetId);
    if (targetIt == activeEffects_.end()) {
        return effects; // Retorna vetor vazio
    }
    
    const auto& targetEffects = targetIt->second;
    effects.reserve(targetEffects.size());
    
    for (const auto& activeEffect : targetEffects) {
        effects.push_back(activeEffect.effect);
    }
    
    return effects;
}

void SkillEffectManager::ProcessEffectTicks(
    const common::types::ActorId& targetId,
    std::chrono::milliseconds deltaTime) {
    
    std::lock_guard<std::mutex> lock(effectsMutex_);
    
    auto targetIt = activeEffects_.find(targetId);
    if (targetIt == activeEffects_.end()) {
        return; // Alvo não possui efeitos
    }
    
    auto& targetEffects = targetIt->second;
    auto now = std::chrono::steady_clock::now();
    
    for (auto& activeEffect : targetEffects) {
        // Verificar se é hora de aplicar um tick
        if (activeEffect.effect.tick_interval.count() > 0 && 
            now >= activeEffect.nextTickTime) {
            
            // Aplicar o efeito do tick (implementação real despacharia para o sistema de processamento)
            // Exemplo: dano ao longo do tempo, cura ao longo do tempo, etc.
            
            // Agendar próximo tick
            activeEffect.nextTickTime += activeEffect.effect.tick_interval;
            
            // Se o próximo tick estiver além do fim do efeito, ajustar
            if (activeEffect.nextTickTime > activeEffect.endTime) {
                activeEffect.nextTickTime = activeEffect.endTime;
            }
        }
    }
}

int SkillEffectManager::ClearEffects(
    const common::types::ActorId& targetId,
    bool removePositive) {
    
    std::lock_guard<std::mutex> lock(effectsMutex_);
    
    auto targetIt = activeEffects_.find(targetId);
    if (targetIt == activeEffects_.end()) {
        return 0; // Alvo não possui efeitos
    }
    
    auto& targetEffects = targetIt->second;
    int removedCount = 0;
    
    if (removePositive) {
        // Remover todos os efeitos
        removedCount = static_cast<int>(targetEffects.size());
        activeEffects_.erase(targetId);
    } else {
        // Remover apenas efeitos negativos
        int initialSize = static_cast<int>(targetEffects.size());
        
        targetEffects.erase(
            std::remove_if(targetEffects.begin(), targetEffects.end(),
                [](const ActiveEffect& effect) {
                    // Verificar se o efeito é negativo (depende da implementação)
                    // Por exemplo: efeito_tipo >= 100 são negativos
                    return effect.effect.effect_type >= 100;
                }),
            targetEffects.end());
            
        removedCount = initialSize - static_cast<int>(targetEffects.size());
        
        // Se não houver mais efeitos para este alvo, remover a entrada do mapa
        if (targetEffects.empty()) {
            activeEffects_.erase(targetId);
        }
    }
    
    return removedCount;
}

void SkillEffectManager::ApplyEffectsToStats(
    const common::types::ActorId& targetId,
    const common::types::CharacterStats& baseStats,
    common::types::CharacterStats& resultStats) const {
    
    std::lock_guard<std::mutex> lock(effectsMutex_);
    
    // Iniciar com as estatísticas base
    resultStats = baseStats;
    
    auto targetIt = activeEffects_.find(targetId);
    if (targetIt == activeEffects_.end()) {
        return; // Alvo não possui efeitos, mantém stats base
    }
    
    const auto& targetEffects = targetIt->second;
    
    // Primeiro, aplicar modificadores planos
    for (const auto& activeEffect : targetEffects) {
        const auto& effect = activeEffect.effect;
        
        // Aplicar com base no tipo de efeito e stat_type
        switch (effect.stat_type) {
            case 0: // Strength
                resultStats.strength += static_cast<int32_t>(effect.value);
                break;
            case 1: // Vitality
                resultStats.vitality += static_cast<int32_t>(effect.value);
                break;
            case 2: // Dexterity
                resultStats.dexterity += static_cast<int32_t>(effect.value);
                break;
            case 3: // Intelligence
                resultStats.intelligence += static_cast<int32_t>(effect.value);
                break;
            case 4: // Wisdom
                resultStats.wisdom += static_cast<int32_t>(effect.value);
                break;
            // Outros casos para diferentes estatísticas
        }
    }
    
    // Em seguida, aplicar modificadores percentuais
    for (const auto& activeEffect : targetEffects) {
        const auto& effect = activeEffect.effect;
        
        // Pular efeitos que não são modificadores percentuais
        if (!effect.is_percent_modifier) {
            continue;
        }
        
        // Aplicar com base no tipo de efeito e stat_type
        switch (effect.stat_type) {
            case 0: // Strength
                resultStats.strength = static_cast<int32_t>(resultStats.strength * (1.0f + effect.value / 100.0f));
                break;
            case 1: // Vitality
                resultStats.vitality = static_cast<int32_t>(resultStats.vitality * (1.0f + effect.value / 100.0f));
                break;
            case 2: // Dexterity
                resultStats.dexterity = static_cast<int32_t>(resultStats.dexterity * (1.0f + effect.value / 100.0f));
                break;
            case 3: // Intelligence
                resultStats.intelligence = static_cast<int32_t>(resultStats.intelligence * (1.0f + effect.value / 100.0f));
                break;
            case 4: // Wisdom
                resultStats.wisdom = static_cast<int32_t>(resultStats.wisdom * (1.0f + effect.value / 100.0f));
                break;
            // Outros casos para diferentes estatísticas
        }
    }
    
    // Garantir que nenhuma estatística fique abaixo do mínimo
    resultStats.strength = std::max(1, resultStats.strength);
    resultStats.vitality = std::max(1, resultStats.vitality);
    resultStats.dexterity = std::max(1, resultStats.dexterity);
    resultStats.intelligence = std::max(1, resultStats.intelligence);
    resultStats.wisdom = std::max(1, resultStats.wisdom);
}

void SkillEffectManager::ScheduleEffectRemoval(
    const common::types::ActorId& targetId,
    uint32_t effectId,
    std::chrono::milliseconds duration) {
    
    // Usar o TimerManager para agendar a remoção
    timerManager_.ScheduleTask(
        [this, targetId, effectId]() {
            this->RemoveEffect(targetId, effectId);
        },
        duration);
}

std::vector<SkillEffectManager::ActiveEffect>::iterator SkillEffectManager::FindEffect(
    const common::types::ActorId& targetId,
    uint32_t effectId) {
    
    auto targetIt = activeEffects_.find(targetId);
    if (targetIt == activeEffects_.end()) {
        // Se o alvo não tem nenhum efeito, retorna um iterador inválido
        // Como não podemos retornar activeEffects_[targetId].end() diretamente
        // (criaria uma entrada vazia), adicionamos uma entrada vazia temporária
        activeEffects_[targetId] = {};
        return activeEffects_[targetId].end();
    }
    
    auto& targetEffects = targetIt->second;
    return std::find_if(targetEffects.begin(), targetEffects.end(),
        [effectId](const ActiveEffect& effect) {
            return effect.id == effectId;
        });
}

} // namespace combat
} // namespace tmsrv
} // namespace wydbr 
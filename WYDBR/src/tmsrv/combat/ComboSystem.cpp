/**
 * @file ComboSystem.cpp
 * @brief Implementação do sistema de combos
 */

#include "ComboSystem.h"
#include <algorithm>
#include <functional>

namespace wydbr {
namespace tmsrv {
namespace combat {

ComboSystem::ComboSystem(common::utils::TimerManager& timerManager)
    : timerManager_(timerManager), nextComboId_(1), nextBonusId_(1) {
}

ComboSystem::~ComboSystem() {
}

uint32_t ComboSystem::RegisterCombo(const common::types::ComboDefinition& comboDefinition) {
    std::lock_guard<std::mutex> lock(comboMutex_);
    
    // Verificar se já existe um combo com a mesma sequência
    for (const auto& combo : comboDefinitions_) {
        if (combo.skill_sequence == comboDefinition.skill_sequence &&
            combo.required_class == comboDefinition.required_class) {
            return 0; // Combo já registrado
        }
    }
    
    // Atribuir ID e adicionar à lista
    common::types::ComboDefinition newCombo = comboDefinition;
    newCombo.combo_id = nextComboId_++;
    comboDefinitions_.push_back(newCombo);
    
    return newCombo.combo_id;
}

bool ComboSystem::UnregisterCombo(uint32_t comboId) {
    std::lock_guard<std::mutex> lock(comboMutex_);
    
    auto it = std::find_if(comboDefinitions_.begin(), comboDefinitions_.end(),
        [comboId](const common::types::ComboDefinition& combo) {
            return combo.combo_id == comboId;
        });
    
    if (it != comboDefinitions_.end()) {
        comboDefinitions_.erase(it);
        return true;
    }
    
    return false;
}

common::types::ComboResult ComboSystem::ProcessSkillUse(
    const common::types::ActorId& actorId,
    common::types::SkillId skillId,
    const common::types::ActorId* targetId,
    std::chrono::steady_clock::time_point timestamp) {
    
    std::lock_guard<std::mutex> lock(comboMutex_);
    
    common::types::ComboResult result;
    result.combo_activated = false;
    result.combo_advanced = false;
    
    auto& activeCombo = activePlayerCombos_[actorId];
    
    // Verificar se o combo existente expirou
    bool isComboExpired = false;
    if (!activeCombo.skillSequence.empty()) {
        auto timeSinceLastSkill = timestamp - activeCombo.lastSkillTime;
        if (timeSinceLastSkill > std::chrono::seconds(5)) { // 5 segundos é o tempo padrão de expiração
            isComboExpired = true;
        }
    }
    
    // Reiniciar o combo se expirou ou se pressionar a mesma habilidade novamente
    if (isComboExpired || 
        (!activeCombo.skillSequence.empty() && activeCombo.skillSequence.back() == skillId)) {
        activeCombo.skillSequence.clear();
    }
    
    // Adicionar a habilidade à sequência
    activeCombo.skillSequence.push_back(skillId);
    activeCombo.lastSkillTime = timestamp;
    if (targetId) {
        activeCombo.lastTarget = *targetId;
    }
    
    // Manter apenas as últimas 5 habilidades na sequência (janela de combo)
    if (activeCombo.skillSequence.size() > 5) {
        activeCombo.skillSequence.erase(activeCombo.skillSequence.begin());
    }
    
    // Verificar se a sequência corresponde a um combo
    const common::types::ComboDefinition* matchedCombo = FindMatchingCombo(
        activeCombo.skillSequence, targetId);
    
    // Se encontrou um combo correspondente
    if (matchedCombo) {
        // Se a sequência completa corresponde exatamente
        if (activeCombo.skillSequence.size() == matchedCombo->skill_sequence.size()) {
            // Combo completo!
            result.combo_activated = true;
            result.combo_id = matchedCombo->combo_id;
            result.combo_name = matchedCombo->name;
            
            // Aplicar efeitos do combo
            ApplyComboEffects(actorId, *matchedCombo, targetId);
            
            // Atualizar as informações do combo ativo
            activeCombo.info.id = matchedCombo->combo_id;
            activeCombo.info.name = matchedCombo->name;
            activeCombo.info.level = matchedCombo->level;
            activeCombo.info.current_step = matchedCombo->skill_sequence.size();
            activeCombo.info.total_steps = matchedCombo->skill_sequence.size();
            
            // Reiniciar a sequência após combo completo
            activeCombo.skillSequence.clear();
        } else {
            // Combo em progresso
            result.combo_advanced = true;
            result.combo_id = matchedCombo->combo_id;
            result.combo_name = matchedCombo->name;
            result.current_step = activeCombo.skillSequence.size();
            result.total_steps = matchedCombo->skill_sequence.size();
            
            // Atualizar as informações do combo ativo
            activeCombo.info.id = matchedCombo->combo_id;
            activeCombo.info.name = matchedCombo->name;
            activeCombo.info.level = matchedCombo->level;
            activeCombo.info.current_step = activeCombo.skillSequence.size();
            activeCombo.info.total_steps = matchedCombo->skill_sequence.size();
        }
    } else {
        // Nenhum combo conhecido corresponde a esta sequência
        if (!activeCombo.skillSequence.empty()) {
            // Reiniciar o combo se a sequência não for reconhecida
            activeCombo.skillSequence.clear();
            activeCombo.info = {}; // Limpar informações do combo
        }
    }
    
    return result;
}

common::types::ComboInfo ComboSystem::GetCurrentCombo(const common::types::ActorId& actorId) const {
    std::lock_guard<std::mutex> lock(comboMutex_);
    
    auto it = activePlayerCombos_.find(actorId);
    if (it != activePlayerCombos_.end()) {
        return it->second.info;
    }
    
    return {};
}

void ComboSystem::ResetCombo(const common::types::ActorId& actorId) {
    std::lock_guard<std::mutex> lock(comboMutex_);
    
    auto it = activePlayerCombos_.find(actorId);
    if (it != activePlayerCombos_.end()) {
        it->second.skillSequence.clear();
        it->second.info = {}; // Limpar informações do combo
    }
}

uint32_t ComboSystem::AddComboBonus(
    const common::types::ActorId& actorId,
    uint8_t bonusType,
    float bonusValue,
    std::chrono::milliseconds duration) {
    
    std::lock_guard<std::mutex> lock(bonusMutex_);
    
    ComboBonus bonus;
    bonus.id = nextBonusId_++;
    bonus.type = bonusType;
    bonus.value = bonusValue;
    bonus.endTime = std::chrono::steady_clock::now() + duration;
    
    actorBonuses_[actorId].push_back(bonus);
    
    // Programar a remoção automática
    ScheduleBonusRemoval(actorId, bonus.id, duration);
    
    return bonus.id;
}

bool ComboSystem::RemoveComboBonus(
    const common::types::ActorId& actorId,
    uint32_t bonusId) {
    
    std::lock_guard<std::mutex> lock(bonusMutex_);
    
    auto actorIt = actorBonuses_.find(actorId);
    if (actorIt == actorBonuses_.end()) {
        return false;
    }
    
    auto& bonuses = actorIt->second;
    auto bonusIt = std::find_if(bonuses.begin(), bonuses.end(),
        [bonusId](const ComboBonus& bonus) {
            return bonus.id == bonusId;
        });
    
    if (bonusIt != bonuses.end()) {
        bonuses.erase(bonusIt);
        
        // Se não houver mais bônus, remover a entrada
        if (bonuses.empty()) {
            actorBonuses_.erase(actorIt);
        }
        
        return true;
    }
    
    return false;
}

std::vector<common::types::ComboBonus> ComboSystem::GetActiveBonuses(
    const common::types::ActorId& actorId) const {
    
    std::lock_guard<std::mutex> lock(bonusMutex_);
    
    std::vector<common::types::ComboBonus> result;
    auto actorIt = actorBonuses_.find(actorId);
    if (actorIt == actorBonuses_.end()) {
        return result;
    }
    
    // Converter bonuses internos para o formato público
    for (const auto& bonus : actorIt->second) {
        common::types::ComboBonus publicBonus;
        publicBonus.id = bonus.id;
        publicBonus.type = bonus.type;
        publicBonus.value = bonus.value;
        
        result.push_back(publicBonus);
    }
    
    return result;
}

const common::types::ComboDefinition* ComboSystem::FindMatchingCombo(
    const std::vector<common::types::SkillId>& skillSequence,
    const common::types::ActorId* targetId) const {
    
    if (skillSequence.empty()) {
        return nullptr;
    }
    
    // Verificar cada combo registrado
    for (const auto& combo : comboDefinitions_) {
        // Verificar se a sequência atual coincide com o início do combo
        if (skillSequence.size() <= combo.skill_sequence.size()) {
            bool matches = true;
            
            for (size_t i = 0; i < skillSequence.size(); ++i) {
                if (skillSequence[i] != combo.skill_sequence[i]) {
                    matches = false;
                    break;
                }
            }
            
            if (matches) {
                return &combo;
            }
        }
    }
    
    return nullptr;
}

void ComboSystem::ApplyComboEffects(
    const common::types::ActorId& actorId,
    const common::types::ComboDefinition& combo,
    const common::types::ActorId* targetId) {
    
    // Aplicar bônus temporários baseados no combo
    for (const auto& effect : combo.effects) {
        AddComboBonus(
            actorId,
            effect.bonus_type,
            effect.bonus_value,
            std::chrono::milliseconds(effect.duration_ms));
    }
    
    // Nota: em uma implementação real, aqui também despacharia eventos para
    // outros sistemas (ex: efeitos visuais, dano adicional, etc)
}

void ComboSystem::ScheduleBonusRemoval(
    const common::types::ActorId& actorId,
    uint32_t bonusId,
    std::chrono::milliseconds duration) {
    
    timerManager_.ScheduleTask(
        [this, actorId, bonusId]() {
            this->RemoveComboBonus(actorId, bonusId);
        },
        duration);
}

} // namespace combat
} // namespace tmsrv
} // namespace wydbr 
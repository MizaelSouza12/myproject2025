/**
 * SkillLoader.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/skill/SkillLoader.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "../../include/skill/SkillLoader.h"
#include "../../include/core/Logger.h"

#include <fstream>
#include <cstring>
#include <algorithm>
#include <iostream>

namespace wydbr {
namespace skill {

// Constantes para validação do arquivo
const char* SKILL_FILE_SIGNATURE = "SKIL";
const uint32_t SKILL_FILE_VERSION = 0x00010000;

// Singleton global
std::unique_ptr<SkillLoader> g_skillLoader = std::make_unique<SkillLoader>();

SkillLoader::SkillLoader() : m_loaded(false), m_skillCount(0), m_fileVersion(SKILL_FILE_VERSION) {
    core::g_logger->info("SkillLoader", "Inicializando carregador de habilidades");
}

SkillLoader::~SkillLoader() {
    core::g_logger->info("SkillLoader", "Finalizando carregador de habilidades");
}

bool SkillLoader::loadFromFile(const std::string& filePath) {
    // Já carregado?
    if (m_loaded) {
        core::g_logger->warning("SkillLoader", "Habilidades já foram carregadas");
        return true;
    }

    core::g_logger->info("SkillLoader", "Carregando habilidades do arquivo: " + filePath);

    // Abrir o arquivo
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        core::g_logger->error("SkillLoader", "Falha ao abrir o arquivo: " + filePath);
        return false;
    }

    // Ler o cabeçalho
    BIN_HEADER header;
    file.read(reinterpret_cast<char*>(&header), sizeof(BIN_HEADER));

    // Verificar o cabeçalho
    if (!validateFileHeader(header)) {
        core::g_logger->error("SkillLoader", "Arquivo de habilidades inválido: assinatura incorreta");
        file.close();
        return false;
    }

    // Verificar a versão
    if (header.Version != SKILL_FILE_VERSION) {
        core::g_logger->warning("SkillLoader", "Versão do arquivo diferente da esperada: " + 
                              std::to_string(header.Version) + " vs " + std::to_string(SKILL_FILE_VERSION));
        // Atualizar a versão para a do arquivo lido
        m_fileVersion = header.Version;
    }

    // Verificar o número de habilidades
    if (header.SkillCount > SkillLimits::MAX_SKILL_ID) {
        core::g_logger->error("SkillLoader", "Número de habilidades excede o máximo permitido: " +
                            std::to_string(header.SkillCount) + " vs " + std::to_string(SkillLimits::MAX_SKILL_ID));
        file.close();
        return false;
    }

    // Ler as habilidades
    core::g_logger->info("SkillLoader", "Lendo " + std::to_string(header.SkillCount) + " habilidades");
    
    m_skills.clear();
    
    for (uint32_t i = 0; i < header.SkillCount; ++i) {
        // Ler entrada de habilidade
        BIN_SKILL_ENTRY entry;
        file.read(reinterpret_cast<char*>(&entry), sizeof(BIN_SKILL_ENTRY));
        
        // Processar os dados brutos da habilidade
        if (!processRawSkillData(entry.SkillID, entry.Data)) {
            core::g_logger->warning("SkillLoader", "Falha ao processar habilidade ID: " + std::to_string(entry.SkillID));
            continue;
        }
        
        // Armazenar a habilidade
        m_skills[entry.SkillID] = entry.Data;
    }
    
    // Verificar se todas as habilidades foram carregadas
    if (m_skills.size() != header.SkillCount) {
        core::g_logger->warning("SkillLoader", "Nem todas as habilidades foram carregadas: " + 
                              std::to_string(m_skills.size()) + " vs " + std::to_string(header.SkillCount));
    }
    
    // Atualizar contagem de habilidades
    m_skillCount = m_skills.size();
    m_loaded = true;
    
    // Fechar o arquivo
    file.close();
    
    core::g_logger->info("SkillLoader", "Carregamento de habilidades concluído com sucesso");
    
    return true;
}

size_t SkillLoader::getSkillCount() const {
    return m_skillCount;
}

const STRUCT_SKILLDATA* SkillLoader::getSkillData(uint16_t skillId) const {
    auto it = m_skills.find(skillId);
    if (it != m_skills.end()) {
        return &(it->second);
    }
    return nullptr;
}

const std::map<uint16_t, STRUCT_SKILLDATA>& SkillLoader::getAllSkills() const {
    return m_skills;
}

bool SkillLoader::hasSkill(uint16_t skillId) const {
    return m_skills.find(skillId) != m_skills.end();
}

int32_t SkillLoader::calculateSkillDamage(uint16_t skillId, uint8_t level, int32_t baseDamage) const {
    const STRUCT_SKILLDATA* skill = getSkillData(skillId);
    if (!skill) {
        return 0;
    }
    
    // Cálculo básico de dano
    // Nota: Esta é uma fórmula simplificada. A fórmula real pode ser mais complexa.
    int32_t damage = skill->InstanceValue;
    
    // Aplicar escala baseada no nível
    damage += (level * (skill->InstanceValue / 10));
    
    // Aplicar atributo de instância
    if (skill->InstanceAttribute > 0) {
        // Fator de atributo (normalmente é um percentual do atributo)
        float attributeFactor = static_cast<float>(skill->InstanceAttribute) / 100.0f;
        damage += static_cast<int32_t>(baseDamage * attributeFactor);
    }
    
    return damage;
}

bool SkillLoader::isPassiveSkill(uint16_t skillId) const {
    const STRUCT_SKILLDATA* skill = getSkillData(skillId);
    if (!skill) {
        return false;
    }
    
    return skill->PassiveCheck != 0;
}

bool SkillLoader::isAggressiveSkill(uint16_t skillId) const {
    const STRUCT_SKILLDATA* skill = getSkillData(skillId);
    if (!skill) {
        return false;
    }
    
    return skill->Aggressive != 0;
}

SkillTargetType SkillLoader::getTargetType(uint16_t skillId) const {
    const STRUCT_SKILLDATA* skill = getSkillData(skillId);
    if (!skill) {
        return SKILL_TARGET_NONE;
    }
    
    return convertTargetValue(skill->Target);
}

SkillInstanceType SkillLoader::getInstanceType(uint16_t skillId) const {
    const STRUCT_SKILLDATA* skill = getSkillData(skillId);
    if (!skill) {
        return SKILL_INSTANCE_NONE;
    }
    
    return convertInstanceTypeValue(skill->InstanceType);
}

int32_t SkillLoader::getRange(uint16_t skillId) const {
    const STRUCT_SKILLDATA* skill = getSkillData(skillId);
    if (!skill) {
        return 0;
    }
    
    return skill->Range;
}

int32_t SkillLoader::getManaCost(uint16_t skillId, uint8_t level) const {
    const STRUCT_SKILLDATA* skill = getSkillData(skillId);
    if (!skill) {
        return 0;
    }
    
    // A fórmula pode variar dependendo da implementação
    // Aqui estamos usando uma fórmula simples que aumenta o custo de MP com o nível
    int32_t baseMana = skill->Mana;
    int32_t manaPerLevel = baseMana / 10;
    
    return baseMana + (manaPerLevel * (level - 1));
}

int32_t SkillLoader::getCooldown(uint16_t skillId, uint8_t level) const {
    const STRUCT_SKILLDATA* skill = getSkillData(skillId);
    if (!skill) {
        return 0;
    }
    
    // A fórmula pode variar, mas geralmente o cooldown diminui com o nível
    int32_t baseCooldown = skill->Delay;
    int32_t cooldownReduction = baseCooldown / 20; // 5% de redução por nível
    
    int32_t cooldown = baseCooldown - (cooldownReduction * (level - 1));
    
    // Garantir que o cooldown não fique negativo
    return std::max(0, cooldown);
}

bool SkillLoader::saveToFile(const std::string& filePath) const {
    if (!m_loaded || m_skills.empty()) {
        core::g_logger->error("SkillLoader", "Nenhuma habilidade carregada para salvar");
        return false;
    }
    
    core::g_logger->info("SkillLoader", "Salvando habilidades para arquivo: " + filePath);
    
    // Abrir o arquivo
    std::ofstream file(filePath, std::ios::binary);
    if (!file) {
        core::g_logger->error("SkillLoader", "Falha ao abrir o arquivo para escrita: " + filePath);
        return false;
    }
    
    // Escrever o cabeçalho
    if (!writeFileHeader(file)) {
        core::g_logger->error("SkillLoader", "Falha ao escrever o cabeçalho do arquivo");
        file.close();
        return false;
    }
    
    // Escrever as habilidades
    for (const auto& pair : m_skills) {
        BIN_SKILL_ENTRY entry;
        entry.SkillID = pair.first;
        entry.Data = pair.second;
        
        // Escrever entrada de habilidade
        file.write(reinterpret_cast<const char*>(&entry), sizeof(BIN_SKILL_ENTRY));
    }
    
    // Fechar o arquivo
    file.close();
    
    core::g_logger->info("SkillLoader", "Salvamento de habilidades concluído com sucesso");
    
    return true;
}

bool SkillLoader::createSkill(uint16_t skillId, const STRUCT_SKILLDATA& skillData) {
    // Verificar se a habilidade já existe
    if (hasSkill(skillId)) {
        core::g_logger->error("SkillLoader", "Habilidade já existe: " + std::to_string(skillId));
        return false;
    }
    
    // Verificar se o ID da habilidade é válido
    if (!isValidSkillId(skillId)) {
        core::g_logger->error("SkillLoader", "ID de habilidade inválido: " + std::to_string(skillId));
        return false;
    }
    
    // Processar os dados brutos da habilidade
    if (!processRawSkillData(skillId, skillData)) {
        core::g_logger->error("SkillLoader", "Falha ao processar habilidade ID: " + std::to_string(skillId));
        return false;
    }
    
    // Armazenar a habilidade
    m_skills[skillId] = skillData;
    m_skillCount = m_skills.size();
    
    core::g_logger->info("SkillLoader", "Habilidade criada com sucesso: " + std::to_string(skillId));
    
    return true;
}

bool SkillLoader::updateSkill(uint16_t skillId, const STRUCT_SKILLDATA& skillData) {
    // Verificar se a habilidade existe
    if (!hasSkill(skillId)) {
        core::g_logger->error("SkillLoader", "Habilidade não existe: " + std::to_string(skillId));
        return false;
    }
    
    // Processar os dados brutos da habilidade
    if (!processRawSkillData(skillId, skillData)) {
        core::g_logger->error("SkillLoader", "Falha ao processar habilidade ID: " + std::to_string(skillId));
        return false;
    }
    
    // Atualizar a habilidade
    m_skills[skillId] = skillData;
    
    core::g_logger->info("SkillLoader", "Habilidade atualizada com sucesso: " + std::to_string(skillId));
    
    return true;
}

bool SkillLoader::removeSkill(uint16_t skillId) {
    // Verificar se a habilidade existe
    if (!hasSkill(skillId)) {
        core::g_logger->error("SkillLoader", "Habilidade não existe: " + std::to_string(skillId));
        return false;
    }
    
    // Remover a habilidade
    m_skills.erase(skillId);
    m_skillCount = m_skills.size();
    
    core::g_logger->info("SkillLoader", "Habilidade removida com sucesso: " + std::to_string(skillId));
    
    return true;
}

bool SkillLoader::validateFileHeader(const BIN_HEADER& header) const {
    // Verificar assinatura
    if (std::memcmp(header.Signature, SKILL_FILE_SIGNATURE, 4) != 0) {
        core::g_logger->error("SkillLoader", "Assinatura do arquivo inválida");
        return false;
    }
    
    // Verificar versão
    if (header.Version == 0) {
        core::g_logger->error("SkillLoader", "Versão do arquivo inválida");
        return false;
    }
    
    // Verificar contagem de habilidades
    if (header.SkillCount == 0) {
        core::g_logger->warning("SkillLoader", "Arquivo não contém habilidades");
        return false;
    }
    
    return true;
}

bool SkillLoader::processRawSkillData(uint16_t skillId, const STRUCT_SKILLDATA& rawData) {
    // Verificações básicas
    if (skillId == 0) {
        core::g_logger->warning("SkillLoader", "ID de habilidade inválido: 0");
        return false;
    }
    
    // Verificar alcance
    if (rawData.Range > SkillLimits::MAX_SKILL_RANGE) {
        core::g_logger->warning("SkillLoader", "Alcance da habilidade " + std::to_string(skillId) + 
                               " excede o máximo permitido: " + std::to_string(rawData.Range) + 
                               " vs " + std::to_string(SkillLimits::MAX_SKILL_RANGE));
        // Não retornar falso aqui para permitir carregar a habilidade mesmo com erro
    }
    
    // Verificar cooldown
    if (rawData.Delay > SkillLimits::MAX_SKILL_COOLDOWN) {
        core::g_logger->warning("SkillLoader", "Cooldown da habilidade " + std::to_string(skillId) + 
                               " excede o máximo permitido: " + std::to_string(rawData.Delay) + 
                               " vs " + std::to_string(SkillLimits::MAX_SKILL_COOLDOWN));
        // Não retornar falso aqui para permitir carregar a habilidade mesmo com erro
    }
    
    // Verificar duração
    if (rawData.Time > SkillLimits::MAX_SKILL_DURATION) {
        core::g_logger->warning("SkillLoader", "Duração da habilidade " + std::to_string(skillId) + 
                               " excede o máximo permitido: " + std::to_string(rawData.Time) + 
                               " vs " + std::to_string(SkillLimits::MAX_SKILL_DURATION));
        // Não retornar falso aqui para permitir carregar a habilidade mesmo com erro
    }
    
    // Verificar número máximo de alvos
    if (rawData.MaxTarget > SkillLimits::MAX_SKILL_TARGETS) {
        core::g_logger->warning("SkillLoader", "Número máximo de alvos da habilidade " + 
                               std::to_string(skillId) + " excede o máximo permitido: " + 
                               std::to_string(rawData.MaxTarget) + " vs " + 
                               std::to_string(SkillLimits::MAX_SKILL_TARGETS));
        // Não retornar falso aqui para permitir carregar a habilidade mesmo com erro
    }
    
    return true;
}

bool SkillLoader::writeFileHeader(std::ofstream& stream) const {
    // Preparar o cabeçalho
    BIN_HEADER header;
    std::memcpy(header.Signature, SKILL_FILE_SIGNATURE, 4);
    header.Version = m_fileVersion;
    header.SkillCount = static_cast<uint32_t>(m_skills.size());
    std::memset(header.Reserved, 0, sizeof(header.Reserved));
    
    // Escrever o cabeçalho
    stream.write(reinterpret_cast<const char*>(&header), sizeof(BIN_HEADER));
    
    // Verificar se a operação foi bem-sucedida
    return stream.good();
}

bool SkillLoader::isValidSkillId(uint16_t skillId) const {
    // Verificar se o ID está dentro dos limites
    if (skillId == 0 || skillId > SkillLimits::MAX_SKILL_ID) {
        return false;
    }
    
    return true;
}

SkillTargetType SkillLoader::convertTargetValue(int32_t targetValue) const {
    switch (targetValue) {
        case 0: return SKILL_TARGET_NONE;
        case 1: return SKILL_TARGET_SELF;
        case 2: return SKILL_TARGET_ALLY;
        case 3: return SKILL_TARGET_ENEMY;
        case 4: return SKILL_TARGET_GROUND;
        case 5: return SKILL_TARGET_CORPSE;
        case 6: return SKILL_TARGET_ITEM;
        case 7: return SKILL_TARGET_PET;
        case 8: return SKILL_TARGET_GROUP;
        case 9: return SKILL_TARGET_GUILD;
        case 10: return SKILL_TARGET_NPC;
        case 11: return SKILL_TARGET_PARTY_MEMBER;
        case 12: return SKILL_TARGET_ALL;
        case 13: return SKILL_TARGET_MERCHANT;
        case 14: return SKILL_TARGET_NEUTRAL;
        case 15: return SKILL_TARGET_FRIENDLY_AREA;
        case 16: return SKILL_TARGET_ENEMY_AREA;
        case 17: return SKILL_TARGET_ALL_AREA;
        case 18: return SKILL_TARGET_PET_AREA;
        case 19: return SKILL_TARGET_SPECIAL;
        default: return SKILL_TARGET_UNKNOWN;
    }
}

SkillInstanceType SkillLoader::convertInstanceTypeValue(int32_t instanceTypeValue) const {
    switch (instanceTypeValue) {
        case 0: return SKILL_INSTANCE_NONE;
        case 1: return SKILL_INSTANCE_DAMAGE;
        case 2: return SKILL_INSTANCE_MAGIC_DAMAGE;
        case 3: return SKILL_INSTANCE_HEAL;
        case 4: return SKILL_INSTANCE_RESURRECT;
        case 5: return SKILL_INSTANCE_TELEPORT;
        case 6: return SKILL_INSTANCE_DISPEL;
        case 7: return SKILL_INSTANCE_SUMMON;
        case 8: return SKILL_INSTANCE_TRANSFORM;
        case 9: return SKILL_INSTANCE_MP_DAMAGE;
        case 10: return SKILL_INSTANCE_MP_HEAL;
        case 11: return SKILL_INSTANCE_STEAL_HP;
        case 12: return SKILL_INSTANCE_STEAL_MP;
        case 13: return SKILL_INSTANCE_KNOCKBACK;
        case 14: return SKILL_INSTANCE_PULL;
        case 15: return SKILL_INSTANCE_DASH;
        case 16: return SKILL_INSTANCE_IMMUNITY;
        case 17: return SKILL_INSTANCE_SPECIAL;
        default: return SKILL_INSTANCE_UNKNOWN;
    }
}

} // namespace skill
} // namespace wydbr
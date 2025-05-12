#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

class CombatSystemGenerator {
private:
    bool createCombatFormulas(const std::string& basePath) {
        // Garantir que o diretório exista
        std::string dirPath = basePath + "/tmsrv/combat";
        fs::create_directories(dirPath);
        
        // Criar o arquivo de cabeçalho CombatFormulas.h
        std::string headerPath = dirPath + "/CombatFormulas.h";
        std::ofstream headerFile(headerPath);
        
        if (!headerFile.is_open()) {
            std::cerr << "Falha ao criar arquivo: " << headerPath << std::endl;
            return false;
        }
        
        headerFile << R"(/**
 * @file CombatFormulas.h
 * @brief Define fórmulas avançadas de combate utilizadas pelo servidor
 */

#pragma once

#include <cmath>
#include <random>
#include <memory>
#include <vector>
#include <unordered_map>
#include "../../common/entities/Character.h"
#include "../../common/items/Equipment.h"

namespace wydbr {
namespace tmsrv {
namespace combat {

/**
 * @class CombatFormulas
 * @brief Implementa fórmulas avançadas para cálculos de combate
 * 
 * Esta classe fornece métodos para calcular dano, precisão, evasão e outros
 * aspectos de combate com base nos atributos dos personagens e equipamentos.
 */
class CombatFormulas {
public:
    /**
     * @brief Calcula o dano base de um ataque
     * @param attacker Atacante
     * @param defender Defensor
     * @param skillPower Poder da habilidade (se aplicável)
     * @param weaponType Tipo de arma
     * @return Valor de dano calculado
     */
    static int calculateBaseDamage(
        const std::shared_ptr<Character>& attacker,
        const std::shared_ptr<Character>& defender,
        int skillPower = 0, 
        WeaponType weaponType = WeaponType::NONE
    );
    
    /**
     * @brief Calcula a chance de acerto crítico 
     * @param attacker Atacante
     * @param defender Defensor
     * @param skillModifier Modificador da habilidade
     * @return Probabilidade de acerto crítico (0.0-1.0)
     */
    static float calculateCriticalChance(
        const std::shared_ptr<Character>& attacker,
        const std::shared_ptr<Character>& defender,
        float skillModifier = 0.0f
    );
    
    /**
     * @brief Calcula multiplicador de dano crítico
     * @param attacker Atacante
     * @param weaponType Tipo de arma
     * @param classType Tipo de classe
     * @return Multiplicador de dano crítico
     */
    static float calculateCriticalMultiplier(
        const std::shared_ptr<Character>& attacker,
        WeaponType weaponType,
        ClassType classType
    );
    
    /**
     * @brief Calcula a taxa de bloqueio com escudo
     * @param defender Defensor
     * @param shieldType Tipo de escudo
     * @return Probabilidade de bloqueio (0.0-1.0)
     */
    static float calculateBlockRate(
        const std::shared_ptr<Character>& defender,
        ShieldType shieldType
    );
    
    /**
     * @brief Calcula a quantidade de dano bloqueado
     * @param defender Defensor
     * @param incomingDamage Dano recebido
     * @param shieldType Tipo de escudo
     * @return Quantidade de dano bloqueado
     */
    static int calculateBlockedDamage(
        const std::shared_ptr<Character>& defender,
        int incomingDamage,
        ShieldType shieldType
    );
    
    /**
     * @brief Calcula dano baseado em vulnerabilidades elementais
     * @param defender Defensor
     * @param baseDamage Dano base
     * @param elementType Tipo de elemento do ataque
     * @return Dano modificado por vulnerabilidades
     */
    static int calculateElementalDamage(
        const std::shared_ptr<Character>& defender,
        int baseDamage,
        ElementType elementType
    );
    
    /**
     * @brief Randomiza valores de dano para variedade
     * @param baseDamage Dano base calculado
     * @param varianceFactor Fator de variância (padrão 0.1 = ±10%)
     * @return Dano final randomizado
     */
    static int randomizeDamage(int baseDamage, float varianceFactor = 0.1f);
    
private:
    // Gerador de números aleatórios thread-safe
    static thread_local std::mt19937 rng;
    
    // Tabelas de coeficientes para cada classe
    static const std::unordered_map<ClassType, float> CLASS_STR_COEFFICIENTS;
    static const std::unordered_map<ClassType, float> CLASS_DEX_COEFFICIENTS;
    static const std::unordered_map<ClassType, float> CLASS_INT_COEFFICIENTS;
    
    // Inicializa o RNG com uma seed adequada
    static void initializeRNG();
};

} // namespace combat
} // namespace tmsrv
} // namespace wydbr
)";
        headerFile.close();
        
        // Criar o arquivo de implementação CombatFormulas.cpp
        std::string implPath = dirPath + "/CombatFormulas.cpp";
        std::ofstream implFile(implPath);
        
        if (!implFile.is_open()) {
            std::cerr << "Falha ao criar arquivo: " << implPath << std::endl;
            return false;
        }
        
        implFile << R"(/**
 * @file CombatFormulas.cpp
 * @brief Implementação das fórmulas avançadas de combate
 */

#include "CombatFormulas.h"
#include <chrono>
#include <algorithm>
#include "../../common/utils/MathUtils.h"

namespace wydbr {
namespace tmsrv {
namespace combat {

// Inicialização das variáveis estáticas
thread_local std::mt19937 CombatFormulas::rng;

// Coeficientes por classe para STR
const std::unordered_map<ClassType, float> CombatFormulas::CLASS_STR_COEFFICIENTS = {
    {ClassType::TRANSC,    1.5f},
    {ClassType::FOEMA,     1.2f},
    {ClassType::BM,        0.9f},
    {ClassType::HUNTER,    1.1f},
    {ClassType::PSTWO,     1.3f},
    {ClassType::ARCH,      0.8f},
    {ClassType::CELESTIAL, 1.0f}
};

// Coeficientes por classe para DEX
const std::unordered_map<ClassType, float> CombatFormulas::CLASS_DEX_COEFFICIENTS = {
    {ClassType::TRANSC,    0.8f},
    {ClassType::FOEMA,     1.0f},
    {ClassType::BM,        1.2f},
    {ClassType::HUNTER,    1.5f},
    {ClassType::PSTWO,     1.1f},
    {ClassType::ARCH,      1.4f},
    {ClassType::CELESTIAL, 1.0f}
};

// Coeficientes por classe para INT
const std::unordered_map<ClassType, float> CombatFormulas::CLASS_INT_COEFFICIENTS = {
    {ClassType::TRANSC,    0.7f},
    {ClassType::FOEMA,     1.3f},
    {ClassType::BM,        1.6f},
    {ClassType::HUNTER,    0.8f},
    {ClassType::PSTWO,     0.7f},
    {ClassType::ARCH,      1.1f},
    {ClassType::CELESTIAL, 1.5f}
};

void CombatFormulas::initializeRNG() {
    // Inicializa o gerador com uma seed baseada no tempo atual
    // Isso garante uma distribuição diferente a cada execução
    auto seed = static_cast<unsigned int>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
    rng.seed(seed);
}

int CombatFormulas::calculateBaseDamage(
    const std::shared_ptr<Character>& attacker,
    const std::shared_ptr<Character>& defender,
    int skillPower,
    WeaponType weaponType
) {
    // Verifica se os parâmetros são válidos
    if (!attacker || !defender) {
        return 0;
    }
    
    const auto& stats = attacker->getStats();
    const auto classType = attacker->getClassType();
    
    // Recupera os coeficientes específicos da classe
    float strCoef = CLASS_STR_COEFFICIENTS.at(classType);
    float dexCoef = CLASS_DEX_COEFFICIENTS.at(classType);
    float intCoef = CLASS_INT_COEFFICIENTS.at(classType);
    
    // Calcula o dano base com base nos atributos
    float baseDamage = 0.0f;
    
    // Diferentes fórmulas para diferentes tipos de arma
    switch (weaponType) {
        case WeaponType::SWORD:
        case WeaponType::AXE:
        case WeaponType::SPEAR:
            // Armas corpo a corpo - maior peso em STR
            baseDamage = (stats.strength * strCoef * 2.5f) + 
                         (stats.dexterity * dexCoef * 0.8f) + 
                         (stats.intelligence * intCoef * 0.2f);
            break;
            
        case WeaponType::BOW:
        case WeaponType::CROSSBOW:
            // Armas de longa distância - maior peso em DEX
            baseDamage = (stats.strength * strCoef * 0.8f) + 
                         (stats.dexterity * dexCoef * 2.5f) + 
                         (stats.intelligence * intCoef * 0.4f);
            break;
            
        case WeaponType::STAFF:
        case WeaponType::WAND:
            // Armas mágicas - maior peso em INT
            baseDamage = (stats.strength * strCoef * 0.2f) + 
                         (stats.dexterity * dexCoef * 0.6f) + 
                         (stats.intelligence * intCoef * 3.0f);
            break;
            
        default:
            // Sem arma ou caso não tratado
            baseDamage = (stats.strength * strCoef * 1.0f) + 
                         (stats.dexterity * dexCoef * 1.0f) + 
                         (stats.intelligence * intCoef * 1.0f);
            break;
    }
    
    // Adiciona poder da habilidade
    baseDamage += skillPower;
    
    // Adiciona bônus de arma e refinamento
    const auto& weapon = attacker->getEquipment().getWeapon();
    if (weapon) {
        baseDamage += weapon->getBaseDamage();
        baseDamage += weapon->getRefineLevel() * weapon->getRefineDamageBonus();
    }
    
    // Reduz com base na defesa do defensor
    float defense = defender->getStats().defense + 
                   defender->getEquipment().calculateTotalDefense();
    
    // Fórmula de penetração de defesa
    float damageFinal = baseDamage * (1000.0f / (1000.0f + defense));
    
    return static_cast<int>(std::max(1.0f, damageFinal));
}

float CombatFormulas::calculateCriticalChance(
    const std::shared_ptr<Character>& attacker,
    const std::shared_ptr<Character>& defender,
    float skillModifier
) {
    if (!attacker || !defender) {
        return 0.0f;
    }
    
    const auto& atkStats = attacker->getStats();
    const auto& defStats = defender->getStats();
    
    // Base de chance crítica depende primariamente da destreza
    float baseCritical = atkStats.dexterity * 0.2f + atkStats.luck * 0.5f;
    
    // Ajuste baseado em diferença de nível
    float levelDiff = static_cast<float>(attacker->getLevel() - defender->getLevel());
    float levelAdjust = std::max(-5.0f, std::min(5.0f, levelDiff * 0.5f));
    
    // Redução baseada em defesa e sorte do defensor
    float reduction = defStats.luck * 0.3f;
    
    // Aplica o modificador de habilidade
    float finalChance = (baseCritical + levelAdjust - reduction) * (1.0f + skillModifier);
    
    // Limita entre 5% e 40%
    return MathUtils::clamp(finalChance / 100.0f, 0.05f, 0.40f);
}

float CombatFormulas::calculateCriticalMultiplier(
    const std::shared_ptr<Character>& attacker,
    WeaponType weaponType,
    ClassType classType
) {
    if (!attacker) {
        return 1.5f; // Multiplicador padrão
    }
    
    // Base começa em 1.5x
    float baseMultiplier = 1.5f;
    
    // Bônus baseado no tipo de arma
    switch (weaponType) {
        case WeaponType::SWORD:
            baseMultiplier += 0.3f;
            break;
        case WeaponType::AXE:
            baseMultiplier += 0.5f;
            break;
        case WeaponType::BOW:
        case WeaponType::CROSSBOW:
            baseMultiplier += 0.4f;
            break;
        case WeaponType::STAFF:
        case WeaponType::WAND:
            baseMultiplier += 0.2f;
            break;
        default:
            break;
    }
    
    // Bônus baseado na classe
    switch (classType) {
        case ClassType::HUNTER:
        case ClassType::PSTWO:
            baseMultiplier += 0.3f;
            break;
        case ClassType::BM:
            baseMultiplier += 0.1f;
            break;
        case ClassType::TRANSC:
            baseMultiplier += 0.4f;
            break;
        default:
            break;
    }
    
    // Influência de atributos
    const auto& stats = attacker->getStats();
    float attributeBonus = stats.strength * 0.001f + stats.dexterity * 0.002f;
    
    return baseMultiplier + attributeBonus;
}

float CombatFormulas::calculateBlockRate(
    const std::shared_ptr<Character>& defender,
    ShieldType shieldType
) {
    if (!defender) {
        return 0.0f;
    }
    
    // Taxa de bloqueio base depende do tipo de escudo
    float baseBlockRate = 0.0f;
    switch (shieldType) {
        case ShieldType::BUCKLER:
            baseBlockRate = 0.15f;
            break;
        case ShieldType::KITE:
            baseBlockRate = 0.25f;
            break;
        case ShieldType::TOWER:
            baseBlockRate = 0.35f;
            break;
        default:
            return 0.0f; // Sem escudo não há bloqueio
    }
    
    // Ajustes baseados em atributos
    const auto& stats = defender->getStats();
    float attributeModifier = stats.strength * 0.001f + stats.dexterity * 0.002f;
    
    // Bônus de habilidades passivas
    float passiveBonus = 0.0f;
    // TODO: Implementar bônus de habilidades passivas
    
    // Taxa de bloqueio final
    float finalBlockRate = baseBlockRate + attributeModifier + passiveBonus;
    
    // Limita entre 0% e 70%
    return MathUtils::clamp(finalBlockRate, 0.0f, 0.7f);
}

int CombatFormulas::calculateBlockedDamage(
    const std::shared_ptr<Character>& defender,
    int incomingDamage,
    ShieldType shieldType
) {
    if (!defender || incomingDamage <= 0) {
        return 0;
    }
    
    // Percentual de redução por tipo de escudo
    float reductionPercent = 0.0f;
    switch (shieldType) {
        case ShieldType::BUCKLER:
            reductionPercent = 0.3f;
            break;
        case ShieldType::KITE:
            reductionPercent = 0.5f;
            break;
        case ShieldType::TOWER:
            reductionPercent = 0.7f;
            break;
        default:
            return 0; // Sem escudo, não bloqueia dano
    }
    
    // Bônus baseado em atributos
    const auto& stats = defender->getStats();
    float attributeBonus = stats.strength * 0.002f;
    
    // Calcula o dano bloqueado
    float totalReduction = reductionPercent + attributeBonus;
    totalReduction = MathUtils::clamp(totalReduction, 0.0f, 0.9f);
    
    return static_cast<int>(incomingDamage * totalReduction);
}

int CombatFormulas::calculateElementalDamage(
    const std::shared_ptr<Character>& defender,
    int baseDamage,
    ElementType elementType
) {
    if (!defender || baseDamage <= 0) {
        return baseDamage;
    }
    
    // Tabela de efetividade elemental (multiplicadores)
    static const std::unordered_map<ElementType, std::unordered_map<ElementType, float>> ELEMENTAL_TABLE = {
        {ElementType::FIRE, {
            {ElementType::FIRE,     1.0f},
            {ElementType::WATER,    0.5f},
            {ElementType::EARTH,    1.5f},
            {ElementType::WIND,     1.0f},
            {ElementType::LIGHT,    1.0f},
            {ElementType::DARK,     1.0f},
            {ElementType::NEUTRAL,  1.0f}
        }},
        {ElementType::WATER, {
            {ElementType::FIRE,     1.5f},
            {ElementType::WATER,    1.0f},
            {ElementType::EARTH,    0.5f},
            {ElementType::WIND,     1.0f},
            {ElementType::LIGHT,    1.0f},
            {ElementType::DARK,     1.0f},
            {ElementType::NEUTRAL,  1.0f}
        }},
        {ElementType::EARTH, {
            {ElementType::FIRE,     0.5f},
            {ElementType::WATER,    1.5f},
            {ElementType::EARTH,    1.0f},
            {ElementType::WIND,     0.5f},
            {ElementType::LIGHT,    1.0f},
            {ElementType::DARK,     1.0f},
            {ElementType::NEUTRAL,  1.0f}
        }},
        {ElementType::WIND, {
            {ElementType::FIRE,     1.0f},
            {ElementType::WATER,    1.0f},
            {ElementType::EARTH,    1.5f},
            {ElementType::WIND,     1.0f},
            {ElementType::LIGHT,    1.0f},
            {ElementType::DARK,     1.0f},
            {ElementType::NEUTRAL,  1.0f}
        }},
        {ElementType::LIGHT, {
            {ElementType::FIRE,     1.0f},
            {ElementType::WATER,    1.0f},
            {ElementType::EARTH,    1.0f},
            {ElementType::WIND,     1.0f},
            {ElementType::LIGHT,    1.0f},
            {ElementType::DARK,     1.5f},
            {ElementType::NEUTRAL,  1.0f}
        }},
        {ElementType::DARK, {
            {ElementType::FIRE,     1.0f},
            {ElementType::WATER,    1.0f},
            {ElementType::EARTH,    1.0f},
            {ElementType::WIND,     1.0f},
            {ElementType::LIGHT,    1.5f},
            {ElementType::DARK,     1.0f},
            {ElementType::NEUTRAL,  1.0f}
        }},
        {ElementType::NEUTRAL, {
            {ElementType::FIRE,     1.0f},
            {ElementType::WATER,    1.0f},
            {ElementType::EARTH,    1.0f},
            {ElementType::WIND,     1.0f},
            {ElementType::LIGHT,    1.0f},
            {ElementType::DARK,     1.0f},
            {ElementType::NEUTRAL,  1.0f}
        }}
    };
    
    // Obtém o elemento do defensor
    ElementType defenderElement = defender->getElementType();
    
    // Aplica o multiplicador elemental
    float multiplier = ELEMENTAL_TABLE.at(elementType).at(defenderElement);
    
    // Resistência elemental do defensor (%)
    float resistance = defender->getElementalResistance(elementType) / 100.0f;
    resistance = MathUtils::clamp(resistance, 0.0f, 0.8f); // Máximo 80% de resistência
    
    // Aplica a resistência
    multiplier = multiplier * (1.0f - resistance);
    
    return static_cast<int>(baseDamage * multiplier);
}

int CombatFormulas::randomizeDamage(int baseDamage, float varianceFactor) {
    // Inicializa o RNG se necessário
    if (rng.min() == rng.max()) {
        initializeRNG();
    }
    
    // Defina os limites de variação
    float minFactor = 1.0f - varianceFactor;
    float maxFactor = 1.0f + varianceFactor;
    
    // Cria uma distribuição uniforme
    std::uniform_real_distribution<float> dist(minFactor, maxFactor);
    
    // Aplica o fator aleatório
    float randomFactor = dist(rng);
    
    // Garante que o dano nunca seja menor que 1
    return std::max(1, static_cast<int>(baseDamage * randomFactor));
}

} // namespace combat
} // namespace tmsrv
} // namespace wydbr
)";
        implFile.close();
        
        return true;
    }
    
    bool createDamageProcessor(const std::string& basePath) {
        // Implementação similar à anterior, criando os arquivos DamageProcessor.h e DamageProcessor.cpp
        return true; // Implementar conforme necessário
    }
    
    bool createSkillEffectManager(const std::string& basePath) {
        // Implementação similar à anterior, criando os arquivos SkillEffectManager.h e SkillEffectManager.cpp
        return true; // Implementar conforme necessário
    }
    
    bool createComboSystem(const std::string& basePath) {
        // Implementação similar à anterior, criando os arquivos ComboSystem.h e ComboSystem.cpp
        return true; // Implementar conforme necessário
    }
    
public:
    bool generate(const std::string& basePath) {
        std::cout << "Gerando Sistema de Combate Avançado..." << std::endl;
        
        bool success = true;
        
        // Criar os componentes do sistema
        success &= createCombatFormulas(basePath);
        success &= createDamageProcessor(basePath);
        success &= createSkillEffectManager(basePath);
        success &= createComboSystem(basePath);
        
        return success;
    }
};

int main(int argc, char* argv[]) {
    std::string basePath = (argc > 1) ? argv[1] : "../src";
    
    CombatSystemGenerator generator;
    bool success = generator.generate(basePath);
    
    return success ? 0 : 1;
} 
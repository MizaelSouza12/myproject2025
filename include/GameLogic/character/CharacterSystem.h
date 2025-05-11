/**
 * @file CharacterSystem.h
 * @brief Sistema de personagens do WYDBR 2.0
 */

#ifndef WYDBR_CHARACTER_SYSTEM_H
#define WYDBR_CHARACTER_SYSTEM_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "wyd_core.h"
#include "mob_struct.h"

namespace WYDBR {

/**
 * @class CharacterSystem
 * @brief Sistema de personagens do WYDBR 2.0
 * 
 * Este sistema gerencia todos os personagens do jogo, incluindo:
 * - Criação e remoção de personagens
 * - Atualização de status
 * - Gerenciamento de inventário
 * - Gerenciamento de habilidades
 * - Gerenciamento de equipamentos
 */
class CharacterSystem {
public:
    /**
     * @brief Obtém a instância única do sistema de personagens
     * @return Referência para a instância do sistema de personagens
     */
    static CharacterSystem& GetInstance();
    
    /**
     * @brief Inicializa o sistema de personagens
     */
    void Initialize();
    
    /**
     * @brief Finaliza o sistema de personagens
     */
    void Shutdown();
    
    /**
     * @brief Cria um novo personagem
     * @param name Nome do personagem
     * @param classType Classe do personagem
     * @return ID do personagem criado
     */
    uint32_t CreateCharacter(const std::string& name, CharacterClass classType);
    
    /**
     * @brief Remove um personagem
     * @param characterId ID do personagem
     */
    void RemoveCharacter(uint32_t characterId);
    
    /**
     * @brief Obtém um personagem
     * @param characterId ID do personagem
     * @return Ponteiro para o personagem
     */
    Mob* GetCharacter(uint32_t characterId);
    
    /**
     * @brief Obtém um personagem (versão constante)
     * @param characterId ID do personagem
     * @return Ponteiro constante para o personagem
     */
    const Mob* GetCharacter(uint32_t characterId) const;
    
    /**
     * @brief Atualiza o status de um personagem
     * @param characterId ID do personagem
     * @param deltaTime Tempo desde a última atualização
     */
    void UpdateCharacter(uint32_t characterId, float deltaTime);
    
    /**
     * @brief Aplica dano a um personagem
     * @param characterId ID do personagem
     * @param damage Quantidade de dano
     * @return true se o personagem morreu, false caso contrário
     */
    bool ApplyDamage(uint32_t characterId, uint32_t damage);
    
    /**
     * @brief Aplica cura a um personagem
     * @param characterId ID do personagem
     * @param amount Quantidade de cura
     */
    void ApplyHeal(uint32_t characterId, uint32_t amount);
    
    /**
     * @brief Aplica um status a um personagem
     * @param characterId ID do personagem
     * @param status Status a ser aplicado
     * @param duration Duração do status em segundos
     */
    void ApplyStatus(uint32_t characterId, uint32_t status, float duration);
    
    /**
     * @brief Remove um status de um personagem
     * @param characterId ID do personagem
     * @param status Status a ser removido
     */
    void RemoveStatus(uint32_t characterId, uint32_t status);
    
    /**
     * @brief Verifica se um personagem tem um status
     * @param characterId ID do personagem
     * @param status Status a ser verificado
     * @return true se o personagem tem o status, false caso contrário
     */
    bool HasStatus(uint32_t characterId, uint32_t status) const;
    
    /**
     * @brief Atualiza a posição de um personagem
     * @param characterId ID do personagem
     * @param position Nova posição
     */
    void UpdatePosition(uint32_t characterId, const Position& position);
    
    /**
     * @brief Obtém a posição de um personagem
     * @param characterId ID do personagem
     * @return Posição do personagem
     */
    Position GetPosition(uint32_t characterId) const;
    
    /**
     * @brief Verifica se um personagem está vivo
     * @param characterId ID do personagem
     * @return true se o personagem está vivo, false caso contrário
     */
    bool IsAlive(uint32_t characterId) const;
    
    /**
     * @brief Verifica se um personagem pode usar uma habilidade
     * @param characterId ID do personagem
     * @param skillId ID da habilidade
     * @return true se o personagem pode usar a habilidade, false caso contrário
     */
    bool CanUseSkill(uint32_t characterId, uint16_t skillId) const;
    
    /**
     * @brief Usa uma habilidade
     * @param characterId ID do personagem
     * @param skillId ID da habilidade
     * @return true se a habilidade foi usada com sucesso, false caso contrário
     */
    bool UseSkill(uint32_t characterId, uint16_t skillId);

    // Atributos e status
    bool AddExperience(uint32_t characterId, uint32_t amount);
    bool AddGold(uint32_t characterId, uint32_t amount);
    bool UpdateStats(uint32_t characterId);
    bool LevelUp(uint32_t characterId);

    // Equipamentos
    bool EquipItem(uint32_t characterId, uint32_t itemId, uint8_t slot);
    bool UnequipItem(uint32_t characterId, uint8_t slot);
    bool IsItemEquipped(uint32_t characterId, uint32_t itemId) const;

    // Skills
    bool LearnSkill(uint32_t characterId, uint16_t skillId);
    bool UpgradeSkill(uint32_t characterId, uint16_t skillId);

    // Queries
    bool IsCharacterOnline(uint32_t characterId) const;
    std::vector<uint32_t> GetOnlineCharacters() const;

private:
    CharacterSystem() = default;
    ~CharacterSystem() = default;

    // Prevenir cópias
    CharacterSystem(const CharacterSystem&) = delete;
    CharacterSystem& operator=(const CharacterSystem&) = delete;

    // Cálculos internos
    uint32_t CalculateNextLevelExp(uint8_t level) const;
    void CalculateBaseStats(Mob& character) const;
    void ApplyEquipmentStats(Mob& character) const;
    void ApplySkillStats(Mob& character) const;

    // Estado
    std::unordered_map<uint32_t, std::unique_ptr<Mob>> m_characters;
    std::unordered_map<uint32_t, bool> m_onlineStatus;

    // Próximo ID disponível
    uint32_t nextCharacterId = 1;
    
    // Constantes
    static constexpr float STATUS_UPDATE_INTERVAL = 1.0f;  ///< Intervalo de atualização de status em segundos
    static constexpr float REGEN_INTERVAL = 5.0f;          ///< Intervalo de regeneração em segundos
    static constexpr float HP_REGEN_RATE = 0.01f;          ///< Taxa de regeneração de HP
    static constexpr float MP_REGEN_RATE = 0.02f;          ///< Taxa de regeneração de MP
};

} // namespace WYDBR

#endif // WYDBR_CHARACTER_SYSTEM_H 
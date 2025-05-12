/**
 * WYDPlayer.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/player/WYDPlayer.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYD_PLAYER_H
#define WYD_PLAYER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>

#include "headers/wyd_core.h"
#include "src/server/tmsrv/player/WYDInventory.h"
#include "src/server/tmsrv/player/WYDSkillManager.h"

namespace wydbr {


class WorldManager;
class PlayerManager;

/**
 * Representa um jogador no mundo do jogo
 */
/**
 * Classe WYDPlayer
 * 
 * Esta classe implementa a funcionalidade WYDPlayer conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDPlayer
 * 
 * Esta classe implementa a funcionalidade WYDPlayer conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDPlayer : public WYDObject {
public:
    // /**
 * WYDPlayer
 * 
 * Implementa a funcionalidade WYDPlayer conforme especificação original.
 * @param id Parâmetro id
 * @param name Parâmetro name
 * @param playerManager Parâmetro playerManager
 * @return Retorna Construtor
 */
 Construtor
    WYDPlayer(uint32_t id, const std::string& name, PlayerManager* playerManager);
    
    // Destrutor
    virtual ~WYDPlayer();
    
    // Carrega dados do jogador do banco de dados
    /**
 * loadFromDatabase
 * 
 * Implementa a funcionalidade loadFromDatabase conforme especificação original.
 * @param account Parâmetro account
 * @return Retorna bool
 */

    bool loadFromDatabase(const std::string& account);
    
    // Cria um novo jogador
    /**
 * createNew
 * 
 * Implementa a funcionalidade createNew conforme especificação original.
 * @param charClass Parâmetro charClass
 * @return Retorna bool
 */

    bool createNew(wyd::CharacterClass charClass);
    
    // Salva dados do jogador no banco de dados
    /**
 * saveToDatabase
 * 
 * Implementa a funcionalidade saveToDatabase conforme especificação original.
 * @return Retorna bool
 */

    bool saveToDatabase();
    
    // Getters básicos
    const std::string& getAccount() const { return account_; }
    const std::string& getName() const { return name_; }
    /**
 * getCharClass
 * 
 * Implementa a funcionalidade getCharClass conforme especificação original.
 * @return Retorna wyd::CharacterClass
 */

    wyd::CharacterClass getCharClass() const{ return characterClass_; }
    /**
 * getLevel
 * 
 * Implementa a funcionalidade getLevel conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getLevel() const{ return level_; }
    const std::string& getGuild() const { return guildName_; }
    /**
 * getGuildId
 * 
 * Implementa a funcionalidade getGuildId conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getGuildId() const{ return guildId_; }
    /**
 * getStrength
 * 
 * Implementa a funcionalidade getStrength conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getStrength() const{ return strength_; }
    /**
 * getDexterity
 * 
 * Implementa a funcionalidade getDexterity conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getDexterity() const{ return dexterity_; }
    /**
 * getIntelligence
 * 
 * Implementa a funcionalidade getIntelligence conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getIntelligence() const{ return intelligence_; }
    /**
 * getConstitution
 * 
 * Implementa a funcionalidade getConstitution conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getConstitution() const{ return constitution_; }
    /**
 * getStatPoints
 * 
 * Implementa a funcionalidade getStatPoints conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getStatPoints() const{ return statPoints_; }
    /**
 * getSkillPoints
 * 
 * Implementa a funcionalidade getSkillPoints conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getSkillPoints() const{ return skillPoints_; }
    /**
 * getMaxHP
 * 
 * Implementa a funcionalidade getMaxHP conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getMaxHP() const{ return maxHP_; }
    /**
 * getHP
 * 
 * Implementa a funcionalidade getHP conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getHP() const{ return hp_; }
    /**
 * getMaxMP
 * 
 * Implementa a funcionalidade getMaxMP conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getMaxMP() const{ return maxMP_; }
    /**
 * getMP
 * 
 * Implementa a funcionalidade getMP conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getMP() const{ return mp_; }
    /**
 * getExperience
 * 
 * Implementa a funcionalidade getExperience conforme especificação original.
 * @return Retorna uint64_t
 */

    uint64_t getExperience() const{ return experience_; }
    /**
 * getNextLevelExp
 * 
 * Implementa a funcionalidade getNextLevelExp conforme especificação original.
 * @return Retorna uint64_t
 */

    uint64_t getNextLevelExp() const;
    /**
 * getGold
 * 
 * Implementa a funcionalidade getGold conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getGold() const{ return gold_; }
    /**
 * getBankGold
 * 
 * Implementa a funcionalidade getBankGold conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getBankGold() const{ return bankGold_; }
    
    // Setters controlados
    /**
 * setAccount
 * 
 * Implementa a funcionalidade setAccount conforme especificação original.
 * @param account Parâmetro account
 */

    void setAccount(const std::string& account){ account_ = account; }
    /**
 * setLevel
 * 
 * Implementa a funcionalidade setLevel conforme especificação original.
 * @param level Parâmetro level
 */

    void setLevel(uint16_t level);
    /**
 * setGuild
 * 
 * Implementa a funcionalidade setGuild conforme especificação original.
 * @param guildName Parâmetro guildName
 * @param guildId Parâmetro guildId
 */

    void setGuild(const std::string& guildName, uint32_t guildId);
    /**
 * setStatPoints
 * 
 * Implementa a funcionalidade setStatPoints conforme especificação original.
 * @param points Parâmetro points
 */

    void setStatPoints(uint16_t points){ statPoints_ = points; }
    /**
 * setSkillPoints
 * 
 * Implementa a funcionalidade setSkillPoints conforme especificação original.
 * @param points Parâmetro points
 */

    void setSkillPoints(uint16_t points){ skillPoints_ = points; }
    /**
 * setHP
 * 
 * Implementa a funcionalidade setHP conforme especificação original.
 * @param hp Parâmetro hp
 */

    void setHP(uint32_t hp);
    /**
 * setMP
 * 
 * Implementa a funcionalidade setMP conforme especificação original.
 * @param mp Parâmetro mp
 */

    void setMP(uint32_t mp);
    /**
 * setExperience
 * 
 * Implementa a funcionalidade setExperience conforme especificação original.
 * @param exp Parâmetro exp
 */

    void setExperience(uint64_t exp);
    /**
 * addExperience
 * 
 * Implementa a funcionalidade addExperience conforme especificação original.
 * @param exp Parâmetro exp
 */

    void addExperience(uint64_t exp);
    /**
 * setGold
 * 
 * Implementa a funcionalidade setGold conforme especificação original.
 * @param gold Parâmetro gold
 */

    void setGold(uint32_t gold){ gold_ = gold; }
    /**
 * setBankGold
 * 
 * Implementa a funcionalidade setBankGold conforme especificação original.
 * @param gold Parâmetro gold
 */

    void setBankGold(uint32_t gold){ bankGold_ = gold; }
    
    // Posição no mundo
    const std::string& getMapId() const { return mapId_; }
    /**
 * getPosX
 * 
 * Implementa a funcionalidade getPosX conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getPosX() const{ return posX_; }
    /**
 * getPosY
 * 
 * Implementa a funcionalidade getPosY conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getPosY() const{ return posY_; }
    /**
 * getDirection
 * 
 * Implementa a funcionalidade getDirection conforme especificação original.
 * @return Retorna uint8_t
 */

    uint8_t getDirection() const{ return direction_; }
    
    // Modifica posição
    /**
 * teleport
 * 
 * Implementa a funcionalidade teleport conforme especificação original.
 * @param mapId Parâmetro mapId
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool teleport(const std::string& mapId, uint16_t x, uint16_t y);
    /**
 * move
 * 
 * Implementa a funcionalidade move conforme especificação original.
 * @param newX Parâmetro newX
 * @param newY Parâmetro newY
 * @param direction Parâmetro direction
 * @return Retorna bool
 */

    bool move(uint16_t newX, uint16_t newY, uint8_t direction);
    
    // Atributos
    /**
 * increaseStrength
 * 
 * Implementa a funcionalidade increaseStrength conforme especificação original.
 * @param amount Parâmetro amount
 */

    void increaseStrength(uint16_t amount);
    /**
 * increaseDexterity
 * 
 * Implementa a funcionalidade increaseDexterity conforme especificação original.
 * @param amount Parâmetro amount
 */

    void increaseDexterity(uint16_t amount);
    /**
 * increaseIntelligence
 * 
 * Implementa a funcionalidade increaseIntelligence conforme especificação original.
 * @param amount Parâmetro amount
 */

    void increaseIntelligence(uint16_t amount);
    /**
 * increaseConstitution
 * 
 * Implementa a funcionalidade increaseConstitution conforme especificação original.
 * @param amount Parâmetro amount
 */

    void increaseConstitution(uint16_t amount);
    
    // Inventário
    /**
 * getInventory
 * 
 * Implementa a funcionalidade getInventory conforme especificação original.
 * @return Retorna WYDInventory
 */

    WYDInventory* getInventory(){ return inventory_.get(); }
    
    // Habilidades
    /**
 * getSkillManager
 * 
 * Implementa a funcionalidade getSkillManager conforme especificação original.
 * @return Retorna WYDSkillManager
 */

    WYDSkillManager* getSkillManager(){ return skillManager_.get(); }
    
    // Status
    /**
 * isDead
 * 
 * Implementa a funcionalidade isDead conforme especificação original.
 * @return Retorna bool
 */

    bool isDead() const{ return hp_ == 0; }
    /**
 * isInCombat
 * 
 * Implementa a funcionalidade isInCombat conforme especificação original.
 * @return Retorna bool
 */

    bool isInCombat() const{ return combatEndTime_ > std::time(nullptr); }
    /**
 * isTrading
 * 
 * Implementa a funcionalidade isTrading conforme especificação original.
 * @return Retorna bool
 */

    bool isTrading() const{ return isTrading_; }
    
    // Combate
    /**
 * enterCombat
 * 
 * Implementa a funcionalidade enterCombat conforme especificação original.
 * @param 10 Parâmetro 10
 */

    void enterCombat(uint32_t durationSeconds = 10);
    /**
 * exitCombat
 * 
 * Implementa a funcionalidade exitCombat conforme especificação original.
 */

    void exitCombat();
    /**
 * attack
 * 
 * Implementa a funcionalidade attack conforme especificação original.
 * @param targetId Parâmetro targetId
 * @return Retorna bool
 */

    bool attack(uint32_t targetId);
    /**
 * takeDamage
 * 
 * Implementa a funcionalidade takeDamage conforme especificação original.
 * @param amount Parâmetro amount
 * @param 0 Parâmetro 0
 */

    void takeDamage(uint32_t amount, uint32_t sourceId = 0);
    /**
 * heal
 * 
 * Implementa a funcionalidade heal conforme especificação original.
 * @param amount Parâmetro amount
 * @param 0 Parâmetro 0
 */

    void heal(uint32_t amount, uint32_t sourceId = 0);
    /**
 * die
 * 
 * Implementa a funcionalidade die conforme especificação original.
 */

    void die();
    /**
 * resurrect
 * 
 * Implementa a funcionalidade resurrect conforme especificação original.
 * @param 100 Parâmetro 100
 */

    void resurrect(uint8_t percentHP = 100);
    
    // Comércio
    /**
 * startTrading
 * 
 * Implementa a funcionalidade startTrading conforme especificação original.
 * @param partnerId Parâmetro partnerId
 * @return Retorna bool
 */

    bool startTrading(uint32_t partnerId);
    /**
 * stopTrading
 * 
 * Implementa a funcionalidade stopTrading conforme especificação original.
 * @param false Parâmetro false
 * @return Retorna bool
 */

    bool stopTrading(bool success = false);
    
    // Serialização/desserialização
    std::vector<uint8_t> serialize() const override;
    /**
 * deserialize
 * 
 * Implementa a funcionalidade deserialize conforme especificação original.
 * @param data Parâmetro data
 */

    void deserialize(const std::vector<uint8_t>& data) override;
    
    // Métodos utilitários
    /**
 * recalculateStats
 * 
 * Implementa a funcionalidade recalculateStats conforme especificação original.
 */

    void recalculateStats();
    /**
 * update
 * 
 * Implementa a funcionalidade update conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void update(uint64_t timestamp);
    /**
 * sendMessage
 * 
 * Implementa a funcionalidade sendMessage conforme especificação original.
 * @param message Parâmetro message
 */

    void sendMessage(const std::string& message);
    /**
 * sendSystemMessage
 * 
 * Implementa a funcionalidade sendSystemMessage conforme especificação original.
 * @param message Parâmetro message
 */

    void sendSystemMessage(const std::string& message);
    
    // Métodos para rede
    /**
 * sendPacket
 * 
 * Implementa a funcionalidade sendPacket conforme especificação original.
 * @param packet Parâmetro packet
 */

    void sendPacket(const WYDPacket& packet);
    
private:
    // Conta
    std::string account_;
    
    // Classificação
    wyd::CharacterClass characterClass_;
    
    // Guilda
    std::string guildName_;
    uint32_t guildId_;
    
    // Posição
    std::string mapId_;
    uint16_t posX_;
    uint16_t posY_;
    uint8_t direction_;
    
    // Atributos primários
    uint16_t strength_;
    uint16_t dexterity_;
    uint16_t intelligence_;
    uint16_t constitution_;
    uint16_t statPoints_;
    
    // Atributos derivados (calculados)
    uint16_t attack_;
    uint16_t defense_;
    uint16_t magicAttack_;
    uint16_t magicDefense_;
    uint16_t accuracy_;
    uint16_t dodge_;
    uint16_t critical_;
    uint16_t resistance_;
    
    // Nível e experiência
    uint16_t level_;
    uint64_t experience_;
    uint16_t skillPoints_;
    
    // Vida/Mana
    uint32_t maxHP_;
    uint32_t hp_;
    uint32_t maxMP_;
    uint32_t mp_;
    
    // Recursos
    uint32_t gold_;
    uint32_t bankGold_;
    
    // Estado de combate
    std::time_t combatEndTime_;
    uint32_t lastAttacker_;
    
    // Estado de comércio
    bool isTrading_;
    uint32_t tradePartnerId_;
    
    // Sistema de inventário
    std::unique_ptr<WYDInventory> inventory_;
    
    // Sistema de habilidades
    std::unique_ptr<WYDSkillManager> skillManager_;
    
    // Referências
    PlayerManager* playerManager_;
    
    // Mutex para acesso seguro
    mutable std::mutex mutex_;
    
    // Métodos auxiliares
    /**
 * applyLevelUpEffects
 * 
 * Implementa a funcionalidade applyLevelUpEffects conforme especificação original.
 */

    void applyLevelUpEffects();
    /**
 * calculateExpForLevel
 * 
 * Implementa a funcionalidade calculateExpForLevel conforme especificação original.
 * @param level Parâmetro level
 * @return Retorna uint64_t
 */

    uint64_t calculateExpForLevel(uint16_t level) const;
};

#endif // WYD_PLAYER_H

} // namespace wydbr

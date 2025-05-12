/**
 * Serializer.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/network/Serializer.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "Serializer.h"
#include <iostream>
#include <algorithm>

namespace wydbr {


/**
 * @file Serializer.cpp
 * @brief Implementação do sistema de serialização/desserialização do WYDBRASIL
 * 
 * Esta classe implementa a conversão binária entre estruturas de dados e pacotes de rede,
 * garantindo total compatibilidade com o protocolo do WYD original.
 */

// Implementação do singleton
Serializer& Serializer::getInstance() {
    static Serializer instance;
    return instance;
}

// Construtor
Serializer::Serializer() 
    : m_strictMode(true), 
      m_initialized(false) {
}

// Destrutor
Serializer::~Serializer() {
    shutdown();
}

// Inicializa o serializador
bool Serializer::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Configurar validadores padrã/**
 * setMobValidator
 * 
 * Implementa a funcionalidade setMobValidator conforme especificação original.
 * @param mob Parâmetro mob
 * @return Retorna o
 */
o
    setMobValidator([](const STRUCT_MOB& mob){
        // Validação bá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param '\0' Parâmetro '\0'
 * @return Retorna sica
 */
sica
        if(mob.Name[0] == '\0'){
            return false;
        }
        
        // Verificar valores dentro dos /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param MAX_LEVEL Parâmetro MAX_LEVEL
 * @return Retorna limites
 */
 limites
        if(mob.Level < 0 || mob.Level > MAX_LEVEL){
            return false;
        }
        
        // Validações adicionais...
        
        return true;
    });
    
    setItemValidator([](const STRUCT_ITEM& item) {
        // Validação bá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param MAX_ITEMLIST Parâmetro MAX_ITEMLIST
 * @return Retorna sica
 */
sica
        if(item.sIndex < 0 || item.sIndex >= MAX_ITEMLIST){
            return false;
        }
        
        // Verificar valores dentro dos limites
        if (item.stEffect[0].cEffect != 0 && 
            (item.stEffect[0].cValue < 0 || item.stEffect[0].cValue > 255)) {
            return false;
        }
        
        // Validações adicionais...
        
        return true;
    });
    
    setAffectValidator([](const STRUCT_AFFECT& affect) {
        // Validação bá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 255 Parâmetro 255
 * @return Retorna sica
 */
sica
        if(affect.Type < 0 || affect.Type >= 255){
            return false;
        }
        
        // Verificar valores dentro dos /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna limites
 */
 limites
        if(affect.Time < 0){
            return false;
        }
        
        // Validações adicionais...
        
        return true;
    });
    
    setPositionValidator([](const STRUCT_POSITION& pos) {
        // Validação básica - definir limites de coordenadas válidas
        // (exemplo simplificado, ajustar para valores reais do mapa)
        if (pos.X < 0 || pos.X > 4096 || pos.Y < 0 || pos.Y > 4096) {
            return false;
        }
        
        return true;
    });
    
    m_initialized = true;
    return true;
}

// Finaliza o serializador
void Serializer::shutdown() {
    if (!m_initialized) {
        return;
    }
    
    // Limpar validadores
    std::lock_guard<std::mutex> lock(m_validatorMutex);
    m_mobValidator = nullptr;
    m_itemValidator = nullptr;
    m_affectValidator = nullptr;
    m_positionValidator = nullptr;
    
    m_initialized = false;
}

// Serializa uma string para um buffer binário
bool Serializer::serializeString(const std::string& str, BYTE* buffer, int& offset, int bufferSize, int fixedSize) {
    if (!m_initialized || !buffer || offset < 0) {
        return false;
    }
    
    int strLen = static_cast<int>(str.length());
    
    if (fixedSize > 0) {
        // String de tamanho /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param bufferSize Parâmetro bufferSize
 * @return Retorna fixo
 */
 fixo
        if(offset + fixedSize > bufferSize){
            return false;
        }
        
        // Copiar a string limitando ao tamanho fixo
        int copyLen = std::min(strLen, fixedSize - 1);
        std::memcpy(buffer + offset, str.c_str(), copyLen);
        
        // Adicionar terminador nulo
        buffer[offset + copyLen] = '\0';
        
        // Preencher o restante com zeros se necessá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param fixedSize Parâmetro fixedSize
 * @return Retorna rio
 */
rio
        if(copyLen + 1 < fixedSize){
            std::memset(buffer + offset + copyLen + 1, 0, fixedSize - copyLen - 1);
        }
        
        offset += fixedSize;
    }
    else {
        // String de tamanho variável
        if (offset + sizeof(int) + strLen + 1 > static_cast<size_t>(bufferSize)) {
            return false;
        }
        
        // Serializar tamanho da string
        if (!serialize(strLen, buffer, offset, bufferSize)) {
            return false;
        }
        
        // Serializar caracteres + nulo
        std::memcpy(buffer + offset, str.c_str(), strLen + 1);
        offset += strLen + 1;
    }
    
    return true;
}

// Desserializa uma string de um buffer binário
bool Serializer::deserializeString(std::string& str, const BYTE* buffer, int& offset, int bufferSize, int fixedSize) {
    if (!m_initialized || !buffer || offset < 0) {
        return false;
    }
    
    if (fixedSize > 0) {
        // String de tamanho /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param bufferSize Parâmetro bufferSize
 * @return Retorna fixo
 */
 fixo
        if(offset + fixedSize > bufferSize){
            return false;
        }
        
        // Copiar até encontrar o terminador nulo ou atingir o tamanho máximo
        str.assign(reinterpret_cast<const char*>(buffer + offset));
        offset += fixedSize;
    }
    else {
        // String de tamanho variável
        if (offset + sizeof(int) > static_cast<size_t>(bufferSize)) {
            return false;
        }
        
        // Desserializar tamanho da string
        int strLen = 0;
        if (!deserialize(strLen, buffer, offset, bufferSize)) {
            return false;
        }
        
        // Validar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param bufferSize Parâmetro bufferSize
 * @return Retorna tamanho
 */
 tamanho
        if(strLen < 0 || strLen >= MAX_STRING_LENGTH || offset + strLen + 1 > bufferSize){
            return false;
        }
        
        // Desserializar caracteres
        str.assign(reinterpret_cast<const char*>(buffer + offset), strLen);
        offset += strLen + 1;
    }
    
    return true;
}

// Serializa uma estrutura STRUCT_MOB para um buffer binário
bool Serializer::serializeMob(const STRUCT_MOB& mob, BYTE* buffer, int& offset, int bufferSize) {
    if (!m_initialized || !buffer || offset < 0) {
        return false;
    }
    
    // Validar estrutura se modo estrito /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_strictMode Parâmetro m_strictMode
 * @return Retorna ativado
 */
 ativado
    if(m_strictMode){
        std::lock_guard<std::mutex> lock(m_validatorMutex);
        if (m_mobValidator && !m_mobValidator(mob)) {
            return false;
        }
    }
    
    // Verificar espaço disponível
    if (offset + sizeof(STRUCT_MOB) > static_cast<size_t>(bufferSize)) {
        return false;
    }
    
    // Nome (string de tamanho fixo)
    if (!serializeString(mob.Name, buffer, offset, bufferSize, sizeof(mob.Name))) {
        return false;
    }
    
    // Atributos numéricos
    if (!serialize(mob.Clan, buffer, offset, bufferSize) ||
        !serialize(mob.Merchant, buffer, offset, bufferSize) ||
        !serialize(mob.CapeInfo, buffer, offset, bufferSize) ||
        !serialize(mob.GuildIndex, buffer, offset, bufferSize) ||
        !serialize(mob.ClassMaster, buffer, offset, bufferSize) ||
        !serialize(mob.Level, buffer, offset, bufferSize) ||
        !serialize(mob.Quest, buffer, offset, bufferSize) ||
        !serialize(mob.Merchant, buffer, offset, bufferSize)) {
        return false;
    }
    
    // Atributos de personagem
    if (!serialize(mob.Str, buffer, offset, bufferSize) ||
        !serialize(mob.Int, buffer, offset, bufferSize) ||
        !serialize(mob.Dex, buffer, offset, bufferSize) ||
        !serialize(mob.Con, buffer, offset, bufferSize)) {
        return false;
    }
    
    // Atributos de combate
    if (!serialize(mob.Ac, buffer, offset, bufferSize) ||
        !serialize(mob.Damage, buffer, offset, bufferSize) ||
        !serialize(mob.Hp, buffer, offset, bufferSize) ||
        !serialize(mob.MaxHp, buffer, offset, bufferSize) ||
        !serialize(mob.Mp, buffer, offset, bufferSize) ||
        !serialize(mob.MaxMp, buffer, offset, bufferSize) ||
        !serialize(mob.Exp, buffer, offset, bufferSize)) {
        return false;
    }
    
    // /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna Equipamentos
 */
 Equipamentos
    for(int i = 0; i < MAX_EQUIP; i++){
        if (!serializeItem(mob.Equip[i], buffer, offset, bufferSize)) {
            return false;
        }
    }
    
    // Inventá/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna rio
 */
rio
    for(int i = 0; i < MAX_CARRY; i++){
        if (!serializeItem(mob.Carry[i], buffer, offset, bufferSize)) {
            return false;
        }
    }
    
    // /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna Skills
 */
 Skills
    for(int i = 0; i < MAX_SKILL; i++){
        if (!serialize(mob.SkillBar[i], buffer, offset, bufferSize)) {
            return false;
        }
    }
    
    // /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna Efeitos
 */
 Efeitos
    for(int i = 0; i < MAX_AFFECT; i++){
        if (!serializeAffect(mob.Affect[i], buffer, offset, bufferSize)) {
            return false;
        }
    }
    
    // Outros campos (posição, estado, etc.)
    if (!serializePosition(mob.Position, buffer, offset, bufferSize)) {
        return false;
    }
    
    // Campos restantes...
    
    return true;
}

// Desserializa uma estrutura STRUCT_MOB de um buffer binário
bool Serializer::deserializeMob(STRUCT_MOB& mob, const BYTE* buffer, int& offset, int bufferSize) {
    if (!m_initialized || !buffer || offset < 0) {
        return false;
    }
    
    // Verificar espaço disponível
    if (offset + sizeof(STRUCT_MOB) > static_cast<size_t>(bufferSize)) {
        return false;
    }
    
    // Nome (string de tamanho fixo)
    std::string name;
    if (!deserializeString(name, buffer, offset, bufferSize, sizeof(mob.Name))) {
        return false;
    }
    strncpy(mob.Name, name.c_str(), sizeof(mob.Name) - 1);
    mob.Name[sizeof(mob.Name) - 1] = '\0';
    
    // Atributos numéricos
    if (!deserialize(mob.Clan, buffer, offset, bufferSize) ||
        !deserialize(mob.Merchant, buffer, offset, bufferSize) ||
        !deserialize(mob.CapeInfo, buffer, offset, bufferSize) ||
        !deserialize(mob.GuildIndex, buffer, offset, bufferSize) ||
        !deserialize(mob.ClassMaster, buffer, offset, bufferSize) ||
        !deserialize(mob.Level, buffer, offset, bufferSize) ||
        !deserialize(mob.Quest, buffer, offset, bufferSize) ||
        !deserialize(mob.Merchant, buffer, offset, bufferSize)) {
        return false;
    }
    
    // Atributos de personagem
    if (!deserialize(mob.Str, buffer, offset, bufferSize) ||
        !deserialize(mob.Int, buffer, offset, bufferSize) ||
        !deserialize(mob.Dex, buffer, offset, bufferSize) ||
        !deserialize(mob.Con, buffer, offset, bufferSize)) {
        return false;
    }
    
    // Atributos de combate
    if (!deserialize(mob.Ac, buffer, offset, bufferSize) ||
        !deserialize(mob.Damage, buffer, offset, bufferSize) ||
        !deserialize(mob.Hp, buffer, offset, bufferSize) ||
        !deserialize(mob.MaxHp, buffer, offset, bufferSize) ||
        !deserialize(mob.Mp, buffer, offset, bufferSize) ||
        !deserialize(mob.MaxMp, buffer, offset, bufferSize) ||
        !deserialize(mob.Exp, buffer, offset, bufferSize)) {
        return false;
    }
    
    // /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna Equipamentos
 */
 Equipamentos
    for(int i = 0; i < MAX_EQUIP; i++){
        if (!deserializeItem(mob.Equip[i], buffer, offset, bufferSize)) {
            return false;
        }
    }
    
    // Inventá/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna rio
 */
rio
    for(int i = 0; i < MAX_CARRY; i++){
        if (!deserializeItem(mob.Carry[i], buffer, offset, bufferSize)) {
            return false;
        }
    }
    
    // /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna Skills
 */
 Skills
    for(int i = 0; i < MAX_SKILL; i++){
        if (!deserialize(mob.SkillBar[i], buffer, offset, bufferSize)) {
            return false;
        }
    }
    
    // /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna Efeitos
 */
 Efeitos
    for(int i = 0; i < MAX_AFFECT; i++){
        if (!deserializeAffect(mob.Affect[i], buffer, offset, bufferSize)) {
            return false;
        }
    }
    
    // Outros campos (posição, estado, etc.)
    if (!deserializePosition(mob.Position, buffer, offset, bufferSize)) {
        return false;
    }
    
    // Campos restantes...
    
    // Validar estrutura se modo estrito /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_strictMode Parâmetro m_strictMode
 * @return Retorna ativado
 */
 ativado
    if(m_strictMode){
        std::lock_guard<std::mutex> lock(m_validatorMutex);
        if (m_mobValidator && !m_mobValidator(mob)) {
            return false;
        }
    }
    
    return true;
}

// Serializa uma estrutura STRUCT_ITEM para um buffer binário
bool Serializer::serializeItem(const STRUCT_ITEM& item, BYTE* buffer, int& offset, int bufferSize) {
    if (!m_initialized || !buffer || offset < 0) {
        return false;
    }
    
    // Validar estrutura se modo estrito /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_strictMode Parâmetro m_strictMode
 * @return Retorna ativado
 */
 ativado
    if(m_strictMode){
        std::lock_guard<std::mutex> lock(m_validatorMutex);
        if (m_itemValidator && !m_itemValidator(item)) {
            return false;
        }
    }
    
    // Verificar espaço disponível (STRUCT_ITEM tem 8 bytes)
    if (offset + 8 > static_cast<size_t>(bufferSize)) {
        return false;
    }
    
    // Serializar índice do item
    if (!serialize(item.sIndex, buffer, offset, bufferSize)) {
        return false;
    }
    
    // Serializar /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna efeitos
 */
 efeitos
    for(int i = 0; i < MAX_ITEM_OPTION; i++){
        if (!serialize(item.stEffect[i].cEffect, buffer, offset, bufferSize) ||
            !serialize(item.stEffect[i].cValue, buffer, offset, bufferSize)) {
            return false;
        }
    }
    
    return true;
}

// Desserializa uma estrutura STRUCT_ITEM de um buffer binário
bool Serializer::deserializeItem(STRUCT_ITEM& item, const BYTE* buffer, int& offset, int bufferSize) {
    if (!m_initialized || !buffer || offset < 0) {
        return false;
    }
    
    // Verificar espaço disponível (STRUCT_ITEM tem 8 bytes)
    if (offset + 8 > static_cast<size_t>(bufferSize)) {
        return false;
    }
    
    // Desserializar índice do item
    if (!deserialize(item.sIndex, buffer, offset, bufferSize)) {
        return false;
    }
    
    // Desserializar /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna efeitos
 */
 efeitos
    for(int i = 0; i < MAX_ITEM_OPTION; i++){
        if (!deserialize(item.stEffect[i].cEffect, buffer, offset, bufferSize) ||
            !deserialize(item.stEffect[i].cValue, buffer, offset, bufferSize)) {
            return false;
        }
    }
    
    // Validar estrutura se modo estrito /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_strictMode Parâmetro m_strictMode
 * @return Retorna ativado
 */
 ativado
    if(m_strictMode){
        std::lock_guard<std::mutex> lock(m_validatorMutex);
        if (m_itemValidator && !m_itemValidator(item)) {
            return false;
        }
    }
    
    return true;
}

// Serializa uma estrutura STRUCT_AFFECT para um buffer binário
bool Serializer::serializeAffect(const STRUCT_AFFECT& affect, BYTE* buffer, int& offset, int bufferSize) {
    if (!m_initialized || !buffer || offset < 0) {
        return false;
    }
    
    // Validar estrutura se modo estrito /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_strictMode Parâmetro m_strictMode
 * @return Retorna ativado
 */
 ativado
    if(m_strictMode){
        std::lock_guard<std::mutex> lock(m_validatorMutex);
        if (m_affectValidator && !m_affectValidator(affect)) {
            return false;
        }
    }
    
    // Verificar espaço disponível
    if (offset + sizeof(STRUCT_AFFECT) > static_cast<size_t>(bufferSize)) {
        return false;
    }
    
    // Serializar campos
    if (!serialize(affect.Type, buffer, offset, bufferSize) ||
        !serialize(affect.Value, buffer, offset, bufferSize) ||
        !serialize(affect.Level, buffer, offset, bufferSize) ||
        !serialize(affect.Time, buffer, offset, bufferSize)) {
        return false;
    }
    
    return true;
}

// Desserializa uma estrutura STRUCT_AFFECT de um buffer binário
bool Serializer::deserializeAffect(STRUCT_AFFECT& affect, const BYTE* buffer, int& offset, int bufferSize) {
    if (!m_initialized || !buffer || offset < 0) {
        return false;
    }
    
    // Verificar espaço disponível
    if (offset + sizeof(STRUCT_AFFECT) > static_cast<size_t>(bufferSize)) {
        return false;
    }
    
    // Desserializar campos
    if (!deserialize(affect.Type, buffer, offset, bufferSize) ||
        !deserialize(affect.Value, buffer, offset, bufferSize) ||
        !deserialize(affect.Level, buffer, offset, bufferSize) ||
        !deserialize(affect.Time, buffer, offset, bufferSize)) {
        return false;
    }
    
    // Validar estrutura se modo estrito /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_strictMode Parâmetro m_strictMode
 * @return Retorna ativado
 */
 ativado
    if(m_strictMode){
        std::lock_guard<std::mutex> lock(m_validatorMutex);
        if (m_affectValidator && !m_affectValidator(affect)) {
            return false;
        }
    }
    
    return true;
}

// Serializa uma estrutura STRUCT_POSITION para um buffer binário
bool Serializer::serializePosition(const STRUCT_POSITION& position, BYTE* buffer, int& offset, int bufferSize) {
    if (!m_initialized || !buffer || offset < 0) {
        return false;
    }
    
    // Validar estrutura se modo estrito /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_strictMode Parâmetro m_strictMode
 * @return Retorna ativado
 */
 ativado
    if(m_strictMode){
        std::lock_guard<std::mutex> lock(m_validatorMutex);
        if (m_positionValidator && !m_positionValidator(position)) {
            return false;
        }
    }
    
    // Verificar espaço disponível
    if (offset + sizeof(STRUCT_POSITION) > static_cast<size_t>(bufferSize)) {
        return false;
    }
    
    // Serializar campos
    if (!serialize(position.X, buffer, offset, bufferSize) ||
        !serialize(position.Y, buffer, offset, bufferSize)) {
        return false;
    }
    
    return true;
}

// Desserializa uma estrutura STRUCT_POSITION de um buffer binário
bool Serializer::deserializePosition(STRUCT_POSITION& position, const BYTE* buffer, int& offset, int bufferSize) {
    if (!m_initialized || !buffer || offset < 0) {
        return false;
    }
    
    // Verificar espaço disponível
    if (offset + sizeof(STRUCT_POSITION) > static_cast<size_t>(bufferSize)) {
        return false;
    }
    
    // Desserializar campos
    if (!deserialize(position.X, buffer, offset, bufferSize) ||
        !deserialize(position.Y, buffer, offset, bufferSize)) {
        return false;
    }
    
    // Validar estrutura se modo estrito /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_strictMode Parâmetro m_strictMode
 * @return Retorna ativado
 */
 ativado
    if(m_strictMode){
        std::lock_guard<std::mutex> lock(m_validatorMutex);
        if (m_positionValidator && !m_positionValidator(position)) {
            return false;
        }
    }
    
    return true;
}

// Define função de validação para estrutura STRUCT_MOB
void Serializer::setMobValidator(std::function<bool(const STRUCT_MOB&)> validator) {
    std::lock_guard<std::mutex> lock(m_validatorMutex);
    m_mobValidator = validator;
}

// Define função de validação para estrutura STRUCT_ITEM
void Serializer::setItemValidator(std::function<bool(const STRUCT_ITEM&)> validator) {
    std::lock_guard<std::mutex> lock(m_validatorMutex);
    m_itemValidator = validator;
}

// Define função de validação para estrutura STRUCT_AFFECT
void Serializer::setAffectValidator(std::function<bool(const STRUCT_AFFECT&)> validator) {
    std::lock_guard<std::mutex> lock(m_validatorMutex);
    m_affectValidator = validator;
}

// Define função de validação para estrutura STRUCT_POSITION
void Serializer::setPositionValidator(std::function<bool(const STRUCT_POSITION&)> validator) {
    std::lock_guard<std::mutex> lock(m_validatorMutex);
    m_positionValidator = validator;
}

// Define modo de segurança para validações
void Serializer::setSecurityMode(bool strictMode) {

} // namespace wydbr

    m_strictMode = strictMode;
}
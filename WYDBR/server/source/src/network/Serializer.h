/**
 * Serializer.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/network/Serializer.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _SERIALIZER_H_
#define _SERIALIZER_H_

/**
 * @file Serializer.h
 * @brief Sistema de serialização/desserialização do WYDBRASIL
 * 
 * Esta classe implementa conversão binária entre estruturas de dados e pacotes de rede,
 * garantindo total compatibilidade com o protocolo original do WYD, enquanto adiciona
 * validações e segurança.
 */

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <cstring>

#include "../../include/Basedef.h"

namespace wydbr {


/**
 * @class Serializer
 * @brief Converte estruturas de dados para pacotes de rede e vice-versa
 * 
 * Esta classe implementa métodos para serializar e desserializar estruturas de dados
 * para o formato binário usado pelo protocolo do WYD, garantindo compatibilidade total
 * com o cliente original.
 */
/**
 * Classe WYDSerializer
 * 
 * Esta classe implementa a funcionalidade WYDSerializer conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class Serializer {
public:
    /**
     * @brief Obtém a instância única do Serializer (Singleton)
     * @return Referência para a instância única
     */
    static Serializer& getInstance();
    
    /**
     * @brief Inicializa o serializador
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Finaliza o serializador
     */
    void shutdown();
    
    /**
     * @brief Serializa um valor para um buffer binário
     * @tparam T Tipo do valor
     * @param value Valor a ser serializado
     * @param buffer Buffer de destino
     * @param offset Posição inicial no buffer (será atualizado)
     * @param bufferSize Tamanho total do buffer
     * @return true se serializado com sucesso
     */
    template<typename T>
    /**
 * serialize
 * 
 * Implementa a funcionalidade serialize conforme especificação original.
 * @param value Parâmetro value
 * @param buffer Parâmetro buffer
 * @param offset Parâmetro offset
 * @param bufferSize Parâmetro bufferSize
 * @return Retorna bool
 */

    bool serialize(const T& value, BYTE* buffer, int& offset, int bufferSize);
    
    /**
     * @brief Desserializa um valor de um buffer binário
     * @tparam T Tipo do valor
     * @param value Valor desserializado (saída)
     * @param buffer Buffer de origem
     * @param offset Posição inicial no buffer (será atualizado)
     * @param bufferSize Tamanho total do buffer
     * @return true se desserializado com sucesso
     */
    template<typename T>
    /**
 * deserialize
 * 
 * Implementa a funcionalidade deserialize conforme especificação original.
 * @param value Parâmetro value
 * @param buffer Parâmetro buffer
 * @param offset Parâmetro offset
 * @param bufferSize Parâmetro bufferSize
 * @return Retorna bool
 */

    bool deserialize(T& value, const BYTE* buffer, int& offset, int bufferSize);
    
    /**
     * @brief Serializa uma string para um buffer binário
     * @param str String a ser serializada
     * @param buffer Buffer de destino
     * @param offset Posição inicial no buffer (será atualizado)
     * @param bufferSize Tamanho total do buffer
     * @param fixedSize Tamanho fixo para a string (0 para variável)
     * @return true se serializado com sucesso
     */
    bool serializeString(const std::string& str, BYTE* buffer, int& offset, int bufferSize, int fixedSize = 0);
    
    /**
     * @brief Desserializa uma string de um buffer binário
     * @param str String desserializada (saída)
     * @param buffer Buffer de origem
     * @param offset Posição inicial no buffer (será atualizado)
     * @param bufferSize Tamanho total do buffer
     * @param fixedSize Tamanho fixo para a string (0 para variável)
     * @return true se desserializado com sucesso
     */
    bool deserializeString(std::string& str, const BYTE* buffer, int& offset, int bufferSize, int fixedSize = 0);
    
    /**
     * @brief Serializa um array para um buffer binário
     * @tparam T Tipo dos elementos do array
     * @param array Array a ser serializado
     * @param count Número de elementos
     * @param buffer Buffer de destino
     * @param offset Posição inicial no buffer (será atualizado)
     * @param bufferSize Tamanho total do buffer
     * @return true se serializado com sucesso
     */
    template<typename T>
    /**
 * serializeArray
 * 
 * Implementa a funcionalidade serializeArray conforme especificação original.
 * @param array Parâmetro array
 * @param count Parâmetro count
 * @param buffer Parâmetro buffer
 * @param offset Parâmetro offset
 * @param bufferSize Parâmetro bufferSize
 * @return Retorna bool
 */

    bool serializeArray(const T* array, int count, BYTE* buffer, int& offset, int bufferSize);
    
    /**
     * @brief Desserializa um array de um buffer binário
     * @tparam T Tipo dos elementos do array
     * @param array Array desserializado (saída)
     * @param count Número de elementos
     * @param buffer Buffer de origem
     * @param offset Posição inicial no buffer (será atualizado)
     * @param bufferSize Tamanho total do buffer
     * @return true se desserializado com sucesso
     */
    template<typename T>
    /**
 * deserializeArray
 * 
 * Implementa a funcionalidade deserializeArray conforme especificação original.
 * @param array Parâmetro array
 * @param count Parâmetro count
 * @param buffer Parâmetro buffer
 * @param offset Parâmetro offset
 * @param bufferSize Parâmetro bufferSize
 * @return Retorna bool
 */

    bool deserializeArray(T* array, int count, const BYTE* buffer, int& offset, int bufferSize);
    
    /**
     * @brief Serializa um vetor para um buffer binário
     * @tparam T Tipo dos elementos do vetor
     * @param vec Vetor a ser serializado
     * @param buffer Buffer de destino
     * @param offset Posição inicial no buffer (será atualizado)
     * @param bufferSize Tamanho total do buffer
     * @return true se serializado com sucesso
     */
    template<typename T>
    /**
 * serializeVector
 * 
 * Implementa a funcionalidade serializeVector conforme especificação original.
 * @param vec Parâmetro vec
 * @param buffer Parâmetro buffer
 * @param offset Parâmetro offset
 * @param bufferSize Parâmetro bufferSize
 * @return Retorna bool
 */

    bool serializeVector(const std::vector<T>& vec, BYTE* buffer, int& offset, int bufferSize);
    
    /**
     * @brief Desserializa um vetor de um buffer binário
     * @tparam T Tipo dos elementos do vetor
     * @param vec Vetor desserializado (saída)
     * @param buffer Buffer de origem
     * @param offset Posição inicial no buffer (será atualizado)
     * @param bufferSize Tamanho total do buffer
     * @param maxElements Número máximo de elementos (segurança)
     * @return true se desserializado com sucesso
     */
    template<typename T>
    /**
 * deserializeVector
 * 
 * Implementa a funcionalidade deserializeVector conforme especificação original.
 * @param vec Parâmetro vec
 * @param buffer Parâmetro buffer
 * @param offset Parâmetro offset
 * @param bufferSize Parâmetro bufferSize
 * @param 1000 Parâmetro 1000
 * @return Retorna bool
 */

    bool deserializeVector(std::vector<T>& vec, const BYTE* buffer, int& offset, int bufferSize, int maxElements = 1000);
    
    /**
     * @brief Serializa uma estrutura STRUCT_MOB para um buffer binário
     * @param mob Estrutura a ser serializada
     * @param buffer Buffer de destino
     * @param offset Posição inicial no buffer (será atualizado)
     * @param bufferSize Tamanho total do buffer
     * @return true se serializado com sucesso
     */
    bool serializeMob(const STRUCT_MOB& mob, BYTE* buffer, int& offset, int bufferSize);
    
    /**
     * @brief Desserializa uma estrutura STRUCT_MOB de um buffer binário
     * @param mob Estrutura desserializada (saída)
     * @param buffer Buffer de origem
     * @param offset Posição inicial no buffer (será atualizado)
     * @param bufferSize Tamanho total do buffer
     * @return true se desserializado com sucesso
     */
    bool deserializeMob(STRUCT_MOB& mob, const BYTE* buffer, int& offset, int bufferSize);
    
    /**
     * @brief Serializa uma estrutura STRUCT_ITEM para um buffer binário
     * @param item Estrutura a ser serializada
     * @param buffer Buffer de destino
     * @param offset Posição inicial no buffer (será atualizado)
     * @param bufferSize Tamanho total do buffer
     * @return true se serializado com sucesso
     */
    bool serializeItem(const STRUCT_ITEM& item, BYTE* buffer, int& offset, int bufferSize);
    
    /**
     * @brief Desserializa uma estrutura STRUCT_ITEM de um buffer binário
     * @param item Estrutura desserializada (saída)
     * @param buffer Buffer de origem
     * @param offset Posição inicial no buffer (será atualizado)
     * @param bufferSize Tamanho total do buffer
     * @return true se desserializado com sucesso
     */
    bool deserializeItem(STRUCT_ITEM& item, const BYTE* buffer, int& offset, int bufferSize);
    
    /**
     * @brief Serializa uma estrutura STRUCT_AFFECT para um buffer binário
     * @param affect Estrutura a ser serializada
     * @param buffer Buffer de destino
     * @param offset Posição inicial no buffer (será atualizado)
     * @param bufferSize Tamanho total do buffer
     * @return true se serializado com sucesso
     */
    bool serializeAffect(const STRUCT_AFFECT& affect, BYTE* buffer, int& offset, int bufferSize);
    
    /**
     * @brief Desserializa uma estrutura STRUCT_AFFECT de um buffer binário
     * @param affect Estrutura desserializada (saída)
     * @param buffer Buffer de origem
     * @param offset Posição inicial no buffer (será atualizado)
     * @param bufferSize Tamanho total do buffer
     * @return true se desserializado com sucesso
     */
    bool deserializeAffect(STRUCT_AFFECT& affect, const BYTE* buffer, int& offset, int bufferSize);
    
    /**
     * @brief Serializa uma estrutura STRUCT_POSITION para um buffer binário
     * @param position Estrutura a ser serializada
     * @param buffer Buffer de destino
     * @param offset Posição inicial no buffer (será atualizado)
     * @param bufferSize Tamanho total do buffer
     * @return true se serializado com sucesso
     */
    bool serializePosition(const STRUCT_POSITION& position, BYTE* buffer, int& offset, int bufferSize);
    
    /**
     * @brief Desserializa uma estrutura STRUCT_POSITION de um buffer binário
     * @param position Estrutura desserializada (saída)
     * @param buffer Buffer de origem
     * @param offset Posição inicial no buffer (será atualizado)
     * @param bufferSize Tamanho total do buffer
     * @return true se desserializado com sucesso
     */
    bool deserializePosition(STRUCT_POSITION& position, const BYTE* buffer, int& offset, int bufferSize);
    
    /**
     * @brief Define função de validação para estrutura STRUCT_MOB
     * @param validator Função de validação
     */
    void setMobValidator(std::function<bool(const STRUCT_MOB&)> validator);
    
    /**
     * @brief Define função de validação para estrutura STRUCT_ITEM
     * @param validator Função de validação
     */
    void setItemValidator(std::function<bool(const STRUCT_ITEM&)> validator);
    
    /**
     * @brief Define função de validação para estrutura STRUCT_AFFECT
     * @param validator Função de validação
     */
    void setAffectValidator(std::function<bool(const STRUCT_AFFECT&)> validator);
    
    /**
     * @brief Define função de validação para estrutura STRUCT_POSITION
     * @param validator Função de validação
     */
    void setPositionValidator(std::function<bool(const STRUCT_POSITION&)> validator);
    
    /**
     * @brief Define modo de segurança para validações
     * @param strictMode Modo estrito (validações mais rigorosas)
     */
    void setSecurityMode(bool strictMode);

private:
    // Construtor privado (singleton)
    Serializer();
    
    // Destrutor
    ~Serializer();
    
    // Impedir cópia e atribuição
    Serializer(const Serializer&) = delete;
    Serializer& operator=(const Serializer&) = delete;
    
    // Funções de validação para estruturas
    std::function<bool(const STRUCT_MOB&)> m_mobValidator;
    std::function<bool(const STRUCT_ITEM&)> m_itemValidator;
    std::function<bool(const STRUCT_AFFECT&)> m_affectValidator;
    std::function<bool(const STRUCT_POSITION&)> m_positionValidator;
    
    // Mutex para validadores
    std::mutex m_validatorMutex;
    
    // Modo estrito
    bool m_strictMode;
    
    // Inicializado
    bool m_initialized;
};

// Implementação dos templates

template<typename T>
bool Serializer::serialize(const T& value, BYTE* buffer, int& offset, int bufferSize) {
    if (!m_initialized || !buffer || offset < 0 || offset + sizeof(T) > static_cast<size_t>(bufferSize)) {
        return false;
    }
    
    // Serializar diretamente para tipos primitivos
    std::memcpy(buffer + offset, &value, sizeof(T));
    offset += sizeof(T);
    
    return true;
}

template<typename T>
bool Serializer::deserialize(T& value, const BYTE* buffer, int& offset, int bufferSize) {
    if (!m_initialized || !buffer || offset < 0 || offset + sizeof(T) > static_cast<size_t>(bufferSize)) {
        return false;
    }
    
    // Desserializar diretamente para tipos primitivos
    std::memcpy(&value, buffer + offset, sizeof(T));
    offset += sizeof(T);
    
    return true;
}

template<typename T>
bool Serializer::serializeArray(const T* array, int count, BYTE* buffer, int& offset, int bufferSize) {
    if (!m_initialized || !buffer || !array || count <= 0 || offset < 0 || 
        offset + sizeof(T) * count > static_cast<size_t>(bufferSize)) {
        return false;
    }
    
    // Serializar array de tipos primitivos
    std::memcpy(buffer + offset, array, sizeof(T) * count);
    offset += sizeof(T) * count;
    
    return true;
}

template<typename T>
bool Serializer::deserializeArray(T* array, int count, const BYTE* buffer, int& offset, int bufferSize) {
    if (!m_initialized || !buffer || !array || count <= 0 || offset < 0 || 
        offset + sizeof(T) * count > static_cast<size_t>(bufferSize)) {
        return false;
    }
    
    // Desserializar array de tipos primitivos
    std::memcpy(array, buffer + offset, sizeof(T) * count);
    offset += sizeof(T) * count;
    
    return true;
}

template<typename T>
bool Serializer::serializeVector(const std::vector<T>& vec, BYTE* buffer, int& offset, int bufferSize) {
    if (!m_initialized || !buffer || offset < 0 || 
        offset + sizeof(int) + sizeof(T) * vec.size() > static_cast<size_t>(bufferSize)) {
        return false;
    }
    
    // Serializar tamanho do vetor
    int size = static_cast<int>(vec.size());
    if (!serialize(size, buffer, offset, bufferSize)) {
        return false;
    }
    
    // Serializar elementos do vetor
    if (!vec.empty()) {
        if (!serializeArray(&vec[0], size, buffer, offset, bufferSize)) {
            return false;
        }
    }
    
    return true;
}

template<typename T>
bool Serializer::deserializeVector(std::vector<T>& vec, const BYTE* buffer, int& offset, int bufferSize, int maxElements) {
    if (!m_initialized || !buffer || offset < 0 || offset + sizeof(int) > static_cast<size_t>(bufferSize)) {
        return false;
    }
    
    // Desserializar tamanho do vetor
    int size = 0;
    if (!deserialize(size, buffer, offset, bufferSize)) {
        return false;
    }
    
    // Validar tamanho
    if (size < 0 || size > maxElements || offset + sizeof(T) * size > static_cast<size_t>(bufferSize)) {
        return false;
    }
    
    // Redimensionar vetor
    vec.resize(size);
    
    // Desserializar elementos do /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna vetor
 */
 vetor
    if(size > 0){
        if (!deserializeArray(&vec[0], size, buffer, offset, bufferSize)) {
            return false;
        }
    }
    
    return true;
}

#endif // _SERIALIZER_H_

} // namespace wydbr

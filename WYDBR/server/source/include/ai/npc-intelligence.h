/**
 * @file npc-intelligence.h
 * @brief IA Assistente para NPCs
 *
 * Implementação do sistema de IA Assistente para NPCs para o WYDBR.
 * Este sistema implementa funcionalidades avançadas mantendo
 * compatibilidade total com o cliente original.
 */

#ifndef WYDBR_AI_NPC_INTELLIGENCE_H
#define WYDBR_AI_NPC_INTELLIGENCE_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <functional>

namespace wydbr {
namespace ai {

/**
 * @class Npcintelligence
 * @brief Implementação C++ do sistema de IA Assistente para NPCs
 */
class Npcintelligence {
public:
    /**
     * @brief Obtém a instância única do sistema (singleton)
     * @return Referência para a instância
     */
    static Npcintelligence& GetInstance();
    
    /**
     * @brief Inicializa o sistema
     * @param config Configuração do sistema
     * @return Verdadeiro se inicializado com sucesso
     */
    bool Initialize(const std::map<std::string, std::string>& config = {});
    
    /**
     * @brief Finaliza o sistema
     */
    void Shutdown();
    
    // Métodos específicos serão implementados de acordo com a funcionalidade
    
private:
    // Construtor e destrutor privados (singleton)
    Npcintelligence();
    ~Npcintelligence();
    
    // Prevenir cópia (singleton)
    Npcintelligence(const Npcintelligence&) = delete;
    Npcintelligence& operator=(const Npcintelligence&) = delete;
    
    // Atributos privados
    bool m_initialized;
    std::mutex m_mutex;
    
    // Métodos privados
};

} // namespace ai
} // namespace wydbr

#endif // WYDBR_AI_NPC_INTELLIGENCE_H

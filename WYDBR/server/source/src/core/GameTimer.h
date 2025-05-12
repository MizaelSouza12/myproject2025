/**
 * GameTimer.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/core/GameTimer.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _GAME_TIMER_H_
#define _GAME_TIMER_H_

/**
 * @file GameTimer.h
 * @brief Sistema de gerenciamento de tempo para o WYDBRASIL
 * 
 * Esta classe implementa um sistema de gerenciamento de tempo para o jogo,
 * permitindo o controle preciso de eventos programados e temporizadores.
 */

#include <functional>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <string>
#include <memory>
#include <algorithm>

namespace wydbr {


/**
 * @typedef TimerId
 * @brief Tipo para ID de temporizador
 */
using TimerId = int64_t;

/**
 * @class GameTimer
 * @brief Sistema de gerenciamento de tempo para eventos do jogo
 * 
 * Esta classe implementa um sistema que gerencia temporizadores e eventos
 * programados para o jogo, garantindo precisão e eficiência.
 */
/**
 * Classe WYDGameTimer
 * 
 * Esta classe implementa a funcionalidade WYDGameTimer conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class GameTimer {
public:
    /**
     * @brief Obtém a instância única do GameTimer (Singleton)
     * @return Referência para a instância única
     */
    static GameTimer& getInstance();
    
    /**
     * @brief Inicializa o sistema de timer
     * @param useHighResolution Usar temporizadores de alta resolução
     * @param numThreads Número de threads para processamento
     * @return true se inicializado com sucesso
     */
    bool initialize(bool useHighResolution = true, int numThreads = 1);
    
    /**
     * @brief Finaliza o sistema de timer
     */
    void shutdown();
    
    /**
     * @brief Cria um timer de uso único
     * @param callback Função de callback
     * @param delayMs Atraso em milissegundos
     * @param description Descrição do timer (opcional)
     * @return ID do timer ou -1 se falha
     */
    TimerId createOneShotTimer(std::function<void()> callback, int delayMs, const std::string& description = "");
    
    /**
     * @brief Cria um timer periódico
     * @param callback Função de callback
     * @param intervalMs Intervalo em milissegundos
     * @param description Descrição do timer (opcional)
     * @param initialDelayMs Atraso inicial em milissegundos (opcional)
     * @param repeatCount Número de repetições (0 = infinito)
     * @return ID do timer ou -1 se falha
     */
    TimerId createPeriodicTimer(std::function<void()> callback, int intervalMs, 
                               const std::string& description = "", int initialDelayMs = 0, 
                               int repeatCount = 0);
    
    /**
     * @brief Cancela um timer
     * @param timerId ID do timer
     * @return true se cancelado com sucesso
     */
    bool cancelTimer(TimerId timerId);
    
    /**
     * @brief Pausa um timer
     * @param timerId ID do timer
     * @return true se pausado com sucesso
     */
    bool pauseTimer(TimerId timerId);
    
    /**
     * @brief Retoma um timer pausado
     * @param timerId ID do timer
     * @return true se retomado com sucesso
     */
    bool resumeTimer(TimerId timerId);
    
    /**
     * @brief Redefine um timer periódico
     * @param timerId ID do timer
     * @param newIntervalMs Novo intervalo em milissegundos
     * @return true se redefinido com sucesso
     */
    bool resetTimer(TimerId timerId, int newIntervalMs);
    
    /**
     * @brief Verifica se um timer está ativo
     * @param timerId ID do timer
     * @return true se ativo
     */
    bool isTimerActive(TimerId timerId);
    
    /**
     * @brief Obtém tempo restante de um timer
     * @param timerId ID do timer
     * @return Tempo restante em milissegundos ou -1 se inválido
     */
    int64_t getTimeRemaining(TimerId timerId);
    
    /**
     * @brief Obtém descrição de um timer
     * @param timerId ID do timer
     * @return Descrição do timer ou string vazia se inválido
     */
    std::string getTimerDescription(TimerId timerId);
    
    /**
     * @brief Obtém estatísticas do sistema de timer
     * @param activeTimers Número de timers ativos (saída)
     * @param totalTriggered Total de temporizadores acionados (saída)
     * @param avgExecutionTime Tempo médio de execução em ms (saída)
     * @param missedDeadlines Número de prazos perdidos (saída)
     */
    void getStats(int& activeTimers, uint64_t& totalTriggered, 
                 float& avgExecutionTime, uint64_t& missedDeadlines);
    
    /**
     * @brief Limpa estatísticas do sistema de timer
     */
    void clearStats();
    
    /**
     * @brief Define a taxa de tempo do jogo (aceleração/desaceleração)
     * @param timeScale Escala de tempo (1.0 = normal, 2.0 = dobro da velocidade)
     */
    void setTimeScale(float timeScale);
    
    /**
     * @brief Obtém a taxa de tempo atual
     * @return Escala de tempo atual
     */
    float getTimeScale() const;
    
    /**
     * @brief Obtém o tempo do jogo (afetado pela escala)
     * @return Tempo em milissegundos
     */
    uint64_t getGameTime() const;
    
    /**
     * @brief Obtém o tempo real (não afetado pela escala)
     * @return Tempo em milissegundos
     */
    uint64_t getRealTime() const;
    
    /**
     * @brief Converte tempo do jogo para tempo real
     * @param gameTime Tempo do jogo em milissegundos
     * @return Tempo real em milissegundos
     */
    uint64_t gameToRealTime(uint64_t gameTime) const;
    
    /**
     * @brief Converte tempo real para tempo do jogo
     * @param realTime Tempo real em milissegundos
     * @return Tempo do jogo em milissegundos
     */
    uint64_t realToGameTime(uint64_t realTime) const;
    
    /**
     * @brief Define o tempo do jogo manualmente (para sincronização)
     * @param gameTime Tempo do jogo em milissegundos
     */
    void setGameTime(uint64_t gameTime);
    
    /**
     * @brief Pausa o tempo do jogo
     */
    void pauseGameTime();
    
    /**
     * @brief Retoma o tempo do jogo
     */
    void resumeGameTime();
    
    /**
     * @brief Verifica se o tempo do jogo está pausado
     * @return true se pausado
     */
    bool isGameTimePaused() const;
    
private:
    // Construtor privado (singleton)
    GameTimer();
    
    // Destrutor
    ~GameTimer();
    
    // Impedir cópia e atribuição
    GameTimer(const GameTimer&) = delete;
    GameTimer& operator=(const GameTimer&) = delete;
    
    // Estrutura para timer
    struct Timer {
        TimerId id;                                  // ID do timer
        std::function<void()> callback;              // Função de callback
        std::chrono::steady_clock::time_point nextTrigger;  // Próximo acionamento
        std::chrono::milliseconds interval;          // Intervalo para timers periódicos
        std::string description;                     // Descrição do timer
        bool isPeriodic;                             // Timer periódico
        bool isActive;                               // Timer ativo
        bool isPaused;                               // Timer pausado
        int repeatCount;                             // Número de repetições restantes
        
        Timer(TimerId id, std::function<void()> callback, 
              std::chrono::steady_clock::time_point nextTrigger,
              std::chrono::milliseconds interval, const std::string& description,
              bool isPeriodic, int repeatCount)
            : id(id), callback(std::move(callback)), nextTrigger(nextTrigger),
              interval(interval), description(description),
              isPeriodic(isPeriodic), isActive(true), isPaused(false),
              repeatCount(repeatCount) {}
    };
    
    // Tipo para fila de prioridade de timers
    struct TimerComparator {
        bool operator()(const std::shared_ptr<Timer>& a, const std::shared_ptr<Timer>& b) const {
            return a->nextTrigger > b->nextTrigger;
        }
    };
    
    // Thread de processamento de timers
    /**
 * timerThread
 * 
 * Implementa a funcionalidade timerThread conforme especificação original.
 */

    void timerThread();
    
    // Verifica e dispara timers
    /**
 * checkAndTriggerTimers
 * 
 * Implementa a funcionalidade checkAndTriggerTimers conforme especificação original.
 */

    void checkAndTriggerTimers();
    
    // Calcula próximo acionamento para timer periódico
    /**
 * calculateNextTrigger
 * 
 * Implementa a funcionalidade calculateNextTrigger conforme especificação original.
 * @param timer Parâmetro timer
 */

    void calculateNextTrigger(std::shared_ptr<Timer> timer);
    
    // Atualiza tempos de jogo
    /**
 * updateGameTime
 * 
 * Implementa a funcionalidade updateGameTime conforme especificação original.
 */

    void updateGameTime();
    
    // Obtém próximo ID de timer
    /**
 * getNextTimerId
 * 
 * Implementa a funcionalidade getNextTimerId conforme especificação original.
 * @return Retorna TimerId
 */

    TimerId getNextTimerId();
    
    // Fila de prioridade para timers ordenados por tempo
    std::priority_queue<std::shared_ptr<Timer>, 
                       std::vector<std::shared_ptr<Timer>>, 
                       TimerComparator> m_timerQueue;
    
    // Mapeia IDs para timers
    std::unordered_map<TimerId, std::shared_ptr<Timer>> m_timers;
    
    // Mutex para acesso à fila/mapa de timers
    std::mutex m_timersMutex;
    
    // Variável de condição para notificar threads
    std::condition_variable m_timersCV;
    
    // Threads de processamento
    std::vector<std::thread> m_threads;
    
    // Próximo ID de timer
    std::atomic<TimerId> m_nextTimerId;
    
    // Flag de atividade
    std::atomic<bool> m_active;
    
    // Escala de tempo
    std::atomic<float> m_timeScale;
    
    // Tempo do jogo
    std::atomic<uint64_t> m_gameTime;
    
    // Tempo inicial do jogo
    std::chrono::steady_clock::time_point m_gameStartTime;
    
    // Tempo de jogo pausado
    std::atomic<bool> m_gameTimePaused;
    
    // Tempo acumulado antes da pausa
    std::atomic<uint64_t> m_pausedGameTime;
    
    // Tempo de pausa
    std::chrono::steady_clock::time_point m_pauseStartTime;
    
    // Configurações
    bool m_useHighResolution;
    int m_numThreads;
    
    // Estatísticas
    std::atomic<uint64_t> m_totalTriggered;
    std::atomic<uint64_t> m_totalExecutionTime;
    std::atomic<uint64_t> m_missedDeadlines;
    
    // Inicializado
    bool m_initialized;
};

#endif // _GAME_TIMER_H_

} // namespace wydbr

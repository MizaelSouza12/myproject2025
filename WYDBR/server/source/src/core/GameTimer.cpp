/**
 * GameTimer.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/core/GameTimer.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "GameTimer.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace wydbr {


/**
 * @file GameTimer.cpp
 * @brief Implementação do sistema de gerenciamento de tempo para o WYDBRASIL
 * 
 * Gerencia temporizadores e eventos programados de forma precisa e eficiente,
 * permitindo controle e sincronização do tempo de jogo.
 */

// Implementação do singleton
GameTimer& GameTimer::getInstance() {
    static GameTimer instance;
    return instance;
}

// Construtor
GameTimer::GameTimer() 
    : m_nextTimerId(1),
      m_active(false),
      m_timeScale(1.0f),
      m_gameTime(0),
      m_gameTimePaused(false),
      m_pausedGameTime(0),
      m_useHighResolution(true),
      m_numThreads(1),
      m_totalTriggered(0),
      m_totalExecutionTime(0),
      m_missedDeadlines(0),
      m_initialized(false) {
    
    m_gameStartTime = std::chrono::steady_clock::now();
}

// Destrutor
GameTimer::~GameTimer() {
    shutdown();
}

// Inicializa o sistema de timer
bool GameTimer::initialize(bool useHighResolution, int numThreads) {
    if (m_initialized) {
        return true;
    }
    
    m_useHighResolution = useHighResolution;
    m_numThreads = std::max(1, numThreads);
    
    m_totalTriggered = 0;
    m_totalExecutionTime = 0;
    m_missedDeadlines = 0;
    
    m_timeScale = 1.0f;
    m_gameTime = 0;
    m_gameTimePaused = false;
    m_pausedGameTime = 0;
    m_gameStartTime = std::chrono::steady_clock::now();
    
    m_active = true;
    
    // Iniciar threads de processamento
    m_threads.resize(m_numThreads);
    for (int i = 0; i < m_numThreads; i++) {
        m_threads[i] = std::thread(&GameTimer::timerThread, this);
    }
    
    m_initialized = true;
    return true;
}

// Finaliza o sistema de timer
void GameTimer::shutdown() {
    if (!m_initialized) {
        return;
    }
    
    m_active = false;
    
    // Notificar threads para finalizar
    m_timersCV.notify_all();
    
    // Aguardar /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param m_threads Parâmetro m_threads
 * @return Retorna threads
 */
 threads
    for(auto& thread : m_threads){
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    // Limpar timers
    std::unique_lock<std::mutex> lock(m_timersMutex);
    
    while (!m_timerQueue.empty()) {
        m_timerQueue.pop();
    }
    
    m_timers.clear();
    
    m_initialized = false;
}

// Cria um timer de uso único
TimerId GameTimer::createOneShotTimer(std::function<void()> callback, int delayMs, const std::string& description) {
    if (!m_initialized || !callback || delayMs < 0) {
        return -1;
    }
    
    TimerId timerId = getNextTimerId();
    
    // Calcular tempo de acionamento
    auto triggerTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(delayMs);
    
    // Criar timer
    auto timer = std::make_shared<Timer>(
        timerId,
        std::move(callback),
        triggerTime,
        std::chrono::milliseconds(0),
        description,
        false,
        1
    );
    
    // Adicionar à fila e mapa
    {
        std::unique_lock<std::mutex> lock(m_timersMutex);
        
        m_timerQueue.push(timer);
        m_timers[timerId] = timer;
    }
    
    // Notificar uma thread de processamento
    m_timersCV.notify_one();
    
    return timerId;
}

// Cria um timer periódico
TimerId GameTimer::createPeriodicTimer(std::function<void()> callback, int intervalMs, 
                                      const std::string& description, int initialDelayMs, 
                                      int repeatCount) {
    if (!m_initialized || !callback || intervalMs <= 0) {
        return -1;
    }
    
    TimerId timerId = getNextTimerId();
    
    // Calcular tempo de acionamento inicial
    auto now = std::chrono::steady_clock::now();
    auto triggerTime = now + std::chrono::milliseconds(initialDelayMs > 0 ? initialDelayMs : intervalMs);
    
    // Criar timer
    auto timer = std::make_shared<Timer>(
        timerId,
        std::move(callback),
        triggerTime,
        std::chrono::milliseconds(intervalMs),
        description,
        true,
        repeatCount
    );
    
    // Adicionar à fila e mapa
    {
        std::unique_lock<std::mutex> lock(m_timersMutex);
        
        m_timerQueue.push(timer);
        m_timers[timerId] = timer;
    }
    
    // Notificar uma thread de processamento
    m_timersCV.notify_one();
    
    return timerId;
}

// Cancela um timer
bool GameTimer::cancelTimer(TimerId timerId) {
    if (!m_initialized) {
        return false;
    }
    
    std::unique_lock<std::mutex> lock(m_timersMutex);
    
    auto it = m_timers.find(timerId);
    if (it == m_timers.end()) {
        return false;
    }
    
    // Marcar timer como inativo
    it->second->isActive = false;
    
    // Não podemos remover da fila de prioridade diretamente,
    // será removido na próxima verificação
    
    return true;
}

// Pausa um timer
bool GameTimer::pauseTimer(TimerId timerId) {
    if (!m_initialized) {
        return false;
    }
    
    std::unique_lock<std::mutex> lock(m_timersMutex);
    
    auto it = m_timers.find(timerId);
    if (it == m_timers.end() || !it->second->isActive) {
        return false;
    }
    
    // Pausar timer
    it->second->isPaused = true;
    
    return true;
}

// Retoma um timer pausado
bool GameTimer::resumeTimer(TimerId timerId) {
    if (!m_initialized) {
        return false;
    }
    
    std::unique_lock<std::mutex> lock(m_timersMutex);
    
    auto it = m_timers.find(timerId);
    if (it == m_timers.end() || !it->second->isActive || !it->second->isPaused) {
        return false;
    }
    
    // Retomar timer - ajustar próximo acionamento
    auto now = std::chrono::steady_clock::now();
    it->second->nextTrigger = now + it->second->interval;
    it->second->isPaused = false;
    
    // Reordenar fila
    std::vector<std::shared_ptr<Timer>> timers;
    
    while (!m_timerQueue.empty()) {
        auto timer = m_timerQueue.top();
        m_timerQueue.pop();
        
        if (timer->id == timerId) {
            // Substituir pelo timer atualizado
            timers.push_back(it->second);
        }
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param timer->isActive Parâmetro timer->isActive
 * @return Retorna else
 */

        else if(timer->isActive){
            timers.push_back(timer);
        }
    }
    
    // Reconstruir /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param timers Parâmetro timers
 * @return Retorna fila
 */
 fila
    for(const auto& timer : timers){
        m_timerQueue.push(timer);
    }
    
    // Notificar uma thread de processamento
    lock.unlock();
    m_timersCV.notify_one();
    
    return true;
}

// Redefine um timer periódico
bool GameTimer::resetTimer(TimerId timerId, int newIntervalMs) {
    if (!m_initialized || newIntervalMs <= 0) {
        return false;
    }
    
    std::unique_lock<std::mutex> lock(m_timersMutex);
    
    auto it = m_timers.find(timerId);
    if (it == m_timers.end() || !it->second->isActive || !it->second->isPeriodic) {
        return false;
    }
    
    // Atualizar intervalo e próximo acionamento
    it->second->interval = std::chrono::milliseconds(newIntervalMs);
    
    auto now = std::chrono::steady_clock::now();
    it->second->nextTrigger = now + it->second->interval;
    
    // Reordenar fila
    std::vector<std::shared_ptr<Timer>> timers;
    
    while (!m_timerQueue.empty()) {
        auto timer = m_timerQueue.top();
        m_timerQueue.pop();
        
        if (timer->id == timerId) {
            // Substituir pelo timer atualizado
            timers.push_back(it->second);
        }
        /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param timer->isActive Parâmetro timer->isActive
 * @return Retorna else
 */

        else if(timer->isActive){
            timers.push_back(timer);
        }
    }
    
    // Reconstruir /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param timers Parâmetro timers
 * @return Retorna fila
 */
 fila
    for(const auto& timer : timers){
        m_timerQueue.push(timer);
    }
    
    // Notificar uma thread de processamento
    lock.unlock();
    m_timersCV.notify_one();
    
    return true;
}

// Verifica se um timer está ativo
bool GameTimer::isTimerActive(TimerId timerId) {
    if (!m_initialized) {
        return false;
    }
    
    std::unique_lock<std::mutex> lock(m_timersMutex);
    
    auto it = m_timers.find(timerId);
    return (it != m_timers.end() && it->second->isActive);
}

// Obtém tempo restante de um timer
int64_t GameTimer::getTimeRemaining(TimerId timerId) {
    if (!m_initialized) {
        return -1;
    }
    
    std::unique_lock<std::mutex> lock(m_timersMutex);
    
    auto it = m_timers.find(timerId);
    if (it == m_timers.end() || !it->second->isActive) {
        return -1;
    }
    
    auto now = std::chrono::steady_clock::now();
    
    if (it->second->nextTrigger <= now) {
        return 0;
    }
    
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        it->second->nextTrigger - now).count();
}

// Obtém descrição de um timer
std::string GameTimer::getTimerDescription(TimerId timerId) {
    if (!m_initialized) {
        return "";
    }
    
    std::unique_lock<std::mutex> lock(m_timersMutex);
    
    auto it = m_timers.find(timerId);
    if (it == m_timers.end()) {
        return "";
    }
    
    return it->second->description;
}

// Obtém estatísticas do sistema de timer
void GameTimer::getStats(int& activeTimers, uint64_t& totalTriggered, 
                        float& avgExecutionTime, uint64_t& missedDeadlines) {
    if (!m_initialized) {
        activeTimers = 0;
        totalTriggered = 0;
        avgExecutionTime = 0.0f;
        missedDeadlines = 0;
        return;
    }
    
    // Contar timers ativos
    {
        std::unique_lock<std::mutex> lock(m_timersMutex);
        
        activeTimers = 0;
        for (const auto& pair : m_timers) {
            if (pair.second->isActive) {
                activeTimers++;
            }
        }
    }
    
    totalTriggered = m_totalTriggered;
    missedDeadlines = m_missedDeadlines;
    
    if (totalTriggered > 0) {
        avgExecutionTime = static_cast<float>(m_totalExecutionTime) / totalTriggered / 1000.0f; // us to ms
    }
    else {
        avgExecutionTime = 0.0f;
    }
}

// Limpa estatísticas do sistema de timer
void GameTimer::clearStats() {
    m_totalTriggered = 0;
    m_totalExecutionTime = 0;
    m_missedDeadlines = 0;
}

// Define a taxa de tempo do jogo
void GameTimer::setTimeScale(float timeScale) {
    if (timeScale < 0.0f) {
        timeScale = 0.0f;
    }
    
    // Atualizar tempo de jogo antes de mudar a /**
 * updateGameTime
 * 
 * Implementa a funcionalidade updateGameTime conforme especificação original.
 * @return Retorna escala
 */
 escala
    updateGameTime();
    
    m_timeScale = timeScale;
}

// Obtém a taxa de tempo atual
float GameTimer::getTimeScale() const {
    return m_timeScale;
}

// Obtém o tempo do jogo
uint64_t GameTimer::getGameTime() const {
    if (m_gameTimePaused) {
        return m_pausedGameTime;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto realTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - m_gameStartTime).count();
    
    return static_cast<uint64_t>(realTime * m_timeScale);
}

// Obtém o tempo real
uint64_t GameTimer::getRealTime() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        now - m_gameStartTime).count();
}

// Converte tempo do jogo para tempo real
uint64_t GameTimer::gameToRealTime(uint64_t gameTime) const {
    float timeScale = m_timeScale;
    if (timeScale < 0.001f) {
        timeScale = 0.001f; // Evitar divisão por zero
    }
    
    return static_cast<uint64_t>(gameTime / timeScale);
}

// Converte tempo real para tempo do jogo
uint64_t GameTimer::realToGameTime(uint64_t realTime) const {
    return static_cast<uint64_t>(realTime * m_timeScale);
}

// Define o tempo do jogo manualmente
void GameTimer::setGameTime(uint64_t gameTime) {
    if (m_gameTimePaused) {
        m_pausedGameTime = gameTime;
    }
    else {
        // Recalcular tempo inicial para coincidir com o tempo fornecido
        auto now = std::chrono::steady_clock::now();
        uint64_t realTime = static_cast<uint64_t>(gameTime / m_timeScale);
        
        auto duration = std::chrono::milliseconds(realTime);
        m_gameStartTime = now - duration;
    }
}

// Pausa o tempo do jogo
void GameTimer::pauseGameTime() {
    if (m_gameTimePaused) {
        return;
    }
    
    m_gameTimePaused = true;
    m_pauseStartTime = std::chrono::steady_clock::now();
    m_pausedGameTime = getGameTime();
}

// Retoma o tempo do jogo
void GameTimer::resumeGameTime() {
    if (!m_gameTimePaused) {
        return;
    }
    
    m_gameTimePaused = false;
    
    // Ajustar tempo inicial para preservar o tempo pausado
    auto now = std::chrono::steady_clock::now();
    uint64_t pauseDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - m_pauseStartTime).count();
    
    // Avançar o tempo inicial pela duração da pausa
    m_gameStartTime += std::chrono::milliseconds(pauseDuration);
}

// Verifica se o tempo do jogo está pausado
bool GameTimer::isGameTimePaused() const {
    return m_gameTimePaused;
}

// Thread de processamento de timers
void GameTimer::timerThread() {
    while (m_active) {
        checkAndTriggerTimers();
        
        // Atualizar tempo de /**
 * updateGameTime
 * 
 * Implementa a funcionalidade updateGameTime conforme especificação original.
 * @return Retorna jogo
 */
 jogo
        updateGameTime();
    }
}

// Verifica e dispara timers
void GameTimer::checkAndTriggerTimers() {
    auto now = std::chrono::steady_clock::now();
    std::vector<std::shared_ptr<Timer>> timersToTrigger;
    
    // Verificar timers prontos para disparar
    {
        std::unique_lock<std::mutex> lock(m_timersMutex);
        
        // Se não há timers, aguardar
        if (m_timerQueue.empty()) {
            m_timersCV.wait(lock, [this] {
                return !m_active || !m_timerQueue.empty();
            });
            
            if (!m_active) {
                return;
            }
        }
        else {
            // Verificar quando o próximo timer deve disparar
            auto nextTimer = m_timerQueue.top();
            
            if (nextTimer->nextTrigger > now) {
                // Aguardar até o próximo disparo ou notificação
                m_timersCV.wait_until(lock, nextTimer->nextTrigger, [this, nextTimer, now] {
                    return !m_active || m_timerQueue.empty() || 
                           m_timerQueue.top()->nextTrigger <= now;
                });
                
                if (!m_active) {
                    return;
                }
            }
        }
        
        // Coletar timers prontos para disparar
        while (!m_timerQueue.empty()) {
            auto timer = m_timerQueue.top();
            
            if (!timer->isActive) {
                // Remover timers inativos
                m_timerQueue.pop();
                continue;
            }
            
            if (timer->isPaused) {
                // Pular timers pausados
                break;
            }
            
            if (timer->nextTrigger <= now) {
                // Timer pronto para disparar
                m_timerQueue.pop();
                
                // Verificar se o prazo foi perdido (mais de 100ms de atraso)
                auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - timer->nextTrigger).count();
                
                if (delay > 100) {
                    m_missedDeadlines++;
                }
                
                // Recalcular próximo disparo para timers perió/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param timer->isPeriodic Parâmetro timer->isPeriodic
 * @return Retorna dicos
 */
dicos
                if(timer->isPeriodic){
                    // Reduzir contador de repetiçõ/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna es
 */
es
                    if(timer->repeatCount > 0){
                        timer->repeatCount--;
                        
                        if (timer->repeatCount == 0) {
                            // Último disparo, marcar como inativo após execução
                            timersToTrigger.push_back(timer);
                            timer->isActive = false;
                            continue;
                        }
                    }
                    
                    // Calcular próximo disparo e readicionar à /**
 * calculateNextTrigger
 * 
 * Implementa a funcionalidade calculateNextTrigger conforme especificação original.
 * @param timer Parâmetro timer
 * @return Retorna fila
 */
 fila
                    calculateNextTrigger(timer);
                    m_timerQueue.push(timer);
                }
                else {
                    // Timer de uso único, marcar como inativo após execução
                    timer->isActive = false;
                }
                
                // Adicionar à lista de disparo
                timersToTrigger.push_back(timer);
            }
            else {
                // Próximo timer ainda não está pronto
                break;
            }
        }
    }
    
    // Disparar timers /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param timersToTrigger Parâmetro timersToTrigger
 * @return Retorna coletados
 */
 coletados
    for(const auto& timer : timersToTrigger){
        if (timer->callback) {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            try {
                timer->callback();
            }
            catch (const std::exception& e) {
                // Capturar exceções para evitar que timers quebrem o sistema
                std::cerr << "Exceção em timer " << timer->id 
                          << " (" << timer->description << "): " 
                          << e.what() << std::endl;
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto executionTime = std::chrono::duration_cast<std::chrono::microseconds>(
                endTime - startTime).count();
            
            // Atualizar estatísticas
            m_totalTriggered++;
            m_totalExecutionTime += executionTime;
        }
    }
}

// Calcula próximo acionamento para timer periódico
void GameTimer::calculateNextTrigger(std::shared_ptr<Timer> timer) {
    auto now = std::chrono::steady_clock::now();
    
    // Verificar se o timer está muito /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param now Parâmetro now
 * @return Retorna atrasado
 */
 atrasado
    if(timer->nextTrigger + timer->interval < now){
        // Mais de um intervalo atrasado, ajustar para o próximo intervalo a partir de agora
        timer->nextTrigger = now + timer->interval;
    }
    else {
        // Adicionar um intervalo ao disparo anterior
        timer->nextTrigger += timer->interval;
    }
}

// Atualiza tempos de jogo
void GameTimer::updateGameTime() {
    if (!m_gameTimePaused) {
        m_gameTime = getGameTime();
    }
}

// Obtém próximo ID de timer
TimerId GameTimer::getNextTimerId() {

} // namespace wydbr

    return m_nextTimerId++;
}
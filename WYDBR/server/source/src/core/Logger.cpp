/**
 * Logger.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/core/Logger.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "Logger.h"
#include <algorithm>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <filesystem>

namespace wydbr {


/**
 * @file Logger.cpp
 * @brief Implementação do sistema de logging avançado para o WYDBRASIL
 * 
 * Sistema de logging flexível e configurável para rastreamento de eventos,
 * depuração e monitoramento do servidor.
 */

// ==================== ConsoleSink ====================

ConsoleSink::ConsoleSink(LogLevel minLevel, bool colorized)
    : m_minLevel(minLevel), m_colorized(colorized) {
}

void ConsoleSink::process(const LogEntry& entry) {
    if (!acceptsLevel(entry.level)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::string timestamp = Logger::formatTimestamp(entry.timestamp);
    std::string levelStr = Logger::levelToString(entry.level);
    
    std::ostringstream oss;
    oss << "[" << timestamp << "] [" << levelStr << "]";
    
    if (!entry.component.empty()) {
        oss << " [" << entry.component << "]";
    }
    
    oss << " " << entry.message;
    
    if (entry.line > 0) {
        oss << " (" << entry.file << ":" << entry.line << ")";
    }
    
    // Adicionar atributos
    if (!entry.attributes.empty()) {
        oss << " {";
        bool first = true;
        for (const auto& attr : entry.attributes) {
            if (!first) {
                oss << ", ";
            }
            first = false;
            oss << attr.first << "='" << attr.second << "'";
        }
        oss << "}";
    }
    
    // Colorizaçã/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_colorized Parâmetro m_colorized
 * @return Retorna o
 */
o
    if(m_colorized){
        std::string colorCode;
        std::string resetCode = "\033[0m";
        
        switch (entry.level) {
            case LogLevel::TRACE:
                colorCode = "\033[90m"; // Cinza
                break;
            case LogLevel::DEBUG:
                colorCode = "\033[94m"; // Azul claro
                break;
            case LogLevel::INFO:
                colorCode = "\033[92m"; // Verde
                break;
            case LogLevel::WARNING:
                colorCode = "\033[93m"; // Amarelo
                break;
            case LogLevel::ERROR:
                colorCode = "\033[91m"; // Vermelho
                break;
            case LogLevel::CRITICAL:
                colorCode = "\033[41m\033[97m"; // Fundo vermelho, texto branco
                break;
            default:
                colorCode = "";
                resetCode = "";
        }
        
        std::cout << colorCode << oss.str() << resetCode << std::endl;
    }
    else {
        std::cout << oss.str() << std::endl;
    }
}

bool ConsoleSink::acceptsLevel(LogLevel level) const {
    return level >= m_minLevel;
}

void ConsoleSink::close() {
    // Nada a fazer para sink do console
}

// ==================== FileSink ====================

FileSink::FileSink(const std::string& filename, LogLevel minLevel, bool rotate, size_t maxSize, int maxFiles)
    : m_filename(filename), m_minLevel(minLevel), m_rotate(rotate), 
      m_maxSize(maxSize), m_maxFiles(maxFiles), m_currentSize(0) {
    
    openFile();
}

bool FileSink::openFile() {
    // Criar diretório se não existir
    /**
 * filePath
 * 
 * Implementa a funcionalidade filePath conforme especificação original.
 * @param m_filename Parâmetro m_filename
 * @return Retorna std::filesystem::path
 */

    std::filesystem::path filePath(m_filename);
    std::filesystem::path directory = filePath.parent_path();
    
    if (!directory.empty() && !std::filesystem::exists(directory)) {
        std::filesystem::create_directories(directory);
    }
    
    // Abrir arquivo
    m_file.open(m_filename, std::ios::app);
    
    if (!m_file) {
        return false;
    }
    
    // Obter tamanho atual
    m_file.seekp(0, std::ios::end);
    m_currentSize = static_cast<size_t>(m_file.tellp());
    
    return true;
}

void FileSink::rotateFiles() {
    // Fechar arquivo atual
    m_file.close();
    
    // Rotacionar /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i-- Parâmetro i--
 * @return Retorna arquivos
 */
 arquivos
    for(int i = m_maxFiles - 1; i > 0; i--){
        std::string oldFile = m_filename + "." + std::to_string(i);
        std::string newFile = m_filename + "." + std::to_string(i + 1);
        
        if (std::filesystem::exists(oldFile)) {
            if (i == m_maxFiles - 1) {
                // Remover arquivo mais antigo
                std::filesystem::remove(oldFile);
            }
            else {
                // Renomear arquivo
                std::filesystem::rename(oldFile, newFile);
            }
        }
    }
    
    // Renomear arquivo atual
    std::string newFile = m_filename + ".1";
    std::filesystem::rename(m_filename, newFile);
    
    // Reabrir /**
 * openFile
 * 
 * Implementa a funcionalidade openFile conforme especificação original.
 * @return Retorna arquivo
 */
 arquivo
    openFile();
    m_currentSize = 0;
}

void FileSink::process(const LogEntry& entry) {
    if (!acceptsLevel(entry.level)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_file.is_open() && !openFile()) {
        return;
    }
    
    std::string timestamp = Logger::formatTimestamp(entry.timestamp);
    std::string levelStr = Logger::levelToString(entry.level);
    
    std::ostringstream oss;
    oss << "[" << timestamp << "] [" << levelStr << "]";
    
    if (!entry.component.empty()) {
        oss << " [" << entry.component << "]";
    }
    
    oss << " " << entry.message;
    
    if (entry.line > 0) {
        oss << " (" << entry.file << ":" << entry.line << ")";
    }
    
    // Adicionar atributos
    if (!entry.attributes.empty()) {
        oss << " {";
        bool first = true;
        for (const auto& attr : entry.attributes) {
            if (!first) {
                oss << ", ";
            }
            first = false;
            oss << attr.first << "='" << attr.second << "'";
        }
        oss << "}";
    }
    
    oss << std::endl;
    
    std::string message = oss.str();
    m_file << message;
    m_file.flush();
    
    m_currentSize += message.size();
    
    // Rotacionar arquivo se necessá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_maxSize Parâmetro m_maxSize
 * @return Retorna rio
 */
rio
    if(m_rotate && m_currentSize >= m_maxSize){
        rotateFiles();
    }
}

bool FileSink::acceptsLevel(LogLevel level) const {
    return level >= m_minLevel;
}

void FileSink::close() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_file.is_open()) {
        m_file.flush();
        m_file.close();
    }
}

// ==================== CallbackSink ====================

CallbackSink::CallbackSink(Callback callback, LogLevel minLevel)
    : m_callback(std::move(callback)), m_minLevel(minLevel) {
}

void CallbackSink::process(const LogEntry& entry) {
    if (!acceptsLevel(entry.level)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_callback) {
        m_callback(entry);
    }
}

bool CallbackSink::acceptsLevel(LogLevel level) const {
    return level >= m_minLevel;
}

void CallbackSink::close() {
    // Nada a fazer para sink de callback
}

// ==================== Logger ====================

// Implementação do singleton
Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

// Construtor
Logger::Logger() 
    : m_globalLevel(LogLevel::INFO),
      m_asyncMode(true),
      m_asyncThreadActive(false),
      m_flushIntervalMs(1000),
      m_enabled(true),
      m_initialized(false) {
}

// Destrutor
Logger::~Logger() {
    shutdown();
}

// Inicializa o sistema de log
bool Logger::initialize(LogLevel defaultLevel, bool asyncMode, int flushInterval) {
    if (m_initialized) {
        return true;
    }
    
    m_globalLevel = defaultLevel;
    m_asyncMode = asyncMode;
    m_flushIntervalMs = flushInterval;
    m_enabled = true;
    
    // Iniciar thread assíncrona se necessá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_asyncMode Parâmetro m_asyncMode
 * @return Retorna rio
 */
rio
    if(m_asyncMode){
        m_asyncThreadActive = true;
        m_asyncThread = std::thread(&Logger::asyncThread, this);
    }
    
    m_initialized = true;
    return true;
}

// Finaliza o sistema de log
void Logger::shutdown() {
    if (!m_initialized) {
        return;
    }
    
    // Finalizar thread assí/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_asyncThreadActive Parâmetro m_asyncThreadActive
 * @return Retorna ncrona
 */
ncrona
    if(m_asyncMode && m_asyncThreadActive){
        m_asyncThreadActive = false;
        m_asyncQueueCV.notify_all();
        
        if (m_asyncThread.joinable()) {
            m_asyncThread.join();
        }
    }
    
    // Processar qualquer log pendente
    {
        std::lock_guard<std::mutex> lock(m_asyncQueueMutex);
        
        while (!m_asyncQueue.empty()) {
            processLogEntry(m_asyncQueue.front());
            m_asyncQueue.pop();
        }
    }
    
    // Fechar todos os sinks
    std::lock_guard<std::mutex> lock(m_sinksMutex);
    
    for (auto& sink : m_sinks) {
        sink->close();
    }
    
    m_sinks.clear();
    
    m_initialized = false;
}

// Adiciona um sink ao logger
void Logger::addSink(std::shared_ptr<LogSink> sink) {
    if (!m_initialized || !sink) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_sinksMutex);
    m_sinks.push_back(sink);
}

// Remove um sink do logger
bool Logger::removeSink(std::shared_ptr<LogSink> sink) {
    if (!m_initialized || !sink) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_sinksMutex);
    
    auto it = std::find(m_sinks.begin(), m_sinks.end(), sink);
    if (it != m_sinks.end()) {
        (*it)->close();
        m_sinks.erase(it);
        return true;
    }
    
    return false;
}

// Define o nível global mínimo para logar
void Logger::setGlobalLevel(LogLevel level) {
    m_globalLevel = level;
}

// Define o nível mínimo para um componente específico
void Logger::setComponentLevel(const std::string& component, LogLevel level) {
    if (component.empty()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_levelsMutex);
    m_componentLevels[component] = level;
}

// Obtém o nível mínimo para um componente específico
LogLevel Logger::getComponentLevel(const std::string& component) const {
    if (component.empty()) {
        return m_globalLevel;
    }
    
    std::lock_guard<std::mutex> lock(m_levelsMutex);
    
    auto it = m_componentLevels.find(component);
    if (it != m_componentLevels.end()) {
        return it->second;
    }
    
    return m_globalLevel;
}

// Cria um sink para console
std::shared_ptr<LogSink> Logger::createConsoleSink(LogLevel minLevel, bool colorized) {
    return std::make_shared<ConsoleSink>(minLevel, colorized);
}

// Cria um sink para arquivo
std::shared_ptr<LogSink> Logger::createFileSink(const std::string& filename, LogLevel minLevel,
                                              bool rotate, size_t maxSize, int maxFiles) {
    return std::make_shared<FileSink>(filename, minLevel, rotate, maxSize, maxFiles);
}

// Cria um sink para callback
std::shared_ptr<LogSink> Logger::createCallbackSink(CallbackSink::Callback callback, LogLevel minLevel) {
    return std::make_shared<CallbackSink>(callback, minLevel);
}

// Loga uma mensagem
void Logger::log(LogLevel level, const std::string& message, const std::string& component,
               const std::string& file, int line) {
    if (!m_initialized || !m_enabled || !isLevelEnabled(level, component)) {
        return;
    }
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.component = component;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::this_thread::get_id();
    entry.file = file;
    entry.line = line;
    
    if (m_asyncMode) {
        // Modo assíncrono
        std::lock_guard<std::mutex> lock(m_asyncQueueMutex);
        m_asyncQueue.push(entry);
        m_asyncQueueCV.notify_one();
    }
    else {
        // Modo sí/**
 * processLogEntry
 * 
 * Implementa a funcionalidade processLogEntry conforme especificação original.
 * @param entry Parâmetro entry
 * @return Retorna ncrono
 */
ncrono
        processLogEntry(entry);
    }
}

// Loga uma mensagem com atributos adicionais
void Logger::logWithAttributes(LogLevel level, const std::string& message, 
                             const std::unordered_map<std::string, std::string>& attributes,
                             const std::string& component, const std::string& file, int line) {
    if (!m_initialized || !m_enabled || !isLevelEnabled(level, component)) {
        return;
    }
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.component = component;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::this_thread::get_id();
    entry.file = file;
    entry.line = line;
    entry.attributes = attributes;
    
    if (m_asyncMode) {
        // Modo assíncrono
        std::lock_guard<std::mutex> lock(m_asyncQueueMutex);
        m_asyncQueue.push(entry);
        m_asyncQueueCV.notify_one();
    }
    else {
        // Modo sí/**
 * processLogEntry
 * 
 * Implementa a funcionalidade processLogEntry conforme especificação original.
 * @param entry Parâmetro entry
 * @return Retorna ncrono
 */
ncrono
        processLogEntry(entry);
    }
}

// Loga uma mensagem com nível TRACE
void Logger::trace(const std::string& message, const std::string& component,
                 const std::string& file, int line) {
    log(LogLevel::TRACE, message, component, file, line);
}

// Loga uma mensagem com nível DEBUG
void Logger::debug(const std::string& message, const std::string& component,
                 const std::string& file, int line) {
    log(LogLevel::DEBUG, message, component, file, line);
}

// Loga uma mensagem com nível INFO
void Logger::info(const std::string& message, const std::string& component,
                const std::string& file, int line) {
    log(LogLevel::INFO, message, component, file, line);
}

// Loga uma mensagem com nível WARNING
void Logger::warning(const std::string& message, const std::string& component,
                   const std::string& file, int line) {
    log(LogLevel::WARNING, message, component, file, line);
}

// Loga uma mensagem com nível ERROR
void Logger::error(const std::string& message, const std::string& component,
                 const std::string& file, int line) {
    log(LogLevel::ERROR, message, component, file, line);
}

// Loga uma mensagem com nível CRITICAL
void Logger::critical(const std::string& message, const std::string& component,
                    const std::string& file, int line) {
    log(LogLevel::CRITICAL, message, component, file, line);
}

// Formata um timestamp para string
std::string Logger::formatTimestamp(const std::chrono::system_clock::time_point& timestamp,
                                   const std::string& format) {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm = *std::localtime_r(&time);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, format.c_str());
    
    // Adicionar milissegundos
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()).count() % 1000;
    
    oss << "." << std::setfill('0') << std::setw(3) << millis;
    
    return oss.str();
}

// Converte um nível de log para string
std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE:
            return "TRACE";
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::CRITICAL:
            return "CRITICAL";
        case LogLevel::SILENT:
            return "SILENT";
        default:
            return "UNKNOWN";
    }
}

// Converte uma string para nível de log
LogLevel Logger::stringToLevel(const std::string& levelStr, LogLevel defaultLevel) {
    std::string upperStr = levelStr;
    std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(),
                 [](unsigned char c) { return std::toupper(c); });
    
    if (upperStr == "TRACE") {
        return LogLevel::TRACE;
    }
    /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "DEBUG" Parâmetro "DEBUG"
 * @return Retorna else
 */

    else if(upperStr == "DEBUG"){
        return LogLevel::DEBUG;
    }
    /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "INFO" Parâmetro "INFO"
 * @return Retorna else
 */

    else if(upperStr == "INFO"){
        return LogLevel::INFO;
    }
    /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "WARN" Parâmetro "WARN"
 * @return Retorna else
 */

    else if(upperStr == "WARNING" || upperStr == "WARN"){
        return LogLevel::WARNING;
    }
    /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "ERR" Parâmetro "ERR"
 * @return Retorna else
 */

    else if(upperStr == "ERROR" || upperStr == "ERR"){
        return LogLevel::ERROR;
    }
    /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "CRIT" Parâmetro "CRIT"
 * @return Retorna else
 */

    else if(upperStr == "CRITICAL" || upperStr == "CRIT"){
        return LogLevel::CRITICAL;
    }
    /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param "SILENT" Parâmetro "SILENT"
 * @return Retorna else
 */

    else if(upperStr == "SILENT"){
        return LogLevel::SILENT;
    }
    
    return defaultLevel;
}

// Força o flush de todos os sinks
void Logger::flush() {
    if (!m_initialized) {
        return;
    }
    
    // Processar logs pendentes no modo assí/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param m_asyncMode Parâmetro m_asyncMode
 * @return Retorna ncrono
 */
ncrono
    if(m_asyncMode){
        std::vector<LogEntry> pendingEntries;
        
        {
            std::lock_guard<std::mutex> lock(m_asyncQueueMutex);
            
            while (!m_asyncQueue.empty()) {
                pendingEntries.push_back(m_asyncQueue.front());
                m_asyncQueue.pop();
            }
        }
        
        for (const auto& entry : pendingEntries) {
            processLogEntry(entry);
        }
    }
    
    // Flush dos sinks
    std::lock_guard<std::mutex> lock(m_sinksMutex);
    
    for (auto& sink : m_sinks) {
        // FileSink já faz flush após cada mensagem
        // Outros tipos de sink podem precisar de implementação específica
    }
}

// Define o intervalo de flush automático (modo assíncrono)
void Logger::setFlushInterval(int intervalMs) {
    if (intervalMs > 0) {
        m_flushIntervalMs = intervalMs;
    }
}

// Habilita ou desabilita todas as mensagens de log
void Logger::setEnabled(bool enabled) {
    m_enabled = enabled;
}

// Verifica se o logger está habilitado
bool Logger::isEnabled() const {
    return m_enabled;
}

// Verifica se um nível de log está habilitado para um componente
bool Logger::isLevelEnabled(LogLevel level, const std::string& component) const {
    if (!m_enabled) {
        return false;
    }
    
    LogLevel minLevel = getComponentLevel(component);
    return level >= minLevel;
}

// Thread de processamento assíncrono
void Logger::asyncThread() {
    while (m_asyncThreadActive) {
        std::vector<LogEntry> entries;
        
        {
            std::unique_lock<std::mutex> lock(m_asyncQueueMutex);
            
            // Esperar por novos logs ou timeout
            m_asyncQueueCV.wait_for(lock, std::chrono::milliseconds(m_flushIntervalMs),
                                  [this] { return !m_asyncThreadActive || !m_asyncQueue.empty(); });
            
            if (!m_asyncThreadActive && m_asyncQueue.empty()) {
                break;
            }
            
            // Coletar todos os logs atuais
            while (!m_asyncQueue.empty()) {
                entries.push_back(m_asyncQueue.front());
                m_asyncQueue.pop();
            }
        }
        
        // Processar logs /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param entries Parâmetro entries
 * @return Retorna coletados
 */
 coletados
        for(const auto& entry : entries){
            processLogEntry(entry);
        }
    }
}

// Processa uma entrada de log
void Logger::processLogEntry(const LogEntry& entry) {

} // namespace wydbr

    std::lock_guard<std::mutex> lock(m_sinksMutex);
    
    for (auto& sink : m_sinks) {
        sink->process(entry);
    }
}
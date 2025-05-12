#include "../../include/GameFileManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <regex>
#include <filesystem>
#include <algorithm>
#include <set>
#include <json/json.hpp> // Usando nlohmann/json para C++

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace WYDStudio {

/**
 * @brief Obtém um timestamp atual como int64
 * @return Timestamp em milissegundos
 */
static int64_t GetCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_point::time_since_epoch()).count();
}

/**
 * @brief Converte um timestamp para uma string formatada
 * @param timestamp Timestamp a ser convertido
 * @return String formatada (YYYY-MM-DD HH:MM:SS)
 */
static std::string FormatTimestamp(int64_t timestamp) {
    std::time_t time = timestamp / 1000;
    std::tm* timeInfo = std::localtime(&time);
    
    std::stringstream ss;
    ss << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

/**
 * @brief Converte uma operação de arquivo para string
 * @param operation Operação a ser convertida
 * @return String representando a operação
 */
static std::string OperationToString(FileOperation operation) {
    switch (operation) {
        case FileOperation::CREATE:
            return "CREATE";
        case FileOperation::MODIFY:
            return "MODIFY";
        case FileOperation::DELETE:
            return "DELETE";
        case FileOperation::MOVE:
            return "MOVE";
        case FileOperation::COPY:
            return "COPY";
        case FileOperation::COMPILE:
            return "COMPILE";
        case FileOperation::DECOMPILE:
            return "DECOMPILE";
        default:
            return "UNKNOWN";
    }
}

/**
 * @brief Converte uma string para operação de arquivo
 * @param operationStr String a ser convertida
 * @return Operação correspondente
 */
static FileOperation StringToOperation(const std::string& operationStr) {
    if (operationStr == "CREATE") return FileOperation::CREATE;
    if (operationStr == "MODIFY") return FileOperation::MODIFY;
    if (operationStr == "DELETE") return FileOperation::DELETE;
    if (operationStr == "MOVE") return FileOperation::MOVE;
    if (operationStr == "COPY") return FileOperation::COPY;
    if (operationStr == "COMPILE") return FileOperation::COMPILE;
    if (operationStr == "DECOMPILE") return FileOperation::DECOMPILE;
    
    return FileOperation::MODIFY; // Default
}

/**
 * @brief Converte um status de servidor para string
 * @param status Status a ser convertido
 * @return String representando o status
 */
static std::string ServerStatusToString(ServerFileStatus status) {
    switch (status) {
        case ServerFileStatus::NOT_APPLICABLE:
            return "NOT_APPLICABLE";
        case ServerFileStatus::IN_SYNC:
            return "IN_SYNC";
        case ServerFileStatus::MODIFIED_LOCALLY:
            return "MODIFIED_LOCALLY";
        case ServerFileStatus::MODIFIED_ON_SERVER:
            return "MODIFIED_ON_SERVER";
        case ServerFileStatus::CONFLICT:
            return "CONFLICT";
        case ServerFileStatus::UNKNOWN:
        default:
            return "UNKNOWN";
    }
}

/**
 * @brief Converte uma string para status de servidor
 * @param statusStr String a ser convertida
 * @return Status correspondente
 */
static ServerFileStatus StringToServerStatus(const std::string& statusStr) {
    if (statusStr == "NOT_APPLICABLE") return ServerFileStatus::NOT_APPLICABLE;
    if (statusStr == "IN_SYNC") return ServerFileStatus::IN_SYNC;
    if (statusStr == "MODIFIED_LOCALLY") return ServerFileStatus::MODIFIED_LOCALLY;
    if (statusStr == "MODIFIED_ON_SERVER") return ServerFileStatus::MODIFIED_ON_SERVER;
    if (statusStr == "CONFLICT") return ServerFileStatus::CONFLICT;
    
    return ServerFileStatus::UNKNOWN;
}

/**
 * @brief Verifica se um arquivo está no diretório especificado
 * @param filePath Caminho do arquivo
 * @param directory Diretório
 * @return true se o arquivo está no diretório
 */
static bool IsFileInDirectory(const std::string& filePath, const std::string& directory) {
    fs::path file(filePath);
    fs::path dir(directory);
    
    // Normalizar para caminhos absolutos
    file = fs::absolute(file);
    dir = fs::absolute(dir);
    
    // Verificar se o arquivo está no diretório
    std::string fileStr = file.string();
    std::string dirStr = dir.string();
    
    // Garantir que o diretório termine com separador
    if (!dirStr.empty() && dirStr.back() != fs::path::preferred_separator) {
        dirStr += fs::path::preferred_separator;
    }
    
    return fileStr.find(dirStr) == 0;
}

class GameFileManager::Impl {
public:
    Impl(std::shared_ptr<FileSystemConnector> fsConnector,
         std::shared_ptr<ServerConnector> serverConnector)
        : m_fsConnector(fsConnector)
        , m_serverConnector(serverConnector)
        , m_initialized(false)
        , m_serverSyncEnabled(false)
        , m_maxBackupsPerFile(10) {
    }
    
    ~Impl() = default;
    
    // Caminhos principais
    std::string m_gamePath;
    std::string m_backupPath;
    
    // Conectores
    std::shared_ptr<FileSystemConnector> m_fsConnector;
    std::shared_ptr<ServerConnector> m_serverConnector;
    
    // Status e configurações
    bool m_initialized;
    bool m_serverSyncEnabled;
    int m_maxBackupsPerFile;
    
    // Rastreamento de backups
    std::map<std::string, std::vector<BackupInfo>> m_backupRegistry;
    std::mutex m_backupMutex;
    
    // Função de log
    std::function<void(const std::string&)> m_logCallback;
    
    /**
     * @brief Registra uma mensagem no log
     * @param message Mensagem a ser registrada
     */
    void Log(const std::string& message) {
        if (m_logCallback) {
            m_logCallback(message);
        } else {
            std::cout << "[GameFileManager] " << message << std::endl;
        }
    }
    
    /**
     * @brief Normaliza um caminho para o formato canônico
     * @param path Caminho a ser normalizado
     * @return Caminho normalizado
     */
    std::string NormalizePath(const std::string& path) {
        try {
            // Verificar se o caminho é relativo
            fs::path fsPath(path);
            
            // Se for relativo e o caminho base estiver disponível, construir caminho absoluto
            if (fsPath.is_relative() && !m_gamePath.empty()) {
                fsPath = fs::path(m_gamePath) / fsPath;
            }
            
            // Normalizar o caminho
            fsPath = fs::absolute(fsPath);
            fsPath = fs::canonical(fsPath);
            
            return fsPath.string();
        }
        catch (const std::exception& e) {
            // Se o arquivo não existe, apenas normalize sem canonicalizar
            try {
                fs::path fsPath(path);
                
                if (fsPath.is_relative() && !m_gamePath.empty()) {
                    fsPath = fs::path(m_gamePath) / fsPath;
                }
                
                fsPath = fs::absolute(fsPath);
                return fsPath.string();
            }
            catch (...) {
                // Se ainda falhar, retornar o caminho original
                return path;
            }
        }
    }
    
    /**
     * @brief Obtém o caminho relativo ao diretório do jogo
     * @param path Caminho absoluto
     * @return Caminho relativo
     */
    std::string GetRelativePath(const std::string& path) {
        try {
            fs::path fullPath = fs::absolute(path);
            fs::path gameDirPath = fs::absolute(m_gamePath);
            
            // Tentar obter caminho relativo
            return fs::relative(fullPath, gameDirPath).string();
        }
        catch (...) {
            // Em caso de erro, retornar o caminho original
            return path;
        }
    }
    
    /**
     * @brief Obtém a extensão de um arquivo
     * @param path Caminho do arquivo
     * @return Extensão do arquivo
     */
    std::string GetFileExtension(const std::string& path) {
        fs::path fsPath(path);
        return fsPath.extension().string();
    }
    
    /**
     * @brief Carrega o registro de backups do disco
     */
    void LoadBackupRegistry() {
        std::lock_guard<std::mutex> lock(m_backupMutex);
        
        // Verificar se o diretório de backup existe
        if (!fs::exists(m_backupPath)) {
            Log("Diretório de backup não encontrado. Criando...");
            fs::create_directories(m_backupPath);
            m_backupRegistry.clear();
            return;
        }
        
        // Caminho para o arquivo de registro
        std::string registryPath = m_backupPath + "/backup-registry.json";
        
        // Verificar se o arquivo existe
        if (!fs::exists(registryPath)) {
            Log("Registro de backup não encontrado. Criando novo registro.");
            m_backupRegistry.clear();
            return;
        }
        
        try {
            // Abrir o arquivo
            std::ifstream file(registryPath);
            if (!file.is_open()) {
                Log("Erro ao abrir arquivo de registro de backup.");
                return;
            }
            
            // Ler o JSON
            json registryJson;
            file >> registryJson;
            file.close();
            
            // Limpar registro atual
            m_backupRegistry.clear();
            
            // Processar cada entrada
            for (const auto& entry : registryJson.items()) {
                std::string originalPath = entry.key();
                std::vector<BackupInfo> backups;
                
                for (const auto& backupJson : entry.value()) {
                    BackupInfo backup;
                    backup.backupPath = backupJson["backupPath"].get<std::string>();
                    backup.originalPath = backupJson["originalPath"].get<std::string>();
                    backup.timestamp = backupJson["timestamp"].get<int64_t>();
                    backup.operation = StringToOperation(backupJson["operation"].get<std::string>());
                    backup.description = backupJson["description"].get<std::string>();
                    
                    backups.push_back(backup);
                }
                
                m_backupRegistry[originalPath] = backups;
            }
            
            Log("Registro de backup carregado com sucesso: " + 
                std::to_string(m_backupRegistry.size()) + " arquivos com backups.");
        }
        catch (const std::exception& e) {
            Log("Erro ao carregar registro de backup: " + std::string(e.what()));
            // Em caso de erro, criar um novo registro vazio
            m_backupRegistry.clear();
        }
    }
    
    /**
     * @brief Salva o registro de backups no disco
     */
    void SaveBackupRegistry() {
        std::lock_guard<std::mutex> lock(m_backupMutex);
        
        // Verificar se o diretório de backup existe
        if (!fs::exists(m_backupPath)) {
            Log("Diretório de backup não encontrado. Criando...");
            fs::create_directories(m_backupPath);
        }
        
        // Caminho para o arquivo de registro
        std::string registryPath = m_backupPath + "/backup-registry.json";
        
        try {
            // Criar objeto JSON
            json registryJson;
            
            // Processar cada entrada
            for (const auto& entry : m_backupRegistry) {
                const std::string& originalPath = entry.first;
                const std::vector<BackupInfo>& backups = entry.second;
                
                json backupArray = json::array();
                
                for (const auto& backup : backups) {
                    json backupJson;
                    backupJson["backupPath"] = backup.backupPath;
                    backupJson["originalPath"] = backup.originalPath;
                    backupJson["timestamp"] = backup.timestamp;
                    backupJson["operation"] = OperationToString(backup.operation);
                    backupJson["description"] = backup.description;
                    
                    backupArray.push_back(backupJson);
                }
                
                registryJson[originalPath] = backupArray;
            }
            
            // Salvar no arquivo
            std::ofstream file(registryPath);
            if (!file.is_open()) {
                Log("Erro ao abrir arquivo de registro de backup para escrita.");
                return;
            }
            
            file << registryJson.dump(4); // Pretty print com 4 espaços
            file.close();
            
            Log("Registro de backup salvo com sucesso.");
        }
        catch (const std::exception& e) {
            Log("Erro ao salvar registro de backup: " + std::string(e.what()));
        }
    }
    
    /**
     * @brief Adiciona uma informação de backup ao registro
     * @param backupInfo Informações do backup
     */
    void AddBackupToRegistry(const BackupInfo& backupInfo) {
        std::lock_guard<std::mutex> lock(m_backupMutex);
        
        // Adicionar ao registro
        m_backupRegistry[backupInfo.originalPath].push_back(backupInfo);
        
        // Verificar limite de backups por arquivo
        if (m_backupRegistry[backupInfo.originalPath].size() > static_cast<size_t>(m_maxBackupsPerFile)) {
            // Ordenar por timestamp (mais antigo primeiro)
            std::sort(m_backupRegistry[backupInfo.originalPath].begin(),
                     m_backupRegistry[backupInfo.originalPath].end(),
                     [](const BackupInfo& a, const BackupInfo& b) {
                         return a.timestamp < b.timestamp;
                     });
            
            // Remover o backup mais antigo
            std::string oldestBackupPath = m_backupRegistry[backupInfo.originalPath][0].backupPath;
            
            // Excluir o arquivo de backup
            if (fs::exists(oldestBackupPath)) {
                try {
                    fs::remove(oldestBackupPath);
                    Log("Backup antigo removido: " + oldestBackupPath);
                }
                catch (const std::exception& e) {
                    Log("Erro ao remover backup antigo: " + std::string(e.what()));
                }
            }
            
            // Remover do registro
            m_backupRegistry[backupInfo.originalPath].erase(m_backupRegistry[backupInfo.originalPath].begin());
        }
        
        // Salvar o registro atualizado
        SaveBackupRegistry();
    }
    
    /**
     * @brief Gera um caminho para um novo arquivo de backup
     * @param originalPath Caminho original do arquivo
     * @param operation Operação sendo realizada
     * @return Caminho para o backup
     */
    std::string GenerateBackupPath(const std::string& originalPath, FileOperation operation) {
        // Obter timestamp atual
        int64_t timestamp = GetCurrentTimestamp();
        
        // Extrair nome e extensão do arquivo original
        fs::path original(originalPath);
        std::string fileName = original.filename().string();
        std::string extension = original.extension().string();
        std::string baseName = fileName.substr(0, fileName.length() - extension.length());
        
        // Criar nome formatado com timestamp
        std::string backupName = baseName + "_" + 
                                std::to_string(timestamp) + "_" + 
                                OperationToString(operation) + 
                                extension;
        
        // Criar caminho para subdiretório organizado por arquivo
        std::string relativePath = GetRelativePath(originalPath);
        std::string hashedDir = std::to_string(std::hash<std::string>{}(relativePath));
        
        // Criar caminho completo
        fs::path backupDir = fs::path(m_backupPath) / "files" / hashedDir;
        fs::path backupPath = backupDir / backupName;
        
        // Garantir que o diretório existe
        fs::create_directories(backupDir);
        
        return backupPath.string();
    }
    
    /**
     * @brief Cria um backup de um arquivo
     * @param path Caminho do arquivo
     * @param operation Operação que está sendo realizada
     * @param description Descrição do backup
     * @return Informações sobre o backup criado
     */
    BackupInfo CreateBackup(const std::string& path, 
                           FileOperation operation,
                           const std::string& description) {
        // Normalizar caminho
        std::string normalizedPath = NormalizePath(path);
        
        // Verificar se o arquivo existe
        if (!fs::exists(normalizedPath)) {
            throw std::runtime_error("Arquivo não encontrado para backup: " + normalizedPath);
        }
        
        // Gerar caminho para o backup
        std::string backupPath = GenerateBackupPath(normalizedPath, operation);
        
        // Criar diretório de backup se necessário
        fs::path backupDir = fs::path(backupPath).parent_path();
        if (!fs::exists(backupDir)) {
            fs::create_directories(backupDir);
        }
        
        // Copiar o arquivo
        fs::copy_file(normalizedPath, backupPath, fs::copy_options::overwrite_existing);
        
        // Criar informações do backup
        BackupInfo backupInfo;
        backupInfo.backupPath = backupPath;
        backupInfo.originalPath = normalizedPath;
        backupInfo.timestamp = GetCurrentTimestamp();
        backupInfo.operation = operation;
        backupInfo.description = description;
        
        // Adicionar ao registro
        AddBackupToRegistry(backupInfo);
        
        Log("Backup criado com sucesso: " + backupPath);
        return backupInfo;
    }
    
    /**
     * @brief Detecta o tipo/formato de um arquivo com base no conteúdo e extensão
     * @param path Caminho do arquivo
     * @return Informações sobre o formato
     */
    std::pair<std::string, std::string> DetectFileFormat(const std::string& path) {
        // Obter extensão
        std::string extension = GetFileExtension(path);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        // Tipo padrão baseado na extensão
        std::string type = "unknown";
        std::string format = "unknown";
        
        if (extension == ".json") {
            type = "json";
            format = "text";
        }
        else if (extension == ".bin") {
            type = "binary";
            format = "wyd_binary";
        }
        else if (extension == ".txt" || extension == ".csv" || extension == ".md") {
            type = "text";
            format = "plain_text";
        }
        else if (extension == ".dat") {
            type = "data";
            format = "wyd_data";
        }
        else if (extension == ".cpp" || extension == ".h" || extension == ".c" || extension == ".hpp") {
            type = "source";
            format = "cpp";
        }
        else if (extension == ".js" || extension == ".ts") {
            type = "source";
            format = "javascript";
        }
        else if (extension == ".py") {
            type = "source";
            format = "python";
        }
        else {
            // Tentar detectar pelo conteúdo
            try {
                std::ifstream file(path, std::ios::binary);
                if (file.is_open()) {
                    char header[4] = {0};
                    file.read(header, 4);
                    file.close();
                    
                    // Verificar assinaturas comuns
                    if (header[0] == '{' || header[0] == '[') {
                        type = "json";
                        format = "text";
                    }
                    else if (header[0] == 0x89 && header[1] == 'P' && header[2] == 'N' && header[3] == 'G') {
                        type = "image";
                        format = "png";
                    }
                    else if (header[0] == 0xFF && header[1] == 0xD8) {
                        type = "image";
                        format = "jpeg";
                    }
                    else if (header[0] == 'G' && header[1] == 'I' && header[2] == 'F') {
                        type = "image";
                        format = "gif";
                    }
                    else if (header[0] == '<') {
                        type = "markup";
                        format = "xml_or_html";
                    }
                }
            }
            catch (...) {
                // Ignorar erros na detecção
            }
        }
        
        return std::make_pair(type, format);
    }
    
    /**
     * @brief Verifica se um arquivo está relacionado ao servidor
     * @param path Caminho do arquivo
     * @return true se o arquivo está relacionado ao servidor
     */
    bool IsServerRelatedFile(const std::string& path) {
        // Extrair extensão e caminho normalizado
        std::string extension = GetFileExtension(path);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        std::string normalizedPath = NormalizePath(path);
        
        // Verificar padrões de arquivos relacionados ao servidor
        if (normalizedPath.find("server") != std::string::npos ||
            normalizedPath.find("Server") != std::string::npos) {
            return true;
        }
        
        if (extension == ".bin" || extension == ".dat" || extension == ".json") {
            // Verificar se está em diretórios relacionados ao servidor
            if (normalizedPath.find("TMSrv") != std::string::npos ||
                normalizedPath.find("DBSrv") != std::string::npos ||
                normalizedPath.find("Common") != std::string::npos) {
                return true;
            }
        }
        
        // Extensões específicas do servidor
        static const std::set<std::string> serverExtensions = {
            ".npc", ".mob", ".item", ".skill", ".quest", ".map"
        };
        
        if (serverExtensions.find(extension) != serverExtensions.end()) {
            return true;
        }
        
        return false;
    }
};

// Implementação da classe GameFileManager

GameFileManager::GameFileManager(std::shared_ptr<FileSystemConnector> fsConnector,
                               std::shared_ptr<ServerConnector> serverConnector)
    : m_pImpl(std::make_unique<Impl>(fsConnector, serverConnector)) {
}

GameFileManager::~GameFileManager() = default;

bool GameFileManager::Initialize(const std::string& gamePath, 
                               const std::string& backupPath,
                               int maxBackupsPerFile) {
    try {
        m_pImpl->Log("Inicializando gerenciador de arquivos...");
        
        // Normalizar caminhos
        std::string normalizedGamePath = fs::absolute(gamePath).string();
        std::string normalizedBackupPath;
        
        // Verificar se o caminho de backup é absoluto ou relativo
        fs::path backupPathObj(backupPath);
        if (backupPathObj.is_absolute()) {
            normalizedBackupPath = fs::absolute(backupPath).string();
        }
        else {
            // Se for relativo, usar o diretório de jogo como base
            normalizedBackupPath = (fs::path(normalizedGamePath) / backupPath).string();
        }
        
        // Verificar se o diretório do jogo existe
        if (!fs::exists(normalizedGamePath)) {
            m_pImpl->Log("Erro: Diretório do jogo não encontrado: " + normalizedGamePath);
            return false;
        }
        
        // Criar diretório de backup se não existir
        if (!fs::exists(normalizedBackupPath)) {
            m_pImpl->Log("Criando diretório de backup: " + normalizedBackupPath);
            fs::create_directories(normalizedBackupPath);
        }
        
        // Guardar caminhos e configurações
        m_pImpl->m_gamePath = normalizedGamePath;
        m_pImpl->m_backupPath = normalizedBackupPath;
        m_pImpl->m_maxBackupsPerFile = maxBackupsPerFile;
        
        // Carregar registro de backups
        m_pImpl->LoadBackupRegistry();
        
        m_pImpl->m_initialized = true;
        m_pImpl->Log("Gerenciador de arquivos inicializado com sucesso.");
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao inicializar gerenciador de arquivos: " + std::string(e.what()));
        return false;
    }
}

void GameFileManager::SetServerSyncEnabled(bool enabled) {
    m_pImpl->m_serverSyncEnabled = enabled;
    m_pImpl->Log("Sincronização com servidor " + std::string(enabled ? "ativada" : "desativada"));
}

bool GameFileManager::IsServerSyncEnabled() const {
    return m_pImpl->m_serverSyncEnabled;
}

FileOperationResult GameFileManager::CreateFile(const std::string& path, 
                                             const std::string& content,
                                             const std::string& description) {
    FileOperationResult result;
    result.success = false;
    result.timestamp = GetCurrentTimestamp();
    result.originalPath = path;
    result.serverSyncRequired = false;
    result.serverStatus = ServerFileStatus::NOT_APPLICABLE;
    
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminho
        std::string normalizedPath = m_pImpl->NormalizePath(path);
        result.originalPath = normalizedPath;
        
        // Verificar se o arquivo já existe
        if (fs::exists(normalizedPath)) {
            throw std::runtime_error("Arquivo já existe: " + normalizedPath);
        }
        
        // Criar diretório pai se necessário
        fs::path parent = fs::path(normalizedPath).parent_path();
        if (!fs::exists(parent)) {
            fs::create_directories(parent);
        }
        
        // Escrever conteúdo no arquivo
        std::ofstream file(normalizedPath);
        if (!file.is_open()) {
            throw std::runtime_error("Não foi possível criar o arquivo: " + normalizedPath);
        }
        
        file << content;
        file.close();
        
        // Verificar se o arquivo foi criado
        if (!fs::exists(normalizedPath)) {
            throw std::runtime_error("Falha ao verificar criação do arquivo: " + normalizedPath);
        }
        
        // Não há backup para criação, pois não havia arquivo anterior
        // Mas registrar a operação para fins de histórico
        BackupInfo backupInfo;
        backupInfo.originalPath = normalizedPath;
        backupInfo.timestamp = result.timestamp;
        backupInfo.operation = FileOperation::CREATE;
        backupInfo.description = description.empty() ? "Criação de arquivo" : description;
        m_pImpl->AddBackupToRegistry(backupInfo);
        
        // Verificar se é necessário sincronizar com o servidor
        if (m_pImpl->m_serverSyncEnabled && m_pImpl->IsServerRelatedFile(normalizedPath)) {
            result.serverSyncRequired = true;
            result.serverStatus = ServerFileStatus::MODIFIED_LOCALLY;
        }
        
        // Preencher resultados
        result.success = true;
        result.message = "Arquivo criado com sucesso: " + normalizedPath;
        
        m_pImpl->Log(result.message);
        return result;
    }
    catch (const std::exception& e) {
        result.message = "Erro ao criar arquivo: " + std::string(e.what());
        m_pImpl->Log(result.message);
        return result;
    }
}

FileOperationResult GameFileManager::CreateBinaryFile(const std::string& path, 
                                                   const void* data, 
                                                   size_t size,
                                                   const std::string& description) {
    FileOperationResult result;
    result.success = false;
    result.timestamp = GetCurrentTimestamp();
    result.originalPath = path;
    result.serverSyncRequired = false;
    result.serverStatus = ServerFileStatus::NOT_APPLICABLE;
    
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminho
        std::string normalizedPath = m_pImpl->NormalizePath(path);
        result.originalPath = normalizedPath;
        
        // Verificar se o arquivo já existe
        if (fs::exists(normalizedPath)) {
            throw std::runtime_error("Arquivo já existe: " + normalizedPath);
        }
        
        // Criar diretório pai se necessário
        fs::path parent = fs::path(normalizedPath).parent_path();
        if (!fs::exists(parent)) {
            fs::create_directories(parent);
        }
        
        // Escrever dados binários no arquivo
        std::ofstream file(normalizedPath, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Não foi possível criar o arquivo: " + normalizedPath);
        }
        
        file.write(static_cast<const char*>(data), size);
        file.close();
        
        // Verificar se o arquivo foi criado
        if (!fs::exists(normalizedPath)) {
            throw std::runtime_error("Falha ao verificar criação do arquivo: " + normalizedPath);
        }
        
        // Não há backup para criação, pois não havia arquivo anterior
        // Mas registrar a operação para fins de histórico
        BackupInfo backupInfo;
        backupInfo.originalPath = normalizedPath;
        backupInfo.timestamp = result.timestamp;
        backupInfo.operation = FileOperation::CREATE;
        backupInfo.description = description.empty() ? "Criação de arquivo binário" : description;
        m_pImpl->AddBackupToRegistry(backupInfo);
        
        // Verificar se é necessário sincronizar com o servidor
        if (m_pImpl->m_serverSyncEnabled && m_pImpl->IsServerRelatedFile(normalizedPath)) {
            result.serverSyncRequired = true;
            result.serverStatus = ServerFileStatus::MODIFIED_LOCALLY;
        }
        
        // Preencher resultados
        result.success = true;
        result.message = "Arquivo binário criado com sucesso: " + normalizedPath;
        
        m_pImpl->Log(result.message);
        return result;
    }
    catch (const std::exception& e) {
        result.message = "Erro ao criar arquivo binário: " + std::string(e.what());
        m_pImpl->Log(result.message);
        return result;
    }
}

FileOperationResult GameFileManager::ModifyFile(const std::string& path, 
                                             const std::string& newContent,
                                             const std::string& description) {
    FileOperationResult result;
    result.success = false;
    result.timestamp = GetCurrentTimestamp();
    result.originalPath = path;
    result.serverSyncRequired = false;
    result.serverStatus = ServerFileStatus::NOT_APPLICABLE;
    
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminho
        std::string normalizedPath = m_pImpl->NormalizePath(path);
        result.originalPath = normalizedPath;
        
        // Verificar se o arquivo existe
        if (!fs::exists(normalizedPath)) {
            throw std::runtime_error("Arquivo não encontrado: " + normalizedPath);
        }
        
        // Criar backup antes de modificar
        BackupInfo backup = CreateBackup(normalizedPath, 
                                      FileOperation::MODIFY, 
                                      description.empty() ? "Modificação de arquivo" : description);
        
        result.backupPath = backup.backupPath;
        
        // Escrever novo conteúdo no arquivo
        std::ofstream file(normalizedPath);
        if (!file.is_open()) {
            throw std::runtime_error("Não foi possível abrir o arquivo para escrita: " + normalizedPath);
        }
        
        file << newContent;
        file.close();
        
        // Verificar se é necessário sincronizar com o servidor
        if (m_pImpl->m_serverSyncEnabled && m_pImpl->IsServerRelatedFile(normalizedPath)) {
            result.serverSyncRequired = true;
            result.serverStatus = ServerFileStatus::MODIFIED_LOCALLY;
        }
        
        // Preencher resultados
        result.success = true;
        result.message = "Arquivo modificado com sucesso: " + normalizedPath;
        
        m_pImpl->Log(result.message);
        return result;
    }
    catch (const std::exception& e) {
        result.message = "Erro ao modificar arquivo: " + std::string(e.what());
        m_pImpl->Log(result.message);
        return result;
    }
}

FileOperationResult GameFileManager::ModifyFilePart(const std::string& path, 
                                                 const std::string& oldContent,
                                                 const std::string& newContent,
                                                 const std::string& description) {
    FileOperationResult result;
    result.success = false;
    result.timestamp = GetCurrentTimestamp();
    result.originalPath = path;
    result.serverSyncRequired = false;
    result.serverStatus = ServerFileStatus::NOT_APPLICABLE;
    
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminho
        std::string normalizedPath = m_pImpl->NormalizePath(path);
        result.originalPath = normalizedPath;
        
        // Verificar se o arquivo existe
        if (!fs::exists(normalizedPath)) {
            throw std::runtime_error("Arquivo não encontrado: " + normalizedPath);
        }
        
        // Ler conteúdo atual
        std::string currentContent = ReadFileAsText(normalizedPath);
        
        // Verificar se o conteúdo a ser substituído existe
        if (currentContent.find(oldContent) == std::string::npos) {
            throw std::runtime_error("Conteúdo a ser substituído não encontrado no arquivo");
        }
        
        // Criar backup antes de modificar
        BackupInfo backup = CreateBackup(normalizedPath, 
                                      FileOperation::MODIFY, 
                                      description.empty() ? "Modificação parcial de arquivo" : description);
        
        result.backupPath = backup.backupPath;
        
        // Substituir conteúdo
        std::string updatedContent = std::regex_replace(
            currentContent, std::regex(oldContent, std::regex_constants::literal), newContent);
        
        // Escrever novo conteúdo no arquivo
        std::ofstream file(normalizedPath);
        if (!file.is_open()) {
            throw std::runtime_error("Não foi possível abrir o arquivo para escrita: " + normalizedPath);
        }
        
        file << updatedContent;
        file.close();
        
        // Verificar se é necessário sincronizar com o servidor
        if (m_pImpl->m_serverSyncEnabled && m_pImpl->IsServerRelatedFile(normalizedPath)) {
            result.serverSyncRequired = true;
            result.serverStatus = ServerFileStatus::MODIFIED_LOCALLY;
        }
        
        // Preencher resultados
        result.success = true;
        result.message = "Parte do arquivo modificada com sucesso: " + normalizedPath;
        
        m_pImpl->Log(result.message);
        return result;
    }
    catch (const std::exception& e) {
        result.message = "Erro ao modificar parte do arquivo: " + std::string(e.what());
        m_pImpl->Log(result.message);
        return result;
    }
}

FileOperationResult GameFileManager::DeleteFile(const std::string& path,
                                             const std::string& description) {
    FileOperationResult result;
    result.success = false;
    result.timestamp = GetCurrentTimestamp();
    result.originalPath = path;
    result.serverSyncRequired = false;
    result.serverStatus = ServerFileStatus::NOT_APPLICABLE;
    
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminho
        std::string normalizedPath = m_pImpl->NormalizePath(path);
        result.originalPath = normalizedPath;
        
        // Verificar se o arquivo existe
        if (!fs::exists(normalizedPath)) {
            throw std::runtime_error("Arquivo não encontrado: " + normalizedPath);
        }
        
        // Criar backup antes de excluir
        BackupInfo backup = CreateBackup(normalizedPath, 
                                      FileOperation::DELETE, 
                                      description.empty() ? "Exclusão de arquivo" : description);
        
        result.backupPath = backup.backupPath;
        
        // Excluir o arquivo
        fs::remove(normalizedPath);
        
        // Verificar se o arquivo foi excluído
        if (fs::exists(normalizedPath)) {
            throw std::runtime_error("Falha ao excluir arquivo: " + normalizedPath);
        }
        
        // Verificar se é necessário sincronizar com o servidor
        if (m_pImpl->m_serverSyncEnabled && m_pImpl->IsServerRelatedFile(normalizedPath)) {
            result.serverSyncRequired = true;
            result.serverStatus = ServerFileStatus::MODIFIED_LOCALLY;
        }
        
        // Preencher resultados
        result.success = true;
        result.message = "Arquivo excluído com sucesso: " + normalizedPath;
        
        m_pImpl->Log(result.message);
        return result;
    }
    catch (const std::exception& e) {
        result.message = "Erro ao excluir arquivo: " + std::string(e.what());
        m_pImpl->Log(result.message);
        return result;
    }
}

FileOperationResult GameFileManager::MoveFile(const std::string& sourcePath,
                                           const std::string& destinationPath,
                                           const std::string& description) {
    FileOperationResult result;
    result.success = false;
    result.timestamp = GetCurrentTimestamp();
    result.originalPath = sourcePath;
    result.newPath = destinationPath;
    result.serverSyncRequired = false;
    result.serverStatus = ServerFileStatus::NOT_APPLICABLE;
    
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminhos
        std::string normalizedSourcePath = m_pImpl->NormalizePath(sourcePath);
        std::string normalizedDestPath = m_pImpl->NormalizePath(destinationPath);
        
        result.originalPath = normalizedSourcePath;
        result.newPath = normalizedDestPath;
        
        // Verificar se o arquivo de origem existe
        if (!fs::exists(normalizedSourcePath)) {
            throw std::runtime_error("Arquivo de origem não encontrado: " + normalizedSourcePath);
        }
        
        // Verificar se o arquivo de destino já existe
        if (fs::exists(normalizedDestPath)) {
            throw std::runtime_error("Arquivo de destino já existe: " + normalizedDestPath);
        }
        
        // Criar backup antes de mover
        BackupInfo backup = CreateBackup(normalizedSourcePath, 
                                      FileOperation::MOVE, 
                                      description.empty() ? "Movendo arquivo" : description);
        
        result.backupPath = backup.backupPath;
        
        // Criar diretório pai do destino se necessário
        fs::path parentDir = fs::path(normalizedDestPath).parent_path();
        if (!fs::exists(parentDir)) {
            fs::create_directories(parentDir);
        }
        
        // Mover o arquivo
        fs::rename(normalizedSourcePath, normalizedDestPath);
        
        // Verificar se a operação foi bem-sucedida
        if (fs::exists(normalizedSourcePath) || !fs::exists(normalizedDestPath)) {
            throw std::runtime_error("Falha ao mover arquivo");
        }
        
        // Verificar se é necessário sincronizar com o servidor
        bool sourceIsServerFile = m_pImpl->IsServerRelatedFile(normalizedSourcePath);
        bool destIsServerFile = m_pImpl->IsServerRelatedFile(normalizedDestPath);
        
        if (m_pImpl->m_serverSyncEnabled && (sourceIsServerFile || destIsServerFile)) {
            result.serverSyncRequired = true;
            result.serverStatus = ServerFileStatus::MODIFIED_LOCALLY;
        }
        
        // Preencher resultados
        result.success = true;
        result.message = "Arquivo movido com sucesso de: " + normalizedSourcePath + " para: " + normalizedDestPath;
        
        m_pImpl->Log(result.message);
        return result;
    }
    catch (const std::exception& e) {
        result.message = "Erro ao mover arquivo: " + std::string(e.what());
        m_pImpl->Log(result.message);
        return result;
    }
}

FileOperationResult GameFileManager::CopyFile(const std::string& sourcePath,
                                           const std::string& destinationPath,
                                           const std::string& description) {
    FileOperationResult result;
    result.success = false;
    result.timestamp = GetCurrentTimestamp();
    result.originalPath = sourcePath;
    result.newPath = destinationPath;
    result.serverSyncRequired = false;
    result.serverStatus = ServerFileStatus::NOT_APPLICABLE;
    
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminhos
        std::string normalizedSourcePath = m_pImpl->NormalizePath(sourcePath);
        std::string normalizedDestPath = m_pImpl->NormalizePath(destinationPath);
        
        result.originalPath = normalizedSourcePath;
        result.newPath = normalizedDestPath;
        
        // Verificar se o arquivo de origem existe
        if (!fs::exists(normalizedSourcePath)) {
            throw std::runtime_error("Arquivo de origem não encontrado: " + normalizedSourcePath);
        }
        
        // Verificar se o arquivo de destino já existe
        if (fs::exists(normalizedDestPath)) {
            throw std::runtime_error("Arquivo de destino já existe: " + normalizedDestPath);
        }
        
        // Criar backup antes de copiar (apenas registrar, não é necessário fazer backup do arquivo)
        BackupInfo backupInfo;
        backupInfo.originalPath = normalizedSourcePath;
        backupInfo.timestamp = result.timestamp;
        backupInfo.operation = FileOperation::COPY;
        backupInfo.description = description.empty() ? "Copiando arquivo" : description;
        m_pImpl->AddBackupToRegistry(backupInfo);
        
        // Criar diretório pai do destino se necessário
        fs::path parentDir = fs::path(normalizedDestPath).parent_path();
        if (!fs::exists(parentDir)) {
            fs::create_directories(parentDir);
        }
        
        // Copiar o arquivo
        fs::copy_file(normalizedSourcePath, normalizedDestPath);
        
        // Verificar se a operação foi bem-sucedida
        if (!fs::exists(normalizedDestPath)) {
            throw std::runtime_error("Falha ao copiar arquivo");
        }
        
        // Verificar se é necessário sincronizar com o servidor
        bool destIsServerFile = m_pImpl->IsServerRelatedFile(normalizedDestPath);
        
        if (m_pImpl->m_serverSyncEnabled && destIsServerFile) {
            result.serverSyncRequired = true;
            result.serverStatus = ServerFileStatus::MODIFIED_LOCALLY;
        }
        
        // Preencher resultados
        result.success = true;
        result.message = "Arquivo copiado com sucesso de: " + normalizedSourcePath + " para: " + normalizedDestPath;
        
        m_pImpl->Log(result.message);
        return result;
    }
    catch (const std::exception& e) {
        result.message = "Erro ao copiar arquivo: " + std::string(e.what());
        m_pImpl->Log(result.message);
        return result;
    }
}

FileOperationResult GameFileManager::CompileJsonToBinary(const std::string& jsonPath,
                                                      const std::string& binPath,
                                                      const std::string& description) {
    FileOperationResult result;
    result.success = false;
    result.timestamp = GetCurrentTimestamp();
    result.originalPath = jsonPath;
    result.serverSyncRequired = false;
    result.serverStatus = ServerFileStatus::NOT_APPLICABLE;
    
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminho do JSON
        std::string normalizedJsonPath = m_pImpl->NormalizePath(jsonPath);
        result.originalPath = normalizedJsonPath;
        
        // Verificar se o arquivo JSON existe
        if (!fs::exists(normalizedJsonPath)) {
            throw std::runtime_error("Arquivo JSON não encontrado: " + normalizedJsonPath);
        }
        
        // Determinar caminho de saída do binário se não fornecido
        std::string outputBinPath;
        if (binPath.empty()) {
            // Usar o mesmo caminho, mas com extensão .bin
            fs::path jsonPathObj(normalizedJsonPath);
            outputBinPath = (jsonPathObj.parent_path() / (jsonPathObj.stem().string() + ".bin")).string();
        }
        else {
            outputBinPath = m_pImpl->NormalizePath(binPath);
        }
        
        result.newPath = outputBinPath;
        
        // Verificar se temos o FileSystemConnector
        if (!m_pImpl->m_fsConnector) {
            throw std::runtime_error("FileSystemConnector não disponível");
        }
        
        // Criar backup do arquivo binário se já existir
        if (fs::exists(outputBinPath)) {
            BackupInfo backup = CreateBackup(outputBinPath, 
                                          FileOperation::COMPILE, 
                                          description.empty() ? "Compilando JSON para binário" : description);
            
            result.backupPath = backup.backupPath;
        }
        
        // Chamar o método de compilação no FileSystemConnector
        bool compilationSuccess = m_pImpl->m_fsConnector->ConvertJsonToBinary(normalizedJsonPath, outputBinPath);
        
        if (!compilationSuccess) {
            throw std::runtime_error("Falha na compilação do JSON para binário");
        }
        
        // Verificar se o arquivo binário foi criado
        if (!fs::exists(outputBinPath)) {
            throw std::runtime_error("Arquivo binário não foi criado após compilação");
        }
        
        // Verificar se é necessário sincronizar com o servidor
        if (m_pImpl->m_serverSyncEnabled && m_pImpl->IsServerRelatedFile(outputBinPath)) {
            result.serverSyncRequired = true;
            result.serverStatus = ServerFileStatus::MODIFIED_LOCALLY;
        }
        
        // Preencher resultados
        result.success = true;
        result.message = "JSON compilado com sucesso para binário: " + outputBinPath;
        
        m_pImpl->Log(result.message);
        return result;
    }
    catch (const std::exception& e) {
        result.message = "Erro ao compilar JSON para binário: " + std::string(e.what());
        m_pImpl->Log(result.message);
        return result;
    }
}

FileOperationResult GameFileManager::DecompileBinaryToJson(const std::string& binPath,
                                                        const std::string& jsonPath,
                                                        const std::string& description) {
    FileOperationResult result;
    result.success = false;
    result.timestamp = GetCurrentTimestamp();
    result.originalPath = binPath;
    result.serverSyncRequired = false;
    result.serverStatus = ServerFileStatus::NOT_APPLICABLE;
    
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminho do binário
        std::string normalizedBinPath = m_pImpl->NormalizePath(binPath);
        result.originalPath = normalizedBinPath;
        
        // Verificar se o arquivo binário existe
        if (!fs::exists(normalizedBinPath)) {
            throw std::runtime_error("Arquivo binário não encontrado: " + normalizedBinPath);
        }
        
        // Determinar caminho de saída do JSON se não fornecido
        std::string outputJsonPath;
        if (jsonPath.empty()) {
            // Usar o mesmo caminho, mas com extensão .json
            fs::path binPathObj(normalizedBinPath);
            outputJsonPath = (binPathObj.parent_path() / (binPathObj.stem().string() + ".json")).string();
        }
        else {
            outputJsonPath = m_pImpl->NormalizePath(jsonPath);
        }
        
        result.newPath = outputJsonPath;
        
        // Verificar se temos o FileSystemConnector
        if (!m_pImpl->m_fsConnector) {
            throw std::runtime_error("FileSystemConnector não disponível");
        }
        
        // Criar backup do arquivo JSON se já existir
        if (fs::exists(outputJsonPath)) {
            BackupInfo backup = CreateBackup(outputJsonPath, 
                                          FileOperation::DECOMPILE, 
                                          description.empty() ? "Descompilando binário para JSON" : description);
            
            result.backupPath = backup.backupPath;
        }
        
        // Chamar o método de descompilação no FileSystemConnector
        bool decompilationSuccess = m_pImpl->m_fsConnector->ConvertBinaryToJson(normalizedBinPath, outputJsonPath);
        
        if (!decompilationSuccess) {
            throw std::runtime_error("Falha na descompilação do binário para JSON");
        }
        
        // Verificar se o arquivo JSON foi criado
        if (!fs::exists(outputJsonPath)) {
            throw std::runtime_error("Arquivo JSON não foi criado após descompilação");
        }
        
        // Verificar se é necessário sincronizar com o servidor
        // (Geralmente não é necessário, pois o JSON é apenas para edição local)
        result.serverSyncRequired = false;
        result.serverStatus = ServerFileStatus::NOT_APPLICABLE;
        
        // Preencher resultados
        result.success = true;
        result.message = "Binário descompilado com sucesso para JSON: " + outputJsonPath;
        
        m_pImpl->Log(result.message);
        return result;
    }
    catch (const std::exception& e) {
        result.message = "Erro ao descompilar binário para JSON: " + std::string(e.what());
        m_pImpl->Log(result.message);
        return result;
    }
}

std::string GameFileManager::ReadFileAsText(const std::string& path) {
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminho
        std::string normalizedPath = m_pImpl->NormalizePath(path);
        
        // Verificar se o arquivo existe
        if (!fs::exists(normalizedPath)) {
            throw std::runtime_error("Arquivo não encontrado: " + normalizedPath);
        }
        
        // Abrir e ler o arquivo
        std::ifstream file(normalizedPath);
        if (!file.is_open()) {
            throw std::runtime_error("Não foi possível abrir o arquivo: " + normalizedPath);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao ler arquivo como texto: " + std::string(e.what()));
        return "";
    }
}

std::vector<uint8_t> GameFileManager::ReadFileAsBinary(const std::string& path) {
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminho
        std::string normalizedPath = m_pImpl->NormalizePath(path);
        
        // Verificar se o arquivo existe
        if (!fs::exists(normalizedPath)) {
            throw std::runtime_error("Arquivo não encontrado: " + normalizedPath);
        }
        
        // Abrir e ler o arquivo em modo binário
        std::ifstream file(normalizedPath, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Não foi possível abrir o arquivo: " + normalizedPath);
        }
        
        // Determinar o tamanho do arquivo
        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        // Ler os dados para um vetor
        std::vector<uint8_t> buffer(size);
        if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            return buffer;
        }
        else {
            throw std::runtime_error("Falha ao ler dados do arquivo: " + normalizedPath);
        }
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao ler arquivo como binário: " + std::string(e.what()));
        return std::vector<uint8_t>();
    }
}

BackupInfo GameFileManager::CreateManualBackup(const std::string& path,
                                             const std::string& description) {
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminho
        std::string normalizedPath = m_pImpl->NormalizePath(path);
        
        // Verificar se o arquivo existe
        if (!fs::exists(normalizedPath)) {
            throw std::runtime_error("Arquivo não encontrado: " + normalizedPath);
        }
        
        // Criar backup
        BackupInfo backup = CreateBackup(normalizedPath, 
                                      FileOperation::MODIFY, 
                                      description.empty() ? "Backup manual" : description);
        
        m_pImpl->Log("Backup manual criado com sucesso: " + backup.backupPath);
        return backup;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao criar backup manual: " + std::string(e.what()));
        
        // Retornar um backup vazio em caso de erro
        BackupInfo emptyBackup;
        emptyBackup.timestamp = GetCurrentTimestamp();
        return emptyBackup;
    }
}

FileOperationResult GameFileManager::RestoreFromBackup(const std::string& backupPath,
                                                    bool restoreToOriginalPath,
                                                    const std::string& alternativePath) {
    FileOperationResult result;
    result.success = false;
    result.timestamp = GetCurrentTimestamp();
    result.originalPath = backupPath;
    result.serverSyncRequired = false;
    result.serverStatus = ServerFileStatus::NOT_APPLICABLE;
    
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminho do backup
        std::string normalizedBackupPath = m_pImpl->NormalizePath(backupPath);
        result.originalPath = normalizedBackupPath;
        
        // Verificar se o arquivo de backup existe
        if (!fs::exists(normalizedBackupPath)) {
            throw std::runtime_error("Arquivo de backup não encontrado: " + normalizedBackupPath);
        }
        
        // Encontrar as informações de backup no registro
        std::string originalFilePath;
        std::lock_guard<std::mutex> lock(m_pImpl->m_backupMutex);
        
        bool backupFound = false;
        for (const auto& entry : m_pImpl->m_backupRegistry) {
            for (const auto& backup : entry.second) {
                if (backup.backupPath == normalizedBackupPath) {
                    originalFilePath = backup.originalPath;
                    backupFound = true;
                    break;
                }
            }
            if (backupFound) break;
        }
        
        if (!backupFound && restoreToOriginalPath) {
            throw std::runtime_error("Informações de backup não encontradas no registro");
        }
        
        // Determinar o caminho de destino para restauração
        std::string destinationPath;
        if (restoreToOriginalPath) {
            destinationPath = originalFilePath;
        } else {
            if (alternativePath.empty()) {
                throw std::runtime_error("Caminho alternativo não fornecido para restauração");
            }
            destinationPath = m_pImpl->NormalizePath(alternativePath);
        }
        
        result.newPath = destinationPath;
        
        // Criar backup do arquivo atual antes de restaurar (se existir)
        if (fs::exists(destinationPath)) {
            BackupInfo currentBackup = CreateBackup(destinationPath, 
                                              FileOperation::MODIFY, 
                                              "Backup antes de restauração");
            
            result.backupPath = currentBackup.backupPath;
        }
        
        // Criar diretório pai do destino se necessário
        fs::path parentDir = fs::path(destinationPath).parent_path();
        if (!fs::exists(parentDir)) {
            fs::create_directories(parentDir);
        }
        
        // Copiar o arquivo de backup para o destino
        fs::copy_file(normalizedBackupPath, destinationPath, fs::copy_options::overwrite_existing);
        
        // Verificar se a restauração foi bem-sucedida
        if (!fs::exists(destinationPath)) {
            throw std::runtime_error("Falha ao restaurar arquivo");
        }
        
        // Verificar se é necessário sincronizar com o servidor
        if (m_pImpl->m_serverSyncEnabled && m_pImpl->IsServerRelatedFile(destinationPath)) {
            result.serverSyncRequired = true;
            result.serverStatus = ServerFileStatus::MODIFIED_LOCALLY;
        }
        
        // Preencher resultados
        result.success = true;
        result.message = "Arquivo restaurado com sucesso de: " + normalizedBackupPath + " para: " + destinationPath;
        
        m_pImpl->Log(result.message);
        return result;
    }
    catch (const std::exception& e) {
        result.message = "Erro ao restaurar arquivo: " + std::string(e.what());
        m_pImpl->Log(result.message);
        return result;
    }
}

std::vector<BackupInfo> GameFileManager::GetBackupsForFile(const std::string& path) {
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminho
        std::string normalizedPath = m_pImpl->NormalizePath(path);
        
        // Buscar backups no registro
        std::lock_guard<std::mutex> lock(m_pImpl->m_backupMutex);
        
        auto it = m_pImpl->m_backupRegistry.find(normalizedPath);
        if (it != m_pImpl->m_backupRegistry.end()) {
            // Ordenar backups do mais recente para o mais antigo
            std::vector<BackupInfo> backups = it->second;
            std::sort(backups.begin(), backups.end(), 
                     [](const BackupInfo& a, const BackupInfo& b) {
                         return a.timestamp > b.timestamp;
                     });
            
            return backups;
        }
        
        // Nenhum backup encontrado
        return std::vector<BackupInfo>();
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao obter backups para arquivo: " + std::string(e.what()));
        return std::vector<BackupInfo>();
    }
}

std::map<std::string, std::vector<BackupInfo>> GameFileManager::GetAllBackups() {
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Retornar uma cópia do registro
        std::lock_guard<std::mutex> lock(m_pImpl->m_backupMutex);
        return m_pImpl->m_backupRegistry;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao obter todos os backups: " + std::string(e.what()));
        return std::map<std::string, std::vector<BackupInfo>>();
    }
}

int GameFileManager::CleanupOldBackups(int keepCount, int64_t olderThan) {
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        std::lock_guard<std::mutex> lock(m_pImpl->m_backupMutex);
        
        int removedCount = 0;
        
        // Para cada arquivo no registro
        for (auto& entry : m_pImpl->m_backupRegistry) {
            std::vector<BackupInfo>& backups = entry.second;
            
            // Ordenar backups do mais recente para o mais antigo
            std::sort(backups.begin(), backups.end(), 
                     [](const BackupInfo& a, const BackupInfo& b) {
                         return a.timestamp > b.timestamp;
                     });
            
            // Criar uma nova lista para manter os backups que serão preservados
            std::vector<BackupInfo> keptBackups;
            
            // Se keepCount é especificado, manter apenas esse número de backups mais recentes
            if (keepCount > 0) {
                int count = 0;
                for (const auto& backup : backups) {
                    if (count < keepCount) {
                        keptBackups.push_back(backup);
                        count++;
                    } else {
                        // Remover o arquivo de backup
                        if (fs::exists(backup.backupPath)) {
                            try {
                                fs::remove(backup.backupPath);
                                removedCount++;
                            }
                            catch (...) {
                                // Ignorar falhas ao remover
                            }
                        }
                    }
                }
            }
            // Se olderThan é especificado, remover backups mais antigos que essa data
            else if (olderThan > 0) {
                for (const auto& backup : backups) {
                    if (backup.timestamp >= olderThan) {
                        keptBackups.push_back(backup);
                    } else {
                        // Remover o arquivo de backup
                        if (fs::exists(backup.backupPath)) {
                            try {
                                fs::remove(backup.backupPath);
                                removedCount++;
                            }
                            catch (...) {
                                // Ignorar falhas ao remover
                            }
                        }
                    }
                }
            }
            
            // Atualizar a lista de backups para este arquivo
            backups = keptBackups;
        }
        
        // Remover entradas vazias do registro
        auto it = m_pImpl->m_backupRegistry.begin();
        while (it != m_pImpl->m_backupRegistry.end()) {
            if (it->second.empty()) {
                it = m_pImpl->m_backupRegistry.erase(it);
            } else {
                ++it;
            }
        }
        
        // Salvar o registro atualizado
        m_pImpl->SaveBackupRegistry();
        
        m_pImpl->Log("Limpeza de backups concluída: " + std::to_string(removedCount) + " backups removidos");
        return removedCount;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao limpar backups antigos: " + std::string(e.what()));
        return 0;
    }
}

std::map<std::string, FileOperationResult> GameFileManager::SyncWithServer(
    const std::vector<std::string>& paths,
    int direction) {
    std::map<std::string, FileOperationResult> results;
    
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        if (!m_pImpl->m_serverSyncEnabled) {
            throw std::runtime_error("Sincronização com servidor não está ativada");
        }
        
        if (!m_pImpl->m_serverConnector) {
            throw std::runtime_error("Conector de servidor não disponível");
        }
        
        // Verificar se o servidor está conectado
        if (!m_pImpl->m_serverConnector->IsConnected()) {
            throw std::runtime_error("Não conectado ao servidor");
        }
        
        std::vector<std::string> filesToSync;
        
        // Se nenhum caminho específico for fornecido, encontrar todos os arquivos relacionados ao servidor
        if (paths.empty()) {
            // Encontrar arquivos relacionados ao servidor no diretório do jogo
            for (const auto& entry : fs::recursive_directory_iterator(m_pImpl->m_gamePath)) {
                if (entry.is_regular_file() && m_pImpl->IsServerRelatedFile(entry.path().string())) {
                    filesToSync.push_back(entry.path().string());
                }
            }
        } else {
            // Usar os caminhos fornecidos
            for (const auto& path : paths) {
                std::string normalizedPath = m_pImpl->NormalizePath(path);
                
                // Verificar se é um arquivo ou diretório
                if (fs::is_directory(normalizedPath)) {
                    // Se for diretório, adicionar todos os arquivos relacionados ao servidor dentro dele
                    for (const auto& entry : fs::recursive_directory_iterator(normalizedPath)) {
                        if (entry.is_regular_file() && m_pImpl->IsServerRelatedFile(entry.path().string())) {
                            filesToSync.push_back(entry.path().string());
                        }
                    }
                } else if (fs::is_regular_file(normalizedPath)) {
                    // Se for arquivo e relacionado ao servidor, adicionar
                    if (m_pImpl->IsServerRelatedFile(normalizedPath)) {
                        filesToSync.push_back(normalizedPath);
                    }
                }
            }
        }
        
        // Sincronizar cada arquivo
        for (const auto& filePath : filesToSync) {
            FileOperationResult result;
            result.success = false;
            result.timestamp = GetCurrentTimestamp();
            result.originalPath = filePath;
            result.serverSyncRequired = false;
            
            try {
                // Em uma implementação real, aqui teríamos código para sincronizar com o servidor
                // usando o ServerConnector. Como é um exemplo, apenas simulamos a sincronização.
                
                std::string fileName = fs::path(filePath).filename().string();
                
                if (direction == 0 || direction == 1) {
                    // Local -> Servidor
                    m_pImpl->Log("Enviando arquivo para o servidor: " + filePath);
                    
                    // Simular envio para o servidor
                    std::vector<uint8_t> fileData = ReadFileAsBinary(filePath);
                    
                    // Em uma implementação real, enviaríamos os dados para o servidor
                    // Exemplo: m_pImpl->m_serverConnector->UploadFile(fileName, fileData);
                    
                    result.serverStatus = ServerFileStatus::IN_SYNC;
                }
                
                if (direction == 0 || direction == 2) {
                    // Servidor -> Local
                    m_pImpl->Log("Baixando arquivo do servidor: " + filePath);
                    
                    // Simular download do servidor
                    // Em uma implementação real, faríamos:
                    // std::vector<uint8_t> serverData = m_pImpl->m_serverConnector->DownloadFile(fileName);
                    
                    // Fazer backup antes de atualizar o arquivo local
                    BackupInfo backup = CreateBackup(filePath, 
                                                  FileOperation::MODIFY, 
                                                  "Sincronização com servidor");
                    
                    result.backupPath = backup.backupPath;
                    
                    // Atualizar o arquivo local com os dados do servidor
                    // Em uma implementação real, escreveríamos os dados do servidor no arquivo local
                    
                    result.serverStatus = ServerFileStatus::IN_SYNC;
                }
                
                result.success = true;
                result.message = "Arquivo sincronizado com sucesso: " + filePath;
                
                m_pImpl->Log(result.message);
            }
            catch (const std::exception& e) {
                result.success = false;
                result.message = "Erro ao sincronizar arquivo: " + std::string(e.what());
                result.serverStatus = ServerFileStatus::CONFLICT;
                
                m_pImpl->Log(result.message);
            }
            
            results[filePath] = result;
        }
        
        return results;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao sincronizar com servidor: " + std::string(e.what()));
        
        // Criar um resultado de erro geral
        FileOperationResult errorResult;
        errorResult.success = false;
        errorResult.timestamp = GetCurrentTimestamp();
        errorResult.message = "Erro ao sincronizar com servidor: " + std::string(e.what());
        errorResult.serverStatus = ServerFileStatus::UNKNOWN;
        
        // Adicionar o erro geral ao mapa de resultados
        results["global_error"] = errorResult;
        
        return results;
    }
}

DirectoryInfo GameFileManager::ListDirectory(const std::string& directory,
                                          bool recursive,
                                          const std::string& filter) {
    DirectoryInfo info;
    info.path = directory;
    info.totalSize = 0;
    info.fileCount = 0;
    info.directoryCount = 0;
    
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminho
        std::string normalizedPath = m_pImpl->NormalizePath(directory);
        info.path = normalizedPath;
        
        // Verificar se o diretório existe
        if (!fs::exists(normalizedPath) || !fs::is_directory(normalizedPath)) {
            throw std::runtime_error("Diretório não encontrado: " + normalizedPath);
        }
        
        // Processar filtros
        std::vector<std::string> filterExtensions;
        if (!filter.empty()) {
            std::string filterCopy = filter;
            
            // Remover espaços em branco
            filterCopy.erase(std::remove_if(filterCopy.begin(), filterCopy.end(), 
                                         [](unsigned char c) { return std::isspace(c); }),
                          filterCopy.end());
            
            // Dividir por vírgulas
            std::istringstream ss(filterCopy);
            std::string ext;
            while (std::getline(ss, ext, ',')) {
                // Garantir que começa com ponto
                if (!ext.empty() && ext[0] != '.') {
                    ext = "." + ext;
                }
                
                filterExtensions.push_back(ext);
            }
        }
        
        // Percorrer o diretório
        if (recursive) {
            for (const auto& entry : fs::recursive_directory_iterator(normalizedPath)) {
                if (entry.is_regular_file()) {
                    // Verificar filtro
                    if (!filterExtensions.empty()) {
                        std::string fileExt = entry.path().extension().string();
                        std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::tolower);
                        
                        bool matchesFilter = false;
                        for (const auto& ext : filterExtensions) {
                            if (fileExt == ext) {
                                matchesFilter = true;
                                break;
                            }
                        }
                        
                        if (!matchesFilter) {
                            continue;
                        }
                    }
                    
                    // Adicionar arquivo
                    info.files.push_back(entry.path().string());
                    info.totalSize += fs::file_size(entry.path());
                    info.fileCount++;
                }
                else if (entry.is_directory()) {
                    // Adicionar diretório
                    info.subdirectories.push_back(entry.path().string());
                    info.directoryCount++;
                }
            }
        }
        else {
            for (const auto& entry : fs::directory_iterator(normalizedPath)) {
                if (entry.is_regular_file()) {
                    // Verificar filtro
                    if (!filterExtensions.empty()) {
                        std::string fileExt = entry.path().extension().string();
                        std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::tolower);
                        
                        bool matchesFilter = false;
                        for (const auto& ext : filterExtensions) {
                            if (fileExt == ext) {
                                matchesFilter = true;
                                break;
                            }
                        }
                        
                        if (!matchesFilter) {
                            continue;
                        }
                    }
                    
                    // Adicionar arquivo
                    info.files.push_back(entry.path().string());
                    info.totalSize += fs::file_size(entry.path());
                    info.fileCount++;
                }
                else if (entry.is_directory()) {
                    // Adicionar diretório
                    info.subdirectories.push_back(entry.path().string());
                    info.directoryCount++;
                }
            }
        }
        
        return info;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao listar diretório: " + std::string(e.what()));
        return info;
    }
}

FileMetadata GameFileManager::GetFileMetadata(const std::string& path) {
    FileMetadata metadata;
    metadata.path = path;
    metadata.serverStatus = ServerFileStatus::UNKNOWN;
    
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminho
        std::string normalizedPath = m_pImpl->NormalizePath(path);
        metadata.path = normalizedPath;
        
        // Verificar se o arquivo existe
        if (!fs::exists(normalizedPath) || !fs::is_regular_file(normalizedPath)) {
            throw std::runtime_error("Arquivo não encontrado: " + normalizedPath);
        }
        
        // Obter informações básicas
        metadata.size = fs::file_size(normalizedPath);
        metadata.lastModified = fs::last_write_time(normalizedPath).time_since_epoch().count();
        
        // Detectar tipo e formato
        auto typeAndFormat = m_pImpl->DetectFileFormat(normalizedPath);
        metadata.type = typeAndFormat.first;
        metadata.format = typeAndFormat.second;
        
        // Verificar status em relação ao servidor
        if (m_pImpl->m_serverSyncEnabled && m_pImpl->IsServerRelatedFile(normalizedPath)) {
            // Em uma implementação real, verificaríamos o status no servidor
            // Por enquanto, apenas marcamos como sincronizado
            metadata.serverStatus = ServerFileStatus::IN_SYNC;
        } else {
            metadata.serverStatus = ServerFileStatus::NOT_APPLICABLE;
        }
        
        // Adicionar metadados específicos para alguns tipos de arquivo
        if (metadata.type == "json") {
            try {
                std::ifstream file(normalizedPath);
                json jsonData = json::parse(file);
                
                // Verificar se é um JSON de item
                if (jsonData.contains("id") && jsonData.contains("name")) {
                    metadata.customData["item_id"] = jsonData["id"].dump();
                    metadata.customData["item_name"] = jsonData["name"].dump();
                }
                
                // Obtém o número de entradas de primeiro nível
                metadata.customData["entry_count"] = std::to_string(jsonData.size());
                
                // Verificar se tem versão
                if (jsonData.contains("version")) {
                    metadata.version = jsonData["version"].dump();
                }
            }
            catch (...) {
                // Ignorar erros ao analisar JSON
            }
        }
        
        return metadata;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao obter metadados do arquivo: " + std::string(e.what()));
        return metadata;
    }
}

bool GameFileManager::CreateDirectory(const std::string& path, bool createParents) {
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminho
        std::string normalizedPath = m_pImpl->NormalizePath(path);
        
        // Verificar se o diretório já existe
        if (fs::exists(normalizedPath)) {
            if (fs::is_directory(normalizedPath)) {
                m_pImpl->Log("Diretório já existe: " + normalizedPath);
                return true;
            } else {
                throw std::runtime_error("O caminho existe mas não é um diretório: " + normalizedPath);
            }
        }
        
        // Criar diretório
        bool success;
        if (createParents) {
            success = fs::create_directories(normalizedPath);
        } else {
            success = fs::create_directory(normalizedPath);
        }
        
        if (success) {
            m_pImpl->Log("Diretório criado com sucesso: " + normalizedPath);
        } else {
            throw std::runtime_error("Falha ao criar diretório: " + normalizedPath);
        }
        
        return success;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao criar diretório: " + std::string(e.what()));
        return false;
    }
}

bool GameFileManager::DeleteDirectory(const std::string& path, bool recursive) {
    try {
        if (!m_pImpl->m_initialized) {
            throw std::runtime_error("Gerenciador de arquivos não inicializado");
        }
        
        // Normalizar caminho
        std::string normalizedPath = m_pImpl->NormalizePath(path);
        
        // Verificar se o diretório existe
        if (!fs::exists(normalizedPath)) {
            m_pImpl->Log("Diretório não encontrado: " + normalizedPath);
            return false;
        }
        
        if (!fs::is_directory(normalizedPath)) {
            throw std::runtime_error("O caminho não é um diretório: " + normalizedPath);
        }
        
        // Excluir diretório
        std::uintmax_t removedCount;
        if (recursive) {
            removedCount = fs::remove_all(normalizedPath);
        } else {
            // Verificar se o diretório está vazio
            if (!fs::is_empty(normalizedPath)) {
                throw std::runtime_error("Diretório não está vazio: " + normalizedPath);
            }
            
            removedCount = fs::remove(normalizedPath) ? 1 : 0;
        }
        
        if (removedCount > 0) {
            m_pImpl->Log("Diretório excluído com sucesso: " + normalizedPath + 
                        " (" + std::to_string(removedCount) + " itens removidos)");
            return true;
        } else {
            throw std::runtime_error("Falha ao excluir diretório: " + normalizedPath);
        }
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao excluir diretório: " + std::string(e.what()));
        return false;
    }
}

bool GameFileManager::FileExists(const std::string& path) {
    try {
        if (!m_pImpl->m_initialized) {
            return false;
        }
        
        // Normalizar caminho
        std::string normalizedPath = m_pImpl->NormalizePath(path);
        
        return fs::exists(normalizedPath) && fs::is_regular_file(normalizedPath);
    }
    catch (...) {
        return false;
    }
}

bool GameFileManager::DirectoryExists(const std::string& path) {
    try {
        if (!m_pImpl->m_initialized) {
            return false;
        }
        
        // Normalizar caminho
        std::string normalizedPath = m_pImpl->NormalizePath(path);
        
        return fs::exists(normalizedPath) && fs::is_directory(normalizedPath);
    }
    catch (...) {
        return false;
    }
}

void GameFileManager::SetLogCallback(std::function<void(const std::string&)> logCallback) {
    m_pImpl->m_logCallback = logCallback;
}

BackupInfo GameFileManager::CreateBackup(const std::string& path, 
                                       FileOperation operation,
                                       const std::string& description) {
    try {
        return m_pImpl->CreateBackup(path, operation, description);
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao criar backup: " + std::string(e.what()));
        
        // Retornar um backup vazio em caso de erro
        BackupInfo emptyBackup;
        emptyBackup.timestamp = GetCurrentTimestamp();
        return emptyBackup;
    }
}

std::string GameFileManager::GenerateBackupPath(const std::string& originalPath, 
                                             FileOperation operation) {
    try {
        return m_pImpl->GenerateBackupPath(originalPath, operation);
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao gerar caminho de backup: " + std::string(e.what()));
        return "";
    }
}

bool GameFileManager::ApplyServerSync(const std::string& path, 
                                    const FileOperationResult& result) {
    // Em uma implementação real, este método sincronizaria o arquivo com o servidor
    // com base no resultado da operação
    // Por enquanto, apenas retornamos sucesso
    return true;
}

} // namespace WYDStudio
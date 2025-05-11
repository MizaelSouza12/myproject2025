#pragma once

#include "../core/WYDTypes.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <queue>
#include <mutex>
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace WYD {
    class SistemaLog {
    private:
        struct LogEntry {
            DWORD id;
            std::string message;
            BYTE type;
            DWORD timestamp;
            DWORD characterId;
            std::string characterName;
            DWORD targetId;
            std::string targetName;
            std::string details;
        };

        struct LogCategory {
            BYTE id;
            std::string name;
            std::string description;
            bool isEnabled;
            std::string filePath;
        };

        std::unordered_map<BYTE, LogCategory> categories;
        std::unordered_map<BYTE, std::ofstream> logFiles;
        std::mutex logMutex;

        // Callbacks
        std::function<void(const LogEntry&)> logCallback;

    public:
        SistemaLog() {
            initializeDefaultCategories();
        }

        ~SistemaLog() {
            closeAllLogFiles();
        }

        // Inicialização
        void initializeDefaultCategories() {
            // Log de Sistema
            LogCategory system{};
            system.id = 0;
            system.name = "Sistema";
            system.description = "Logs do sistema";
            system.isEnabled = true;
            system.filePath = "logs/system.log";
            categories[0] = system;

            // Log de Combate
            LogCategory combat{};
            combat.id = 1;
            combat.name = "Combate";
            combat.description = "Logs de combate";
            combat.isEnabled = true;
            combat.filePath = "logs/combat.log";
            categories[1] = combat;

            // Log de Chat
            LogCategory chat{};
            chat.id = 2;
            chat.name = "Chat";
            chat.description = "Logs de chat";
            chat.isEnabled = true;
            chat.filePath = "logs/chat.log";
            categories[2] = chat;

            // Log de Administração
            LogCategory admin{};
            admin.id = 3;
            admin.name = "Admin";
            admin.description = "Logs administrativos";
            admin.isEnabled = true;
            admin.filePath = "logs/admin.log";
            categories[3] = admin;

            // Log de Erros
            LogCategory error{};
            error.id = 4;
            error.name = "Erro";
            error.description = "Logs de erros";
            error.isEnabled = true;
            error.filePath = "logs/error.log";
            categories[4] = error;
        }

        // Gerenciamento de categorias
        bool addCategory(const LogCategory& category) {
            std::lock_guard<std::mutex> lock(logMutex);
            if (categories.find(category.id) != categories.end()) {
                return false;
            }

            categories[category.id] = category;
            return true;
        }

        bool removeCategory(BYTE categoryId) {
            std::lock_guard<std::mutex> lock(logMutex);
            auto it = categories.find(categoryId);
            if (it == categories.end()) {
                return false;
            }

            // Fechar arquivo de log
            auto fileIt = logFiles.find(categoryId);
            if (fileIt != logFiles.end()) {
                fileIt->second.close();
                logFiles.erase(fileIt);
            }

            categories.erase(it);
            return true;
        }

        // Sistema de log
        bool log(BYTE categoryId, const std::string& message, DWORD characterId = 0, const std::string& characterName = "", DWORD targetId = 0, const std::string& targetName = "", const std::string& details = "") {
            std::lock_guard<std::mutex> lock(logMutex);

            auto it = categories.find(categoryId);
            if (it == categories.end() || !it->second.isEnabled) {
                return false;
            }

            // Criar entrada de log
            LogEntry entry{};
            entry.id = generateLogId();
            entry.message = message;
            entry.type = categoryId;
            entry.timestamp = GetTickCount();
            entry.characterId = characterId;
            entry.characterName = characterName;
            entry.targetId = targetId;
            entry.targetName = targetName;
            entry.details = details;

            // Escrever no arquivo
            writeLogEntry(entry);

            // Chamar callback
            if (logCallback) {
                logCallback(entry);
            }

            return true;
        }

        // Gerenciamento de arquivos
        bool openLogFile(BYTE categoryId) {
            auto it = categories.find(categoryId);
            if (it == categories.end()) {
                return false;
            }

            auto& category = it->second;
            auto& file = logFiles[categoryId];

            if (file.is_open()) {
                return true;
            }

            file.open(category.filePath, std::ios::app);
            return file.is_open();
        }

        void closeLogFile(BYTE categoryId) {
            auto it = logFiles.find(categoryId);
            if (it != logFiles.end()) {
                it->second.close();
                logFiles.erase(it);
            }
        }

        void closeAllLogFiles() {
            for (auto& file : logFiles) {
                file.second.close();
            }
            logFiles.clear();
        }

        // Callbacks
        void registerLogCallback(std::function<void(const LogEntry&)> callback) {
            logCallback = callback;
        }

        // Getters
        const LogCategory& getCategory(BYTE categoryId) const {
            return categories.at(categoryId);
        }

        bool isCategoryEnabled(BYTE categoryId) const {
            auto it = categories.find(categoryId);
            return (it != categories.end()) ? it->second.isEnabled : false;
        }

    private:
        DWORD generateLogId() {
            static DWORD nextId = 1;
            return nextId++;
        }

        void writeLogEntry(const LogEntry& entry) {
            auto& file = logFiles[entry.type];
            if (!file.is_open()) {
                if (!openLogFile(entry.type)) {
                    return;
                }
            }

            // Formatar timestamp
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");

            // Formatar mensagem
            std::stringstream logMessage;
            logMessage << "[" << ss.str() << "] ";
            logMessage << "[" << categories[entry.type].name << "] ";
            
            if (!entry.characterName.empty()) {
                logMessage << "[" << entry.characterName << "] ";
            }
            
            logMessage << entry.message;
            
            if (!entry.targetName.empty()) {
                logMessage << " -> " << entry.targetName;
            }
            
            if (!entry.details.empty()) {
                logMessage << " | " << entry.details;
            }

            // Escrever no arquivo
            file << logMessage.str() << std::endl;
            file.flush();
        }
    };
} 
/**
 * main.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/tmsrv/main.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include <iostream>
#include <string>
#include <csignal>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <filesystem>
#include <memory>
#include <optional>
#include <map>
#include "TMServer.h"

using namespace wydbr::tmsrv;

constexpr const char* DEFAULT_CONFIG = "../config/TMSrv.conf";
constexpr const char* LOG_FILE = "../logs/tmsrv.log";

enum class Mode {
    Normal,
    Maintenance,
    Stats,
    Help
};

struct Args {
    std::string configPath = DEFAULT_CONFIG;
    bool debugMode = false;
    Mode mode = Mode::Normal;
};

static std::unique_ptr<TMServer> g_server;

void log(const std::string& msg) {
    std::cout << "[LOG] " << msg << std::endl;
    std::ofstream logFile(LOG_FILE, std::ios::app);
    if (logFile) logFile << "[LOG] " << msg << std::endl;
}

void logError(const std::string& msg) {
    std::cerr << "[ERRO] " << msg << std::endl;
    std::ofstream logFile(LOG_FILE, std::ios::app);
    if (logFile) logFile << "[ERRO] " << msg << std::endl;
}

void signalHandler(int signal) {
    log("Sinal " + std::to_string(signal) + " recebido. Iniciando desligamento do servidor...");
    if (g_server) {
        g_server->shutdown();
        g_server.reset();
    }
    std::exit(signal);
}

void showHelp(const std::string& programName) {
    std::cout << "Uso: " << programName << " [opções]\n"
              << "Opções:\n"
              << "  -h, --help        Mostra esta ajuda\n"
              << "  -c, --config      Caminho para o arquivo de configuração (padrão: " << DEFAULT_CONFIG << ")\n"
              << "  -d, --debug       Inicia em modo debug (mais logs)\n"
              << "  -m, --maintenance Realiza manutenção e sai\n"
              << "  -s, --stats       Mostra estatísticas de uso\n";
}

std::optional<Args> parseArgs(int argc, char* argv[]) {
    Args args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            args.mode = Mode::Help;
            return args;
        } else if (arg == "-c" || arg == "--config") {
            if (i + 1 < argc) {
                args.configPath = argv[++i];
            } else {
                logError("Erro: Caminho de configuração não especificado");
                return std::nullopt;
            }
        } else if (arg == "-d" || arg == "--debug") {
            args.debugMode = true;
        } else if (arg == "-m" || arg == "--maintenance") {
            args.mode = Mode::Maintenance;
        } else if (arg == "-s" || arg == "--stats") {
            args.mode = Mode::Stats;
        } else {
            logError("Opção desconhecida: " + arg);
            showHelp(argv[0]);
            return std::nullopt;
        }
    }
    return args;
}

int main(int argc, char* argv[]) {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    auto parsed = parseArgs(argc, argv);
    if (!parsed) return 1;
    Args args = *parsed;

    if (args.mode == Mode::Help) {
        showHelp(argv[0]);
        return 0;
    }

    if (!std::filesystem::exists(args.configPath)) {
        logError("Erro: Arquivo de configuração não encontrado: " + args.configPath);
        return 1;
    }

    try {
        log("=============================================");
        log("          WYDBRASIL - TMSrv " + std::string(SERVER_VERSION));
        log("=============================================");
        log("Iniciando servidor...");

        g_server = std::make_unique<TMServer>(args.configPath);

        if (args.mode == Mode::Maintenance) {
            log("Iniciando manutenção do servidor...");
            if (!g_server->initialize()) {
                logError("Erro: Falha ao inicializar servidor para manutenção");
                return 1;
            }
            g_server->performMaintenance(true);
            log("Manutenção concluída com sucesso");
            g_server->shutdown();
            return 0;
        }

        if (!g_server->initialize()) {
            logError("Erro: Falha ao inicializar servidor");
            return 1;
        }
        if (!g_server->start()) {
            logError("Erro: Falha ao iniciar servidor");
            return 1;
        }

        if (args.mode == Mode::Stats) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            auto stats = g_server->getStats();
            log("Estatísticas do servidor:");
            for (const auto& pair : stats) {
                log("  " + pair.first + ": " + pair.second);
            }
            g_server->shutdown();
            return 0;
        }

        log("Servidor em execução. Pressione Ctrl+C para sair.");
        while (g_server->isRunning()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        log("Servidor finalizado.");
        g_server->shutdown();
        return 0;
    } catch (const std::exception& e) {
        logError(std::string("Erro fatal: ") + e.what());
        if (g_server) g_server->shutdown();
        return 1;
    } catch (...) {
        logError("Erro fatal desconhecido.");
        if (g_server) g_server->shutdown();
        return 1;
    }
}
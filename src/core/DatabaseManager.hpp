#pragma once

// DatabaseManager.hpp - Interface gerada automaticamente
// Implemente as funções conforme a arquitetura do servidor
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>

class DatabaseManager {
public:
    DatabaseManager();
    virtual ~DatabaseManager();

    virtual void initialize();
    virtual void shutdown();
};

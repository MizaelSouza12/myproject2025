#pragma once

// CraftingSystem.hpp - Interface gerada automaticamente
// Implemente as funções conforme a arquitetura do servidor
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>

class CraftingSystem {
public:
    CraftingSystem();
    virtual ~CraftingSystem();

    virtual void initialize();
    virtual void shutdown();
};

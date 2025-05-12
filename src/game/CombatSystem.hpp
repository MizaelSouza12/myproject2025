#pragma once

// CombatSystem.hpp - Interface gerada automaticamente
// Implemente as funções conforme a arquitetura do servidor
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>

class CombatSystem {
public:
    CombatSystem();
    virtual ~CombatSystem();

    virtual void initialize();
    virtual void shutdown();
};

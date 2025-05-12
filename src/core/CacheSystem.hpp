#pragma once

// CacheSystem.hpp - Interface gerada automaticamente
// Implemente as funções conforme a arquitetura do servidor
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>

class CacheSystem {
public:
    CacheSystem();
    virtual ~CacheSystem();

    virtual void initialize();
    virtual void shutdown();
};

#pragma once

// NetworkManager.hpp - Interface gerada automaticamente
// Implemente as funções conforme a arquitetura do servidor
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>

class NetworkManager {
public:
    NetworkManager();
    virtual ~NetworkManager();

    virtual void initialize();
    virtual void shutdown();
};

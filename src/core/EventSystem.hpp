#pragma once

// EventSystem.hpp - Interface gerada automaticamente
// Implemente as funções conforme a arquitetura do servidor
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>

class EventSystem {
public:
    EventSystem();
    virtual ~EventSystem();

    virtual void initialize();
    virtual void shutdown();
};

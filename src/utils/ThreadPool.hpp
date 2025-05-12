#pragma once

// ThreadPool.hpp - Interface gerada automaticamente
// Implemente as funções conforme a arquitetura do servidor
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>

class ThreadPool {
public:
    ThreadPool();
    virtual ~ThreadPool();

    virtual void initialize();
    virtual void shutdown();
};

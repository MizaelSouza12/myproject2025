#pragma once

// ClusterManager.hpp - Interface gerada automaticamente
// Implemente as funções conforme a arquitetura do servidor
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>

class ClusterManager {
public:
    ClusterManager();
    virtual ~ClusterManager();

    virtual void initialize();
    virtual void shutdown();
};

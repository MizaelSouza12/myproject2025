const fs = require('fs');
const path = require('path');

const structure = {
  src: {
    core: [
      'SystemManager.cpp', 'SystemManager.hpp',
      'NetworkManager.cpp', 'NetworkManager.hpp',
      'DatabaseManager.cpp', 'DatabaseManager.hpp',
      'Logger.cpp', 'Logger.hpp',
      'Config.cpp', 'Config.hpp',
      'ClusterManager.cpp', 'ClusterManager.hpp',
      'EventSystem.cpp', 'EventSystem.hpp',
      'CacheSystem.cpp', 'CacheSystem.hpp',
      'SecuritySystem.cpp', 'SecuritySystem.hpp',
      'EventBroker.cpp', 'EventBroker.hpp'
    ],
    game: [
      'CharacterSystem.cpp', 'CharacterSystem.hpp',
      'CombatSystem.cpp', 'CombatSystem.hpp',
      'GuildSystem.cpp', 'GuildSystem.hpp',
      'QuestSystem.cpp', 'QuestSystem.hpp',
      'InventorySystem.cpp', 'InventorySystem.hpp',
      'TradeSystem.cpp', 'TradeSystem.hpp',
      'CraftingSystem.cpp', 'CraftingSystem.hpp',
      'TerritorialWarSystem.cpp', 'TerritorialWarSystem.hpp'
    ],
    utils: [
      'ThreadPool.cpp', 'ThreadPool.hpp',
      'EventEmitter.cpp', 'EventEmitter.hpp',
      'Timer.cpp', 'Timer.hpp'
    ]
  },
  config: ['config.json'],
  scripts: ['build.bat', 'deploy.bat', 'backup.bat'],
  '': ['main.cpp', 'CMakeLists.txt']
};

const cppHeader = (name) => `#pragma once

// ${name} - Interface gerada automaticamente
// Implemente as funções conforme a arquitetura do servidor
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>

class ${name.replace('.hpp', '')} {
public:
    ${name.replace('.hpp', '')}();
    virtual ~${name.replace('.hpp', '')}();

    virtual void initialize();
    virtual void shutdown();
};
`;

const cppSource = (name) => `#include "${name.replace('.cpp', '.hpp')}"

// ${name} - Implementação gerada automaticamente

${name.replace('.cpp', '')}::${name.replace('.cpp', '')}() {}
${name.replace('.cpp', '')}::~${name.replace('.cpp', '')}() {}

void ${name.replace('.cpp', '')}::initialize() {
    // Inicialização do sistema
}

void ${name.replace('.cpp', '')}::shutdown() {
    // Finalização do sistema
}
`;

const mainCpp = `#include <iostream>
#include "src/core/SystemManager.hpp"
#include "src/core/Logger.hpp"

int main() {
    Logger logger;
    logger.initialize();
    logger.info("Servidor WYDBR2.0 C++ iniciado!");

    SystemManager systemManager;
    systemManager.initialize();

    // Loop principal do servidor
    while (true) {
        // systemManager.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    systemManager.shutdown();
    logger.shutdown();
    return 0;
}
`;

const cmakeLists = `cmake_minimum_required(VERSION 3.15)
project(WYDBR2.0)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(wydbr2 \${SOURCES})

# Adicione bibliotecas externas aqui (Boost, spdlog, nlohmann_json, etc)
# find_package(Boost REQUIRED)
# target_link_libraries(wydbr2 Boost::boost)
`;

const configJson = `{
  "db_host": "localhost",
  "db_user": "wyd",
  "db_pass": "senha",
  "db_name": "wydbr2",
  "server_port": 8282,
  "log_level": "info"
}
`;

const batchScript = (name) => `@echo off
REM Script de ${name.replace('.bat', '')} gerado automaticamente
cd ..
mkdir build
cd build
cmake ..
cmake --build . --config Release
`;

function createFile(filePath, content) {
  if (!fs.existsSync(filePath)) {
    fs.writeFileSync(filePath, content, 'utf8');
    console.log('Criado:', filePath);
  }
}

function createStructure(base, struct) {
  for (const key in struct) {
    const dirPath = path.join(base, key);
    if (key) fs.mkdirSync(dirPath, { recursive: true });

    if (Array.isArray(struct[key])) {
      struct[key].forEach(file => {
        let content = '';
        if (file.endsWith('.hpp')) content = cppHeader(file);
        else if (file.endsWith('.cpp')) content = cppSource(file);
        else if (file === 'main.cpp') content = mainCpp;
        else if (file === 'CMakeLists.txt') content = cmakeLists;
        else if (file === 'config.json') content = configJson;
        else if (file.endsWith('.bat')) content = batchScript(file);
        createFile(path.join(dirPath, file), content);
      });
    } else {
      createStructure(dirPath, struct[key]);
    }
  }
}

// Execução
debugger;
createStructure('.', structure);
console.log('\nProjeto C++ WYDBR2.0 gerado com sucesso!'); 
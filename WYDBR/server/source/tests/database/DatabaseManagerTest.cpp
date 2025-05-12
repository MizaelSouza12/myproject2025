/**
 * @file DatabaseManagerTest.cpp
 * @brief Testes unitários para o DatabaseManager
 * 
 * Este arquivo contém testes básicos para verificar o funcionamento
 * do DatabaseManager, garantindo compatibilidade com os dados do WYD.
 */

#include "../../include/database/DatabaseManager.h"
#include "../../include/TM_Mob.h"
#include "../../include/TM_Item.h"
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <filesystem>

using namespace wydbr::database;

// Função auxiliar para impressão de mensagens de teste
/**
 * printTestMessage
 * 
 * Implementa a funcionalidade printTestMessage conforme especificação original.
 * @param testName Parâmetro testName
 * @param success Parâmetro success
 */

void printTestMessage(const std::string& testName, bool success){
    std::cout << "[TEST] " << testName << ": " << (success ? "PASSOU" : "FALHOU") << std::endl;
}

// Função auxiliar para criar tabelas de teste
/**
 * createTestTables
 * 
 * Implementa a funcionalidade createTestTables conforme especificação original.
 * @return Retorna bool
 */

bool createTestTables(){
    // Cria tabelas para os testes
    DatabaseResult result;

    // Tabela de Characters
    result = DatabaseManager::getInstance().executeNonQuery(
        "CREATE TABLE Characters ("
        "char_id INTEGER PRIMARY KEY, "
        "account_id INTEGER, "
        "name TEXT, "
        "clan INTEGER, "
        "merchant INTEGER, "
        "guild INTEGER, "
        "class INTEGER, "
        "rsv INTEGER, "
        "quest INTEGER, "
        "level INTEGER, "
        "strength INTEGER, "
        "intelligence INTEGER, "
        "dexterity INTEGER, "
        "constitution INTEGER, "
        "defense INTEGER, "
        "damage INTEGER, "
        "hp INTEGER, "
        "max_hp INTEGER, "
        "mp INTEGER, "
        "max_mp INTEGER, "
        "pos_x INTEGER, "
        "pos_y INTEGER, "
        "guild_level INTEGER, "
        "special_bonus INTEGER, "
        "learned_skill INTEGER)"
    );

    if (result != DatabaseResult::SUCCESS) {
        std::cerr << "Erro ao criar tabela Characters" << std::endl;
        return false;
    }

    // Tabela de CharItems
    result = DatabaseManager::getInstance().executeNonQuery(
        "CREATE TABLE CharItems ("
        "item_id INTEGER PRIMARY KEY, "
        "char_id INTEGER, "
        "slot INTEGER, "
        "item_id INTEGER, "
        "effect1 INTEGER, "
        "effect2 INTEGER, "
        "effect3 INTEGER, "
        "value1 INTEGER, "
        "value2 INTEGER, "
        "value3 INTEGER)"
    );

    if (result != DatabaseResult::SUCCESS) {
        std::cerr << "Erro ao criar tabela CharItems" << std::endl;
        return false;
    }

    // Insere dados de teste
    result = DatabaseManager::getInstance().executeNonQuery(
        "INSERT INTO Characters ("
        "char_id, account_id, name, clan, merchant, guild, class, rsv, quest, "
        "level, strength, intelligence, dexterity, constitution, defense, damage, "
        "hp, max_hp, mp, max_mp, pos_x, pos_y, guild_level, special_bonus, learned_skill) "
        "VALUES (1, 1, 'TestCharacter', 0, 0, 0, 1, 0, 0, "
        "1, 10, 10, 10, 10, 10, 10, 100, 100, 100, 100, 2000, 2000, 0, 0, 0)"
    );

    if (result != DatabaseResult::SUCCESS) {
        std::cerr << "Erro ao inserir dados de teste em Characters" << std::endl;
        return false;
    }

    result = DatabaseManager::getInstance().executeNonQuery(
        "INSERT INTO CharItems ("
        "item_id, char_id, slot, item_id, effect1, effect2, effect3, value1, value2, value3) "
        "VALUES (1, 1, 0, 100, 1, 0, 0, 5, 0, 0)"
    );

    if (result != DatabaseResult::SUCCESS) {
        std::cerr << "Erro ao inserir dados de teste em CharItems" << std::endl;
        return false;
    }

    return true;
}

// Teste de inicialização do DatabaseManager
/**
 * testInitialize
 * 
 * Implementa a funcionalidade testInitialize conforme especificação original.
 * @return Retorna bool
 */

bool testInitialize(){
    // Cria diretório de teste se não existir
    std::filesystem::create_directories("test_db");

    // Inicializa o gerenciador com banco de dados baseado em arquivo
    bool result = DatabaseManager::getInstance().initialize(
        DatabaseType::FILE, 
        "test_db", 
        2  // Tamanho do pool
    );

    return result;
}

// Teste de carga de personagem
/**
 * testLoadCharacter
 * 
 * Implementa a funcionalidade testLoadCharacter conforme especificação original.
 * @return Retorna bool
 */

bool testLoadCharacter(){
    STRUCT_MOB mob;
    DatabaseResult result = DatabaseManager::getInstance().loadCharacter(1, mob);

    if (result != DatabaseResult::SUCCESS) {
        std::cerr << "Erro ao carregar personagem: " << static_cast<int>(result) << std::endl;
        return false;
    }

    // Verifica alguns dados
    bool success = true;
    success = success && (std::string(mob.MobName) == "TestCharacter");
    success = success && (mob.Class == 1);
    success = success && (mob.BaseScore.Level == 1);
    success = success && (mob.BaseScore.Str == 10);
    success = success && (mob.BaseScore.Int == 10);
    success = success && (mob.BaseScore.Dex == 10);
    success = success && (mob.BaseScore.Con == 10);
    success = success && (mob.BaseScore.Hp == 100);
    success = success && (mob.BaseScore.Mp == 100);

    if (!success) {
        std::cerr << "Dados do personagem carregado não correspondem aos esperados" << std::endl;
    }

    return success;
}

// Teste de salvamento de personagem
/**
 * testSaveCharacter
 * 
 * Implementa a funcionalidade testSaveCharacter conforme especificação original.
 * @return Retorna bool
 */

bool testSaveCharacter(){
    STRUCT_MOB mob;
    std::strncpy(mob.MobName, "UpdatedChar");
    mob.Class = 2;
    mob.BaseScore.Level = 2;
    mob.BaseScore.Str = 15;
    mob.BaseScore.Int = 15;
    mob.BaseScore.Dex = 15;
    mob.BaseScore.Con = 15;
    mob.BaseScore.Hp = 150;
    mob.BaseScore.MaxHp = 150;
    mob.BaseScore.Mp = 150;
    mob.BaseScore.MaxMp = 150;
    mob.Position.X = 2100;
    mob.Position.Y = 2100;

    DatabaseResult result = DatabaseManager::getInstance().saveCharacter(1, mob);

    if (result != DatabaseResult::SUCCESS) {
        std::cerr << "Erro ao salvar personagem: " << static_cast<int>(result) << std::endl;
        return false;
    }

    // Verifica se os dados foram salvos corretamente
    STRUCT_MOB loadedMob;
    result = DatabaseManager::getInstance().loadCharacter(1, loadedMob);

    if (result != DatabaseResult::SUCCESS) {
        std::cerr << "Erro ao carregar personagem após salvar: " << static_cast<int>(result) << std::endl;
        return false;
    }

    bool success = true;
    success = success && (std::string(loadedMob.MobName) == "UpdatedChar");
    success = success && (loadedMob.Class == 2);
    success = success && (loadedMob.BaseScore.Level == 2);
    success = success && (loadedMob.BaseScore.Str == 15);
    success = success && (loadedMob.BaseScore.Int == 15);
    success = success && (loadedMob.BaseScore.Dex == 15);
    success = success && (loadedMob.BaseScore.Con == 15);
    success = success && (loadedMob.BaseScore.Hp == 150);
    success = success && (loadedMob.BaseScore.Mp == 150);
    success = success && (loadedMob.Position.X == 2100);
    success = success && (loadedMob.Position.Y == 2100);

    if (!success) {
        std::cerr << "Dados salvos e depois carregados não correspondem" << std::endl;
    }

    return success;
}

// Teste de carga e salvamento de itens
/**
 * testItemOperations
 * 
 * Implementa a funcionalidade testItemOperations conforme especificação original.
 * @return Retorna bool
 */

bool testItemOperations(){
    // Cria alguns itens de teste
    std::vector<STRUCT_ITEM> items;
    
    STRUCT_ITEM item1;
    item1.sIndex = 100;
    item1.stEffect[0] = 1;
    item1.stEFV1 = 10;
    item1.stEffect[1] = 2;
    item1.stEFV2 = 20;
    item1.stEffect[2] = 3;
    item1.stEFV3 = 30;
    items.push_back(item1);
    
    STRUCT_ITEM item2;
    item2.sIndex = 200;
    item2.stEffect[0] = 4;
    item2.stEFV1 = 40;
    item2.stEffect[1] = 5;
    item2.stEFV2 = 50;
    item2.stEffect[2] = 6;
    item2.stEFV3 = 60;
    items.push_back(item2);

    // Salva os itens
    DatabaseResult result = DatabaseManager::getInstance().saveCharacterItems(1, items);
    
    if (result != DatabaseResult::SUCCESS) {
        std::cerr << "Erro ao salvar itens: " << static_cast<int>(result) << std::endl;
        return false;
    }

    // Carrega os itens
    std::vector<STRUCT_ITEM> loadedItems;
    result = DatabaseManager::getInstance().loadCharacterItems(1, loadedItems);
    
    if (result != DatabaseResult::SUCCESS) {
        std::cerr << "Erro ao carregar itens: " << static_cast<int>(result) << std::endl;
        return false;
    }

    // Verifica se carregou corretamente
    if (loadedItems.size() != 2) {
        std::cerr << "Número de itens carregados incorreto: " << loadedItems.size() << " (esperado: 2)" << std::endl;
        return false;
    }

    bool success = true;
    success = success && (loadedItems[0].sIndex == 100);
    success = success && (loadedItems[0].stEffect[0] == 1);
    success = success && (loadedItems[0].stEFV1 == 10);
    success = success && (loadedItems[0].stEffect[1] == 2);
    success = success && (loadedItems[0].stEFV2 == 20);
    success = success && (loadedItems[0].stEffect[2] == 3);
    success = success && (loadedItems[0].stEFV3 == 30);
    
    success = success && (loadedItems[1].sIndex == 200);
    success = success && (loadedItems[1].stEffect[0] == 4);
    success = success && (loadedItems[1].stEFV1 == 40);
    success = success && (loadedItems[1].stEffect[1] == 5);
    success = success && (loadedItems[1].stEFV2 == 50);
    success = success && (loadedItems[1].stEffect[2] == 6);
    success = success && (loadedItems[1].stEFV3 == 60);

    if (!success) {
        std::cerr << "Dados dos itens não correspondem aos esperados" << std::endl;
    }

    return success;
}

// Teste principal
/**
 * main
 * 
 * Implementa a funcionalidade main conforme especificação original.
 * @return Retorna int
 */

int main(){
    std::cout << "==============================================" << std::endl;
    std::cout << " DatabaseManager - Testes Unitários" << std::endl;
    std::cout << "==============================================" << std::endl;
    
    try {
        // Testa inicialização
        bool initSuccess = testInitialize();
        printTestMessage("Inicialização do DatabaseManager", initSuccess);
        if (!initSuccess) {
            std::cerr << "Falha crítica na inicialização, abortando testes." << std::endl;
            return 1;
        }

        // Cria tabelas de teste
        bool tablesCreated = createTestTables();
        printTestMessage("Criação de tabelas de teste", tablesCreated);
        if (!tablesCreated) {
            std::cerr << "Falha na criação de tabelas, abortando testes." << std::endl;
            return 1;
        }

        // Executa os testes
        printTestMessage("Carga de personagem", testLoadCharacter());
        printTestMessage("Salvamento de personagem", testSaveCharacter());
        printTestMessage("Operações com itens", testItemOperations());

        // Finaliza o DatabaseManager
        DatabaseManager::getInstance().shutdown();
        std::cout << "DatabaseManager encerrado com sucesso." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "ERRO: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERRO desconhecido!" << std::endl;
        return 1;
    }

    std::cout << "Testes concluídos." << std::endl;
    return 0;
}
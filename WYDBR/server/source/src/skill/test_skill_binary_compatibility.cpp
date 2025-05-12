/**
 * test_skill_binary_compatibility.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/skill/test_skill_binary_compatibility.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 * 
 * Este programa de teste verifica a compatibilidade binária da estrutura STRUCT_SKILLDATA
 * implementada no projeto WYDBR com a estrutura utilizada no WYD original.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <iomanip>
#include <stdexcept>
#include <cstdint>

#include "../../include/skill/SkillLoader.h"
#include "../../include/skill/SkillBinStructs.h"
#include "../../include/skill/SkillConstants.h"
#include "../../include/TMStruct.h"

// Verificação de tamanho em tempo de compilação
static_assert(sizeof(STRUCT_SKILLDATA) == 92, "STRUCT_SKILLDATA deve ter exatamente 92 bytes");
static_assert(sizeof(wydbr::skill::BIN_HEADER) == 32, "BIN_HEADER deve ter exatamente 32 bytes");
static_assert(sizeof(wydbr::skill::BIN_SKILL_ENTRY) == 94, "BIN_SKILL_ENTRY deve ter exatamente 94 bytes");

/**
 * Gera um arquivo binário de teste com dados de habilidades
 * @param filePath Caminho para o arquivo a ser gerado
 * @return true se a operação foi bem-sucedida, false caso contrário
 */
bool generateTestSkillFile(const std::string& filePath) {
    try {
        std::cout << "Gerando arquivo de teste: " << filePath << std::endl;
        
        // Criando arquivo
        std::ofstream file(filePath, std::ios::binary);
        if (!file) {
            std::cerr << "Erro ao criar arquivo: " << filePath << std::endl;
            return false;
        }
        
        // Criar cabeçalho
        wydbr::skill::BIN_HEADER header;
        std::memcpy(header.Signature, "SKIL", 4);
        header.Version = 0x00010000;
        header.SkillCount = 5; // 5 habilidades de teste
        std::memset(header.Reserved, 0, sizeof(header.Reserved));
        
        // Escrever cabeçalho
        file.write(reinterpret_cast<const char*>(&header), sizeof(wydbr::skill::BIN_HEADER));
        
        // Criar habilidades de teste
        for (uint16_t i = 1; i <= 5; ++i) {
            // Criar entrada de habilidade
            wydbr::skill::BIN_SKILL_ENTRY entry;
            entry.SkillID = i;
            std::memset(&entry.Data, 0, sizeof(STRUCT_SKILLDATA));
            
            // Preenchendo com dados de teste
            entry.Data.Points = i * 10;
            entry.Data.Target = i % 3;
            entry.Data.Mana = i * 20;
            entry.Data.Delay = i * 1000;
            entry.Data.Range = i * 50;
            entry.Data.InstanceType = i;
            entry.Data.InstanceValue = i * 100;
            entry.Data.TickType = i % 2;
            entry.Data.TickValue = i * 10;
            entry.Data.AffectType = i % 4;
            entry.Data.AffectValue = i * 5;
            entry.Data.Time = i * 5000;
            std::snprintf(entry.Data.Act1, sizeof(entry.Data.Act1), "ACT%d", i);
            std::snprintf(entry.Data.Act2, sizeof(entry.Data.Act2), "TGT%d", i);
            entry.Data.InstanceAttribute = i * 2;
            entry.Data.TickAttribute = i;
            entry.Data.Aggressive = i % 2;
            entry.Data.MaxTarget = i + 1;
            entry.Data.PartyCheck = (i % 2) == 0 ? 1 : 0;
            entry.Data.AffectResist = i * 3;
            entry.Data.PassiveCheck = (i % 3) == 0 ? 1 : 0;
            entry.Data.Unknown = 0;
            
            // Escrever entrada de habilidade
            file.write(reinterpret_cast<const char*>(&entry), sizeof(wydbr::skill::BIN_SKILL_ENTRY));
        }
        
        file.close();
        std::cout << "Arquivo de teste gerado com sucesso" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro ao gerar arquivo de teste: " << e.what() << std::endl;
        return false;
    }
}

/**
 * Imprime os detalhes de uma habilidade
 * @param skillId ID da habilidade
 * @param skill Dados da habilidade
 */
void printSkillDetails(uint16_t skillId, const STRUCT_SKILLDATA& skill) {
    std::cout << "Habilidade ID: " << skillId << std::endl;
    std::cout << "  Pontos: " << skill.Points << std::endl;
    
    // Obter tipo de alvo como string legível
    std::string targetType;
    switch (skill.Target) {
        case wydbr::skill::SKILL_TARGET_NONE: targetType = "Nenhum"; break;
        case wydbr::skill::SKILL_TARGET_SELF: targetType = "Próprio"; break;
        case wydbr::skill::SKILL_TARGET_ALLY: targetType = "Aliado"; break;
        case wydbr::skill::SKILL_TARGET_ENEMY: targetType = "Inimigo"; break;
        case wydbr::skill::SKILL_TARGET_GROUND: targetType = "Área no chão"; break;
        default: targetType = "Desconhecido (" + std::to_string(skill.Target) + ")"; break;
    }
    std::cout << "  Alvo: " << targetType << std::endl;
    
    std::cout << "  Mana: " << skill.Mana << std::endl;
    std::cout << "  Delay: " << skill.Delay << " ms" << std::endl;
    std::cout << "  Alcance: " << skill.Range << std::endl;
    
    // Obter tipo de instância como string legível
    std::string instanceType;
    switch (skill.InstanceType) {
        case wydbr::skill::SKILL_INSTANCE_NONE: instanceType = "Nenhum"; break;
        case wydbr::skill::SKILL_INSTANCE_DAMAGE: instanceType = "Dano físico"; break;
        case wydbr::skill::SKILL_INSTANCE_MAGIC_DAMAGE: instanceType = "Dano mágico"; break;
        case wydbr::skill::SKILL_INSTANCE_HEAL: instanceType = "Cura"; break;
        case wydbr::skill::SKILL_INSTANCE_RESURRECT: instanceType = "Ressurreição"; break;
        default: instanceType = "Desconhecido (" + std::to_string(skill.InstanceType) + ")"; break;
    }
    std::cout << "  Tipo de Instância: " << instanceType << std::endl;
    
    std::cout << "  Valor de Instância: " << skill.InstanceValue << std::endl;
    
    // Obter tipo de tick como string legível
    std::string tickType;
    switch (skill.TickType) {
        case wydbr::skill::SKILL_TICK_NONE: tickType = "Nenhum"; break;
        case wydbr::skill::SKILL_TICK_DAMAGE: tickType = "Dano ao longo do tempo"; break;
        case wydbr::skill::SKILL_TICK_HEAL: tickType = "Cura ao longo do tempo"; break;
        case wydbr::skill::SKILL_TICK_MP_DAMAGE: tickType = "Dano de MP ao longo do tempo"; break;
        case wydbr::skill::SKILL_TICK_MP_HEAL: tickType = "Cura de MP ao longo do tempo"; break;
        default: tickType = "Desconhecido (" + std::to_string(skill.TickType) + ")"; break;
    }
    std::cout << "  Tipo de Tick: " << tickType << std::endl;
    
    std::cout << "  Valor de Tick: " << skill.TickValue << std::endl;
    
    // Obter tipo de efeito como string legível
    std::string affectType;
    switch (skill.AffectType) {
        case wydbr::skill::SKILL_AFFECT_NONE: affectType = "Nenhum"; break;
        case wydbr::skill::SKILL_AFFECT_STUN: affectType = "Atordoamento"; break;
        case wydbr::skill::SKILL_AFFECT_SILENCE: affectType = "Silenciamento"; break;
        case wydbr::skill::SKILL_AFFECT_ROOT: affectType = "Enraizamento"; break;
        default: affectType = "Desconhecido (" + std::to_string(skill.AffectType) + ")"; break;
    }
    std::cout << "  Tipo de Efeito: " << affectType << std::endl;
    
    std::cout << "  Valor de Efeito: " << skill.AffectValue << std::endl;
    std::cout << "  Tempo: " << skill.Time << " ms" << std::endl;
    std::cout << "  Animação 1: " << skill.Act1 << std::endl;
    std::cout << "  Animação 2: " << skill.Act2 << std::endl;
    std::cout << "  Atributo de Instância: " << skill.InstanceAttribute << std::endl;
    std::cout << "  Atributo de Tick: " << skill.TickAttribute << std::endl;
    std::cout << "  Agressivo: " << (skill.Aggressive ? "Sim" : "Não") << std::endl;
    std::cout << "  Máximo de Alvos: " << skill.MaxTarget << std::endl;
    std::cout << "  Verificação de Grupo: " << (skill.PartyCheck ? "Sim" : "Não") << std::endl;
    std::cout << "  Resistência a Efeito: " << skill.AffectResist << std::endl;
    std::cout << "  Habilidade Passiva: " << (skill.PassiveCheck ? "Sim" : "Não") << std::endl;
    std::cout << "  Desconhecido: " << skill.Unknown << std::endl;
    std::cout << std::endl;
}

/**
 * Testa o carregamento e salvamento de habilidades
 * @param inputFile Arquivo de entrada
 * @param outputFile Arquivo de saída
 * @return true se a operação foi bem-sucedida, false caso contrário
 */
bool testLoadAndSave(const std::string& inputFile, const std::string& outputFile) {
    try {
        std::cout << "Testando carregamento e salvamento de habilidades" << std::endl;
        
        // Inicializar o carregador de habilidades
        wydbr::skill::SkillLoader loader;
        
        // Carregar habilidades do arquivo de entrada
        if (!loader.loadFromFile(inputFile)) {
            std::cerr << "Falha ao carregar habilidades do arquivo: " << inputFile << std::endl;
            return false;
        }
        
        // Exibir informações sobre as habilidades carregadas
        std::cout << "Habilidades carregadas: " << loader.getSkillCount() << std::endl;
        
        // Imprimir detalhes de cada habilidade
        for (const auto& pair : loader.getAllSkills()) {
            printSkillDetails(pair.first, pair.second);
        }
        
        // Salvar habilidades para o arquivo de saída
        if (!loader.saveToFile(outputFile)) {
            std::cerr << "Falha ao salvar habilidades para o arquivo: " << outputFile << std::endl;
            return false;
        }
        
        std::cout << "Habilidades salvas com sucesso para o arquivo: " << outputFile << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro no teste de carregamento e salvamento: " << e.what() << std::endl;
        return false;
    }
}

/**
 * Compara dois arquivos binários byte a byte
 * @param file1 Primeiro arquivo
 * @param file2 Segundo arquivo
 * @return true se os arquivos são idênticos, false caso contrário
 */
bool compareFiles(const std::string& file1, const std::string& file2) {
    try {
        std::cout << "Comparando arquivos: " << file1 << " e " << file2 << std::endl;
        
        // Abrir arquivos
        std::ifstream f1(file1, std::ios::binary);
        std::ifstream f2(file2, std::ios::binary);
        
        if (!f1 || !f2) {
            std::cerr << "Erro ao abrir arquivos para comparação" << std::endl;
            return false;
        }
        
        // Obter tamanhos dos arquivos
        f1.seekg(0, std::ios::end);
        f2.seekg(0, std::ios::end);
        
        std::streamsize size1 = f1.tellg();
        std::streamsize size2 = f2.tellg();
        
        if (size1 != size2) {
            std::cerr << "Os arquivos têm tamanhos diferentes" << std::endl;
            std::cerr << "  " << file1 << ": " << size1 << " bytes" << std::endl;
            std::cerr << "  " << file2 << ": " << size2 << " bytes" << std::endl;
            return false;
        }
        
        // Voltar ao início dos arquivos
        f1.seekg(0, std::ios::beg);
        f2.seekg(0, std::ios::beg);
        
        // Comparar byte a byte
        const size_t BUFFER_SIZE = 4096;
        char buffer1[BUFFER_SIZE];
        char buffer2[BUFFER_SIZE];
        
        std::streamsize bytesRead = 0;
        std::streamsize totalBytes = 0;
        
        while (totalBytes < size1) {
            // Calcular o tamanho do próximo bloco
            std::streamsize bytesToRead = std::min(BUFFER_SIZE, static_cast<size_t>(size1 - totalBytes));
            
            // Ler dados dos arquivos
            f1.read(buffer1, bytesToRead);
            f2.read(buffer2, bytesToRead);
            
            // Verificar se a leitura foi bem-sucedida
            bytesRead = f1.gcount();
            if (f2.gcount() != bytesRead) {
                std::cerr << "Erro de leitura durante a comparação" << std::endl;
                return false;
            }
            
            // Comparar os dados
            if (std::memcmp(buffer1, buffer2, bytesRead) != 0) {
                // Encontrar a posição exata da diferença
                for (std::streamsize i = 0; i < bytesRead; ++i) {
                    if (buffer1[i] != buffer2[i]) {
                        std::streamsize offset = totalBytes + i;
                        std::cerr << "Diferença encontrada na posição " << offset << std::endl;
                        std::cerr << "  " << file1 << ": 0x" << std::hex << std::setw(2) << std::setfill('0')
                                  << static_cast<int>(static_cast<unsigned char>(buffer1[i])) << std::endl;
                        std::cerr << "  " << file2 << ": 0x" << std::hex << std::setw(2) << std::setfill('0')
                                  << static_cast<int>(static_cast<unsigned char>(buffer2[i])) << std::endl;
                        return false;
                    }
                }
            }
            
            totalBytes += bytesRead;
        }
        
        std::cout << "Os arquivos são idênticos (" << std::dec << totalBytes << " bytes)" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro ao comparar arquivos: " << e.what() << std::endl;
        return false;
    }
}

/**
 * Testa a funcionalidade de modificação de habilidades (criação, atualização, remoção)
 * @param testFile Arquivo de teste para salvar as alterações
 * @return true se a operação foi bem-sucedida, false caso contrário
 */
bool testSkillModification(const std::string& testFile) {
    try {
        std::cout << "Testando operações de modificação de habilidades" << std::endl;
        
        // Inicializar o carregador de habilidades
        wydbr::skill::SkillLoader loader;
        
        // Criar uma nova habilidade
        uint16_t newSkillId = 100;
        STRUCT_SKILLDATA newSkill;
        std::memset(&newSkill, 0, sizeof(STRUCT_SKILLDATA));
        
        // Preencher dados da nova habilidade
        newSkill.Points = 50;
        newSkill.Target = wydbr::skill::SKILL_TARGET_ENEMY;
        newSkill.Mana = 100;
        newSkill.Delay = 5000;
        newSkill.Range = 150;
        newSkill.InstanceType = wydbr::skill::SKILL_INSTANCE_DAMAGE;
        newSkill.InstanceValue = 500;
        newSkill.TickType = wydbr::skill::SKILL_TICK_NONE;
        newSkill.TickValue = 0;
        newSkill.AffectType = wydbr::skill::SKILL_AFFECT_NONE;
        newSkill.AffectValue = 0;
        newSkill.Time = 0;
        std::snprintf(newSkill.Act1, sizeof(newSkill.Act1), "ATTACK");
        std::snprintf(newSkill.Act2, sizeof(newSkill.Act2), "HIT");
        newSkill.InstanceAttribute = 20;
        newSkill.TickAttribute = 0;
        newSkill.Aggressive = 1;
        newSkill.MaxTarget = 1;
        newSkill.PartyCheck = 0;
        newSkill.AffectResist = 0;
        newSkill.PassiveCheck = 0;
        newSkill.Unknown = 0;
        
        // Criar a habilidade
        if (!loader.createSkill(newSkillId, newSkill)) {
            std::cerr << "Falha ao criar nova habilidade" << std::endl;
            return false;
        }
        std::cout << "Habilidade criada com sucesso (ID: " << newSkillId << ")" << std::endl;
        
        // Verificar se a habilidade foi criada corretamente
        const STRUCT_SKILLDATA* createdSkill = loader.getSkillData(newSkillId);
        if (!createdSkill) {
            std::cerr << "Habilidade criada não encontrada" << std::endl;
            return false;
        }
        std::cout << "Habilidade criada encontrada:" << std::endl;
        printSkillDetails(newSkillId, *createdSkill);
        
        // Atualizar a habilidade
        STRUCT_SKILLDATA updatedSkill = *createdSkill;
        updatedSkill.InstanceValue = 1000;
        updatedSkill.Mana = 200;
        updatedSkill.Range = 200;
        
        if (!loader.updateSkill(newSkillId, updatedSkill)) {
            std::cerr << "Falha ao atualizar habilidade" << std::endl;
            return false;
        }
        std::cout << "Habilidade atualizada com sucesso (ID: " << newSkillId << ")" << std::endl;
        
        // Verificar se a habilidade foi atualizada corretamente
        const STRUCT_SKILLDATA* updatedSkillData = loader.getSkillData(newSkillId);
        if (!updatedSkillData) {
            std::cerr << "Habilidade atualizada não encontrada" << std::endl;
            return false;
        }
        if (updatedSkillData->InstanceValue != 1000 || updatedSkillData->Mana != 200 || updatedSkillData->Range != 200) {
            std::cerr << "Atualização da habilidade não aplicada corretamente" << std::endl;
            return false;
        }
        std::cout << "Habilidade atualizada encontrada:" << std::endl;
        printSkillDetails(newSkillId, *updatedSkillData);
        
        // Salvar as alterações
        if (!loader.saveToFile(testFile)) {
            std::cerr << "Falha ao salvar habilidades modificadas para o arquivo: " << testFile << std::endl;
            return false;
        }
        std::cout << "Habilidades modificadas salvas com sucesso para o arquivo: " << testFile << std::endl;
        
        // Remover a habilidade
        if (!loader.removeSkill(newSkillId)) {
            std::cerr << "Falha ao remover habilidade" << std::endl;
            return false;
        }
        std::cout << "Habilidade removida com sucesso (ID: " << newSkillId << ")" << std::endl;
        
        // Verificar se a habilidade foi removida corretamente
        if (loader.hasSkill(newSkillId)) {
            std::cerr << "Habilidade ainda existe após remoção" << std::endl;
            return false;
        }
        std::cout << "Habilidade não encontrada após remoção (comportamento esperado)" << std::endl;
        
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro no teste de modificação de habilidades: " << e.what() << std::endl;
        return false;
    }
}

/**
 * Função principal
 */
int main(int argc, char* argv[]) {
    try {
        std::cout << "=== Teste de Compatibilidade Binária de STRUCT_SKILLDATA ===" << std::endl;
        std::cout << "Tamanho da estrutura: " << sizeof(STRUCT_SKILLDATA) << " bytes" << std::endl;
        
        // Arquivos de teste
        std::string testFile = "skill_test.bin";
        std::string outputFile = "skill_test_output.bin";
        std::string modifiedFile = "skill_test_modified.bin";
        
        // Gerar arquivo de teste
        if (!generateTestSkillFile(testFile)) {
            std::cerr << "Falha ao gerar arquivo de teste" << std::endl;
            return 1;
        }
        
        // Testar carregamento e salvamento
        if (!testLoadAndSave(testFile, outputFile)) {
            std::cerr << "Falha no teste de carregamento e salvamento" << std::endl;
            return 1;
        }
        
        // Comparar arquivos
        if (!compareFiles(testFile, outputFile)) {
            std::cerr << "Os arquivos são diferentes. Compatibilidade binária não preservada." << std::endl;
            return 1;
        }
        
        // Testar operações de modificação de habilidades
        if (!testSkillModification(modifiedFile)) {
            std::cerr << "Falha no teste de modificação de habilidades" << std::endl;
            return 1;
        }
        
        std::cout << "=== Teste Concluído com Sucesso ===" << std::endl;
        std::cout << "A estrutura STRUCT_SKILLDATA mantém compatibilidade binária perfeita." << std::endl;
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro não tratado: " << e.what() << std::endl;
        return 1;
    }
}
#ifndef WYDSTUDIO_BINARY_ANALYZER_H
#define WYDSTUDIO_BINARY_ANALYZER_H

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "../core/MultitaskingCore.h"

namespace wydstudio {
namespace reverse {

// Forward declarations
class DisassembledFunction;
class StructureCandidate;
class ControlFlowGraph;
class DataFlowAnalysis;
class BinarySection;
class MemoryBlock;
class Symbol;

/**
 * @brief Representa um bloco básico na análise de código assembly
 */
class BasicBlock {
public:
    struct Instruction {
        uint32_t address;
        std::vector<uint8_t> bytes;
        std::string mnemonic;
        std::string operands;
        std::string comment;
    };

    BasicBlock(uint32_t startAddr);
    ~BasicBlock() = default;

    uint32_t getStartAddress() const { return m_startAddress; }
    uint32_t getEndAddress() const { return m_endAddress; }
    void setEndAddress(uint32_t addr) { m_endAddress = addr; }

    void addInstruction(const Instruction& inst);
    const std::vector<Instruction>& getInstructions() const { return m_instructions; }

    void addSuccessor(std::shared_ptr<BasicBlock> block) { m_successors.push_back(block); }
    void addPredecessor(std::shared_ptr<BasicBlock> block) { m_predecessors.push_back(block); }
    
    const std::vector<std::shared_ptr<BasicBlock>>& getSuccessors() const { return m_successors; }
    const std::vector<std::shared_ptr<BasicBlock>>& getPredecessors() const { return m_predecessors; }

    bool isFunctionEntry() const { return m_isFunctionEntry; }
    void setFunctionEntry(bool value) { m_isFunctionEntry = value; }

    bool isLoopHeader() const { return m_isLoopHeader; }
    void setLoopHeader(bool value) { m_isLoopHeader = value; }

    bool hasConditionalBranch() const;
    bool hasFunctionCall() const;
    bool hasReturn() const;

private:
    uint32_t m_startAddress;
    uint32_t m_endAddress;
    std::vector<Instruction> m_instructions;
    std::vector<std::shared_ptr<BasicBlock>> m_successors;
    std::vector<std::shared_ptr<BasicBlock>> m_predecessors;
    bool m_isFunctionEntry;
    bool m_isLoopHeader;
};

/**
 * @brief Representa uma função descompilada
 */
class DisassembledFunction {
public:
    enum class CallConvention {
        UNKNOWN,
        CDECL,
        STDCALL,
        FASTCALL,
        THISCALL
    };

    struct Parameter {
        std::string name;
        std::string type;
        std::string registerName;
        int stackOffset;
    };

    struct LocalVariable {
        std::string name;
        std::string type;
        int stackOffset;
        bool isParameter;
    };

    DisassembledFunction(uint32_t entryPoint, const std::string& name = "");
    ~DisassembledFunction() = default;

    uint32_t getEntryPoint() const { return m_entryPoint; }
    std::string getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    void addBasicBlock(std::shared_ptr<BasicBlock> block);
    const std::vector<std::shared_ptr<BasicBlock>>& getBasicBlocks() const { return m_basicBlocks; }
    std::shared_ptr<BasicBlock> getEntryBlock() const;
    
    void setCallConvention(CallConvention conv) { m_callConvention = conv; }
    CallConvention getCallConvention() const { return m_callConvention; }

    void addParameter(const Parameter& param) { m_parameters.push_back(param); }
    const std::vector<Parameter>& getParameters() const { return m_parameters; }

    void addLocalVariable(const LocalVariable& var) { m_localVariables.push_back(var); }
    const std::vector<LocalVariable>& getLocalVariables() const { return m_localVariables; }

    void setDecompiledCode(const std::string& code) { m_decompiledCode = code; }
    std::string getDecompiledCode() const { return m_decompiledCode; }

    void setCFG(std::shared_ptr<ControlFlowGraph> cfg) { m_cfg = cfg; }
    std::shared_ptr<ControlFlowGraph> getCFG() const { return m_cfg; }

    void setDFA(std::shared_ptr<DataFlowAnalysis> dfa) { m_dfa = dfa; }
    std::shared_ptr<DataFlowAnalysis> getDFA() const { return m_dfa; }

    bool containsAddress(uint32_t addr) const;
    uint32_t getSize() const;
    bool hasReturnValue() const { return m_hasReturnValue; }
    void setHasReturnValue(bool value) { m_hasReturnValue = value; }

private:
    uint32_t m_entryPoint;
    std::string m_name;
    std::vector<std::shared_ptr<BasicBlock>> m_basicBlocks;
    CallConvention m_callConvention;
    std::vector<Parameter> m_parameters;
    std::vector<LocalVariable> m_localVariables;
    std::string m_decompiledCode;
    std::shared_ptr<ControlFlowGraph> m_cfg;
    std::shared_ptr<DataFlowAnalysis> m_dfa;
    bool m_hasReturnValue;
};

/**
 * @brief Candidato a estrutura de dados identificada
 */
class StructureCandidate {
public:
    struct Field {
        std::string name;
        std::string type;
        size_t offset;
        size_t size;
        std::string comment;
        float confidence; // 0.0 to 1.0
    };

    StructureCandidate(const std::string& name, size_t size);
    ~StructureCandidate() = default;

    std::string getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    size_t getSize() const { return m_size; }
    void setSize(size_t size) { m_size = size; }

    void addField(const Field& field);
    bool updateField(size_t offset, const Field& field);
    const std::vector<Field>& getFields() const { return m_fields; }

    float getConfidence() const { return m_confidence; }
    void setConfidence(float conf) { m_confidence = conf; }

    std::string generateStructDefinition(const std::string& nameSpace = "") const;
    bool hasOverlappingFields() const;
    void sortFieldsByOffset();
    std::vector<Field> getUnallocatedRegions() const;

private:
    std::string m_name;
    size_t m_size;
    std::vector<Field> m_fields;
    float m_confidence; // 0.0 to 1.0
};

/**
 * @brief Analisador de binários para engenharia reversa
 */
class BinaryAnalyzer {
public:
    enum class AnalysisLevel {
        BASIC,      // Identificação básica de funções
        STANDARD,   // Análise de funções e estruturas básicas
        ADVANCED,   // Análise detalhada e reconstrução de tipos
        FULL        // Análise completa com reconstrução de código
    };

    enum class BinaryType {
        UNKNOWN,
        PE_EXECUTABLE,  // Windows PE
        ELF_EXECUTABLE, // Linux ELF
        MACHO_EXECUTABLE, // macOS Mach-O
        WYD_BINARY,     // Formato binário específico do WYD
        DLL_LIBRARY     // Biblioteca dinâmica
    };

    struct AnalysisOptions {
        AnalysisLevel level = AnalysisLevel::STANDARD;
        bool forceAnalysis = false;
        bool recoverSymbols = true;
        bool analyzeDataStructures = true;
        bool decompileCode = false;
        bool extractStrings = true;
        uint32_t minimumFunctionSize = 5;
        float structureConfidenceThreshold = 0.6f;
    };

    BinaryAnalyzer(const std::string& filePath);
    ~BinaryAnalyzer();

    // Configuração da análise
    void setAnalysisOptions(const AnalysisOptions& options) { m_options = options; }
    const AnalysisOptions& getAnalysisOptions() const { return m_options; }

    // Operações principais
    bool loadBinary();
    bool analyze();
    
    // Informações sobre o binário
    BinaryType getBinaryType() const { return m_binaryType; }
    size_t getBinarySize() const { return m_binaryData.size(); }
    std::string getBinaryMD5() const;
    std::string getBinaryFilePath() const { return m_filePath; }
    
    // Resultados da análise
    std::vector<DisassembledFunction> getFunctions() const { return m_functions; }
    std::vector<StructureCandidate> identifyStructures() const;
    std::unordered_map<uint32_t, std::string> getSymbolTable() const { return m_symbolTable; }
    
    // Acesso a dados binários
    const std::vector<uint8_t>& getBinaryData() const { return m_binaryData; }
    std::vector<BinarySection> getSections() const;
    std::vector<std::string> getExtractedStrings() const { return m_strings; }
    
    // Exportação de dados
    void exportToJSON(const std::string& outputPath) const;
    void visualizeControlFlow(const std::string& outputPath) const;
    std::string generateStructureDefinitions(const std::string& nameSpace = "") const;
    void saveDisassembly(const std::string& outputPath) const;
    
    // Utilitários
    std::vector<uint32_t> findPatternAddresses(const std::vector<uint8_t>& pattern, 
                                               bool useWildcards = false) const;
    std::vector<std::pair<uint32_t, std::string>> findStringReferences() const;
    DisassembledFunction* getFunctionAtAddress(uint32_t addr);
    
    // Eventos de análise
    core::EventBus::SubscriptionID subscribeToAnalysisProgress(std::function<void(float)> callback);
    core::EventBus::SubscriptionID subscribeToAnalysisEvents(
        std::function<void(const std::string&, const std::unordered_map<std::string, std::any>&)> callback);

private:
    // Dados principais
    std::string m_filePath;
    std::vector<uint8_t> m_binaryData;
    BinaryType m_binaryType;
    AnalysisOptions m_options;
    
    // Resultados da análise
    std::vector<DisassembledFunction> m_functions;
    std::unordered_map<uint32_t, std::string> m_symbolTable;
    std::vector<BinarySection> m_sections;
    std::vector<std::string> m_strings;
    
    // Estado da análise
    float m_analysisProgress;
    std::string m_lastError;
    bool m_analyzed;

    // Verificadores e detectores
    bool detectBinaryType();
    bool detectCodeSegments();
    bool identifyFunctionBoundaries();
    bool recoverSymbolTable();
    bool extractStringData();
    
    // Análise avançada
    void buildControlFlowGraphs();
    void performDataFlowAnalysis();
    void identifyFunctionParameters();
    void inferDataTypes();
    void detectCallingConventions();
    void analyzeMemoryAccessPatterns();
    
    // Métodos de ajuda
    void updateAnalysisProgress(float progress);
    void fireAnalysisEvent(const std::string& eventType, 
                          const std::unordered_map<std::string, std::any>& eventData);
    
    // Event bus para notificações
    std::shared_ptr<core::EventBus> m_eventBus;
};

} // namespace reverse
} // namespace wydstudio

#endif // WYDSTUDIO_BINARY_ANALYZER_H
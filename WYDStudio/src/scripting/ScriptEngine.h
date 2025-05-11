#ifndef WYDSTUDIO_SCRIPT_ENGINE_H
#define WYDSTUDIO_SCRIPT_ENGINE_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <any>
#include <optional>
#include <variant>
#include "../core/MultitaskingCore.h"

namespace wydstudio {
namespace scripting {

// Forward declarations
class ScriptContext;
class ScriptObject;
class ScriptFunction;
class ScriptModule;
class ScriptDebugger;

/**
 * @brief Tipos de valor suportados pelo motor de script
 */
enum class ScriptValueType {
    NULL_VALUE,
    BOOLEAN,
    INTEGER,
    FLOAT,
    STRING,
    ARRAY,
    OBJECT,
    FUNCTION,
    BYTES,
    CUSTOM
};

/**
 * @brief Representa um valor no motor de script
 */
class ScriptValue {
public:
    using ArrayType = std::vector<ScriptValue>;
    using ObjectType = std::unordered_map<std::string, ScriptValue>;
    using BytesType = std::vector<uint8_t>;
    
    // Construtor padrão (valor nulo)
    ScriptValue();
    
    // Construtores de tipos específicos
    explicit ScriptValue(bool value);
    explicit ScriptValue(int32_t value);
    explicit ScriptValue(int64_t value);
    explicit ScriptValue(double value);
    explicit ScriptValue(const std::string& value);
    explicit ScriptValue(const char* value);
    explicit ScriptValue(const ArrayType& value);
    explicit ScriptValue(const ObjectType& value);
    explicit ScriptValue(const BytesType& value);
    
    // Destrutor
    ~ScriptValue();
    
    // Getters de tipo
    ScriptValueType getType() const;
    bool isNull() const;
    bool isBoolean() const;
    bool isInteger() const;
    bool isFloat() const;
    bool isNumber() const;  // int ou float
    bool isString() const;
    bool isArray() const;
    bool isObject() const;
    bool isFunction() const;
    bool isBytes() const;
    bool isCustom() const;
    
    // Getters de valor
    bool asBoolean(bool defaultValue = false) const;
    int32_t asInt32(int32_t defaultValue = 0) const;
    int64_t asInt64(int64_t defaultValue = 0) const;
    double asFloat(double defaultValue = 0.0) const;
    std::string asString(const std::string& defaultValue = "") const;
    ArrayType asArray() const;
    ObjectType asObject() const;
    BytesType asBytes() const;
    
    // Acesso a arrays
    ScriptValue operator[](size_t index) const;
    ScriptValue& operator[](size_t index);
    size_t getArrayLength() const;
    bool setArrayValue(size_t index, const ScriptValue& value);
    void pushArrayValue(const ScriptValue& value);
    ScriptValue popArrayValue();
    
    // Acesso a objetos
    ScriptValue operator[](const std::string& key) const;
    ScriptValue& operator[](const std::string& key);
    bool hasProperty(const std::string& key) const;
    std::vector<std::string> getPropertyNames() const;
    bool setProperty(const std::string& key, const ScriptValue& value);
    bool deleteProperty(const std::string& key);
    
    // Conversões
    std::string toString() const;
    
    // Operadores de comparação
    bool operator==(const ScriptValue& other) const;
    bool operator!=(const ScriptValue& other) const;
    
    // Operadores aritméticos
    ScriptValue operator+(const ScriptValue& other) const;
    ScriptValue operator-(const ScriptValue& other) const;
    ScriptValue operator*(const ScriptValue& other) const;
    ScriptValue operator/(const ScriptValue& other) const;
    
    // Custom type support
    template<typename T>
    static ScriptValue fromCustomType(const T& value) {
        ScriptValue sv;
        sv.m_type = ScriptValueType::CUSTOM;
        sv.m_value = std::make_shared<std::any>(value);
        return sv;
    }
    
    template<typename T>
    T asCustomType(const T& defaultValue = T()) const {
        if (m_type != ScriptValueType::CUSTOM || !m_value) {
            return defaultValue;
        }
        
        try {
            return std::any_cast<T>(*static_cast<std::any*>(m_value.get()));
        } catch (const std::bad_any_cast&) {
            return defaultValue;
        }
    }
    
private:
    ScriptValueType m_type;
    std::shared_ptr<void> m_value;
};

/**
 * @brief Função nativa que pode ser chamada de scripts
 */
using ScriptNativeFunction = std::function<ScriptValue(const std::vector<ScriptValue>&, ScriptContext&)>;

/**
 * @brief Contexto de execução para scripts
 */
class ScriptContext {
public:
    ScriptContext();
    ~ScriptContext();
    
    // Gerenciamento de variáveis
    void setVariable(const std::string& name, const ScriptValue& value);
    ScriptValue getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    bool deleteVariable(const std::string& name);
    std::vector<std::string> getVariableNames() const;
    
    // Gerenciamento de funções
    void setFunction(const std::string& name, const ScriptNativeFunction& func);
    ScriptNativeFunction getFunction(const std::string& name) const;
    bool hasFunction(const std::string& name) const;
    bool deleteFunction(const std::string& name);
    std::vector<std::string> getFunctionNames() const;
    
    // Gerenciamento de objetos
    void setObject(const std::string& name, const ScriptObject& obj);
    ScriptObject getObject(const std::string& name) const;
    bool hasObject(const std::string& name) const;
    bool deleteObject(const std::string& name);
    std::vector<std::string> getObjectNames() const;
    
    // Gerenciamento de módulos
    void addModule(const std::string& name, const ScriptModule& module);
    ScriptModule getModule(const std::string& name) const;
    bool hasModule(const std::string& name) const;
    bool unloadModule(const std::string& name);
    std::vector<std::string> getModuleNames() const;
    
    // Acesso ao ScriptEngine
    void setEngineReference(std::weak_ptr<class ScriptEngine> engine);
    std::shared_ptr<class ScriptEngine> getEngine() const;
    
    // Estado de execução
    void setCurrentScriptPath(const std::string& path);
    std::string getCurrentScriptPath() const;
    void setCurrentLine(uint32_t line);
    uint32_t getCurrentLine() const;
    
    // Exceções e erros
    void setLastError(const std::string& error);
    std::string getLastError() const;
    void clearError();
    bool hasError() const;
    
    // Contextos encadeados
    void setParentContext(std::shared_ptr<ScriptContext> parent);
    std::shared_ptr<ScriptContext> getParentContext() const;
    std::shared_ptr<ScriptContext> createChildContext();
    
    // Utilitários
    void clear();
    ScriptContext clone() const;
    
private:
    // Variáveis, funções e objetos
    std::unordered_map<std::string, ScriptValue> m_variables;
    std::unordered_map<std::string, ScriptNativeFunction> m_functions;
    std::unordered_map<std::string, ScriptObject> m_objects;
    std::unordered_map<std::string, ScriptModule> m_modules;
    
    // Navegação no contexto
    std::weak_ptr<ScriptContext> m_parent;
    std::weak_ptr<class ScriptEngine> m_engine;
    
    // Estado de execução
    std::string m_currentScriptPath;
    uint32_t m_currentLine;
    std::string m_lastError;
    bool m_hasError;
};

/**
 * @brief Objeto de script que encapsula dados e comportamentos
 */
class ScriptObject {
public:
    ScriptObject();
    ScriptObject(const std::string& name);
    ~ScriptObject();
    
    // Propriedades
    void setProperty(const std::string& name, const ScriptValue& value);
    ScriptValue getProperty(const std::string& name) const;
    bool hasProperty(const std::string& name) const;
    bool deleteProperty(const std::string& name);
    std::vector<std::string> getPropertyNames() const;
    
    // Métodos
    void setMethod(const std::string& name, const ScriptNativeFunction& func);
    ScriptNativeFunction getMethod(const std::string& name) const;
    bool hasMethod(const std::string& name) const;
    bool deleteMethod(const std::string& name);
    std::vector<std::string> getMethodNames() const;
    
    // Prototipagem
    void setPrototype(std::shared_ptr<ScriptObject> prototype);
    std::shared_ptr<ScriptObject> getPrototype() const;
    
    // Metadata
    void setName(const std::string& name);
    std::string getName() const;
    void setClassType(const std::string& classType);
    std::string getClassType() const;
    
    // Conversão
    ScriptValue toScriptValue() const;
    static ScriptObject fromScriptValue(const ScriptValue& value);
    
private:
    std::string m_name;
    std::string m_classType;
    std::unordered_map<std::string, ScriptValue> m_properties;
    std::unordered_map<std::string, ScriptNativeFunction> m_methods;
    std::shared_ptr<ScriptObject> m_prototype;
};

/**
 * @brief Módulo de script que encapsula funcionalidades
 */
class ScriptModule {
public:
    ScriptModule();
    ScriptModule(const std::string& name);
    ~ScriptModule();
    
    // Propriedades
    void setExport(const std::string& name, const ScriptValue& value);
    ScriptValue getExport(const std::string& name) const;
    bool hasExport(const std::string& name) const;
    bool deleteExport(const std::string& name);
    std::vector<std::string> getExportNames() const;
    
    // Metadados
    void setName(const std::string& name);
    std::string getName() const;
    void setVersion(const std::string& version);
    std::string getVersion() const;
    void setDescription(const std::string& description);
    std::string getDescription() const;
    void setAuthor(const std::string& author);
    std::string getAuthor() const;
    
    // Dependências
    void addDependency(const std::string& moduleName);
    std::vector<std::string> getDependencies() const;
    void clearDependencies();
    
    // Origem
    void setSourcePath(const std::string& path);
    std::string getSourcePath() const;
    void setSourceCode(const std::string& code);
    std::string getSourceCode() const;
    
    // Conversão
    ScriptValue toScriptValue() const;
    
private:
    std::string m_name;
    std::string m_version;
    std::string m_description;
    std::string m_author;
    std::unordered_map<std::string, ScriptValue> m_exports;
    std::vector<std::string> m_dependencies;
    std::string m_sourcePath;
    std::string m_sourceCode;
};

/**
 * @brief Eventos emitidos pelo motor de script
 */
struct ScriptEvent {
    enum class Type {
        SCRIPT_LOADED,
        SCRIPT_UNLOADED,
        SCRIPT_EXECUTED,
        SCRIPT_ERROR,
        FUNCTION_CALLED,
        GLOBAL_CHANGED,
        BREAKPOINT_HIT,
        STEP_COMPLETED,
        EXECUTION_PAUSED,
        EXECUTION_RESUMED,
        EXECUTION_STOPPED
    };
    
    Type type;
    std::string scriptPath;
    std::string functionName;
    uint32_t lineNumber;
    std::string message;
    ScriptValue data;
    
    ScriptEvent(Type eventType) : type(eventType), lineNumber(0) {}
};

/**
 * @brief Opções para compilação e execução de scripts
 */
struct ScriptOptions {
    enum class OptimizationLevel {
        NONE,
        BASIC,
        NORMAL,
        AGGRESSIVE
    };
    
    bool strictMode;
    bool allowEval;
    bool enableDebugInfo;
    bool treatWarningsAsErrors;
    bool allowFileSystem;
    bool allowNetwork;
    bool allowSystemAccess;
    bool allowDynamicImport;
    OptimizationLevel optimization;
    
    uint32_t maxExecutionTime;  // em ms, 0 = sem limite
    uint32_t maxMemoryUsage;    // em MB, 0 = sem limite
    uint32_t maxStackDepth;     // 0 = padrão
    
    std::vector<std::string> modulePaths;  // Caminhos para procurar módulos
    std::unordered_map<std::string, std::string> defines;  // Constantes para pré-processamento
    
    ScriptOptions() 
        : strictMode(true), allowEval(false), enableDebugInfo(true),
          treatWarningsAsErrors(false), allowFileSystem(true),
          allowNetwork(false), allowSystemAccess(false),
          allowDynamicImport(true), optimization(OptimizationLevel::NORMAL),
          maxExecutionTime(10000), maxMemoryUsage(100), maxStackDepth(1000) {}
};

/**
 * @brief Resultado da compilação ou execução de um script
 */
struct ScriptResult {
    enum class Status {
        SUCCESS,
        SYNTAX_ERROR,
        TYPE_ERROR,
        REFERENCE_ERROR,
        RANGE_ERROR,
        TIMEOUT_ERROR,
        MEMORY_ERROR,
        RUNTIME_ERROR,
        IMPORT_ERROR,
        INTERNAL_ERROR,
        PERMISSION_ERROR
    };
    
    Status status;
    std::string errorMessage;
    std::string errorType;
    std::string errorFileName;
    uint32_t errorLineNumber;
    uint32_t errorColumnNumber;
    
    ScriptValue returnValue;
    
    // Métricas de execução
    uint32_t executionTimeMs;
    uint32_t memoryUsageKB;
    uint32_t maxStackDepth;
    
    // Avisos
    std::vector<std::string> warnings;
    
    ScriptResult() : status(Status::SUCCESS), errorLineNumber(0), 
                   errorColumnNumber(0), executionTimeMs(0),
                   memoryUsageKB(0), maxStackDepth(0) {}
                   
    bool isSuccess() const { return status == Status::SUCCESS; }
    bool hasError() const { return status != Status::SUCCESS; }
    
    static ScriptResult createSuccess(const ScriptValue& value) {
        ScriptResult result;
        result.returnValue = value;
        return result;
    }
    
    static ScriptResult createError(Status status, const std::string& message) {
        ScriptResult result;
        result.status = status;
        result.errorMessage = message;
        return result;
    }
};

/**
 * @brief Ponto de interrupção para depuração
 */
struct Breakpoint {
    std::string scriptPath;
    uint32_t lineNumber;
    std::string condition;  // Expressão de condição, vazia = sempre parar
    uint32_t hitCount;      // Número de vezes que o breakpoint foi atingido
    bool enabled;
    
    Breakpoint() : lineNumber(0), hitCount(0), enabled(true) {}
    Breakpoint(const std::string& path, uint32_t line)
        : scriptPath(path), lineNumber(line), hitCount(0), enabled(true) {}
};

/**
 * @brief Depurador para scripts
 */
class ScriptDebugger {
public:
    enum class StepMode {
        NONE,
        INTO,
        OVER,
        OUT
    };
    
    ScriptDebugger();
    ~ScriptDebugger();
    
    // Controle de depuração
    void attach(std::shared_ptr<ScriptEngine> engine);
    void detach();
    bool isAttached() const;
    
    // Breakpoints
    uint32_t addBreakpoint(const std::string& scriptPath, uint32_t line);
    uint32_t addBreakpoint(const std::string& scriptPath, uint32_t line, const std::string& condition);
    bool removeBreakpoint(uint32_t id);
    bool enableBreakpoint(uint32_t id, bool enable = true);
    bool hasBreakpointAt(const std::string& scriptPath, uint32_t line) const;
    std::vector<Breakpoint> getAllBreakpoints() const;
    void clearAllBreakpoints();
    
    // Execução
    void pause();
    void resume();
    void stop();
    void step(StepMode mode = StepMode::INTO);
    bool isPaused() const;
    
    // Inspeção
    ScriptValue evaluate(const std::string& expression);
    std::vector<std::pair<std::string, ScriptValue>> getLocalVariables() const;
    std::vector<std::pair<std::string, ScriptValue>> getGlobalVariables() const;
    std::vector<std::string> getCallStack() const;
    
    // Eventos
    using DebugEventHandler = std::function<void(const ScriptEvent&)>;
    core::EventBus::SubscriptionID subscribeToDebugEvents(DebugEventHandler handler);
    bool unsubscribeFromDebugEvents(core::EventBus::SubscriptionID id);
    
private:
    std::weak_ptr<ScriptEngine> m_engine;
    bool m_attached;
    bool m_paused;
    StepMode m_stepMode;
    
    uint32_t m_nextBreakpointId;
    std::unordered_map<uint32_t, Breakpoint> m_breakpoints;
    
    std::shared_ptr<core::EventBus> m_eventBus;
};

/**
 * @brief Motor de script principal
 */
class ScriptEngine : public std::enable_shared_from_this<ScriptEngine> {
public:
    // Construtor e destrutor
    ScriptEngine();
    ~ScriptEngine();
    
    // Inicialização
    bool initialize();
    bool shutdown();
    bool isInitialized() const;
    
    // Gerenciamento de scripts
    ScriptResult loadScript(const std::string& scriptPath);
    ScriptResult loadScriptFromString(const std::string& scriptCode, const std::string& sourceName = "<string>");
    bool unloadScript(const std::string& scriptPath);
    bool isScriptLoaded(const std::string& scriptPath) const;
    std::vector<std::string> getLoadedScripts() const;
    
    // Execução de scripts
    ScriptResult executeScript(const std::string& scriptPath, const ScriptOptions& options = ScriptOptions());
    ScriptResult executeScriptFromString(const std::string& scriptCode, 
                                      const std::string& sourceName = "<string>",
                                      const ScriptOptions& options = ScriptOptions());
    
    ScriptResult evaluateExpression(const std::string& expression);
    
    // Chamada de funções
    ScriptResult callFunction(const std::string& functionName);
    ScriptResult callFunction(const std::string& functionName, const std::vector<ScriptValue>& args);
    ScriptResult callFunction(const std::string& functionName, std::initializer_list<ScriptValue> args);
    
    // Gerenciamento do contexto global
    ScriptContext& getGlobalContext();
    const ScriptContext& getGlobalContext() const;
    
    // Registro de funções e objetos
    bool registerFunction(const std::string& name, const ScriptNativeFunction& function);
    bool registerObject(const std::string& name, const ScriptObject& object);
    
    // Módulos
    bool loadModule(const std::string& modulePath);
    bool registerModule(const std::string& name, const ScriptModule& module);
    ScriptModule getModule(const std::string& name) const;
    bool unloadModule(const std::string& name);
    std::vector<std::string> getLoadedModules() const;
    
    // Gerenciamento de erros
    std::string getLastError() const;
    void clearError();
    
    // Depuração
    std::shared_ptr<ScriptDebugger> getDebugger();
    bool enableDebugger(bool enable = true);
    bool isDebuggerEnabled() const;
    
    // Eventos
    core::EventBus::SubscriptionID subscribeToScriptEvents(
        std::function<void(const ScriptEvent&)> handler);
    bool unsubscribeFromScriptEvents(core::EventBus::SubscriptionID id);
    
    // Configuração
    void setOption(const std::string& name, const ScriptValue& value);
    ScriptValue getOption(const std::string& name) const;
    void setDefaultOptions(const ScriptOptions& options);
    ScriptOptions getDefaultOptions() const;
    
private:
    bool m_initialized;
    std::shared_ptr<ScriptContext> m_globalContext;
    std::shared_ptr<ScriptDebugger> m_debugger;
    bool m_debuggerEnabled;
    
    // Configurações
    ScriptOptions m_defaultOptions;
    std::unordered_map<std::string, ScriptValue> m_options;
    
    // Scripts carregados
    struct LoadedScript {
        std::string path;
        std::string code;
        std::chrono::system_clock::time_point loadTime;
        std::vector<std::string> dependencies;
    };
    std::unordered_map<std::string, LoadedScript> m_loadedScripts;
    
    // Módulos
    std::unordered_map<std::string, ScriptModule> m_modules;
    
    // Eventos
    std::shared_ptr<core::EventBus> m_eventBus;
    
    // Último erro
    std::string m_lastError;
    
    // Métodos auxiliares
    void fireScriptEvent(ScriptEvent::Type type, 
                        const std::string& scriptPath = "",
                        const std::string& message = "",
                        const ScriptValue& data = ScriptValue());
    
    bool resolveModulePath(const std::string& requestedModule, 
                          const std::string& currentScript,
                          std::string& resolvedPath);
                          
    ScriptResult compileScript(const std::string& code, 
                             const std::string& sourceName,
                             const ScriptOptions& options);
};

/**
 * @brief Gerenciador de motores de script
 */
class ScriptManager {
public:
    // Singleton
    static ScriptManager& getInstance();
    
    // Inicialização
    bool initialize();
    bool shutdown();
    bool isInitialized() const;
    
    // Gerenciamento de motores
    std::shared_ptr<ScriptEngine> createEngine(const std::string& name);
    std::shared_ptr<ScriptEngine> getEngine(const std::string& name);
    bool destroyEngine(const std::string& name);
    std::vector<std::string> getEngineNames() const;
    
    // Motor padrão
    std::shared_ptr<ScriptEngine> getDefaultEngine();
    void setDefaultEngine(const std::string& name);
    
    // Biblioteca de utilidades
    struct ScriptUtility {
        std::string name;
        std::string description;
        ScriptModule module;
    };
    
    bool registerUtility(const ScriptUtility& utility);
    ScriptUtility getUtility(const std::string& name) const;
    bool hasUtility(const std::string& name) const;
    std::vector<std::string> getUtilityNames() const;
    
    // Registro de tipos personalizados
    template<typename T>
    bool registerCustomType(const std::string& typeName, 
                         std::function<ScriptValue(const T&)> toScriptValue,
                         std::function<T(const ScriptValue&)> fromScriptValue) {
        if (m_customTypes.find(typeName) != m_customTypes.end()) {
            return false; // Já existe
        }
        
        struct TypeHandlers {
            std::function<ScriptValue(const std::any&)> toScript;
            std::function<std::any(const ScriptValue&)> fromScript;
        };
        
        TypeHandlers handlers;
        handlers.toScript = [toScriptValue](const std::any& value) -> ScriptValue {
            try {
                const T& typedValue = std::any_cast<const T&>(value);
                return toScriptValue(typedValue);
            } catch (const std::bad_any_cast&) {
                return ScriptValue(); // Valor nulo em caso de erro
            }
        };
        
        handlers.fromScript = [fromScriptValue](const ScriptValue& value) -> std::any {
            return fromScriptValue(value);
        };
        
        m_customTypes[typeName] = handlers;
        return true;
    }
    
    ScriptValue convertToScriptValue(const std::string& typeName, const std::any& value);
    std::any convertFromScriptValue(const std::string& typeName, const ScriptValue& value);
    bool isCustomTypeRegistered(const std::string& typeName) const;
    
    // Gerenciamento de caminhos
    void addModulePath(const std::string& path);
    std::vector<std::string> getModulePaths() const;
    void clearModulePaths();
    
private:
    ScriptManager();
    ~ScriptManager();
    
    // Prevenir cópia
    ScriptManager(const ScriptManager&) = delete;
    ScriptManager& operator=(const ScriptManager&) = delete;
    
    bool m_initialized;
    std::unordered_map<std::string, std::shared_ptr<ScriptEngine>> m_engines;
    std::string m_defaultEngineName;
    
    // Utilitários
    std::unordered_map<std::string, ScriptUtility> m_utilities;
    
    // Tipos personalizados
    struct TypeHandlers {
        std::function<ScriptValue(const std::any&)> toScript;
        std::function<std::any(const ScriptValue&)> fromScript;
    };
    std::unordered_map<std::string, TypeHandlers> m_customTypes;
    
    // Caminhos de módulos
    std::vector<std::string> m_modulePaths;
    
    // Inicialização de utilitários padrão
    void initializeStandardUtilities();
};

} // namespace scripting
} // namespace wydstudio

#endif // WYDSTUDIO_SCRIPT_ENGINE_H
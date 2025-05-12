#ifndef WYDSTUDIO_PROTOCOL_ANALYZER_H
#define WYDSTUDIO_PROTOCOL_ANALYZER_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <optional>
#include <bitset>
#include <chrono>
#include <array>
#include "../core/MultitaskingCore.h"

namespace wydstudio {
namespace network {

/**
 * @brief Direção de um pacote na comunicação
 */
enum class PacketDirection {
    CLIENT_TO_SERVER,
    SERVER_TO_CLIENT,
    UNKNOWN
};

/**
 * @brief Estado de validação de um pacote
 */
enum class ValidationState {
    VALID,
    INVALID_HEADER,
    INVALID_SIZE,
    INVALID_CHECKSUM,
    MALFORMED_PAYLOAD,
    UNKNOWN_PACKET_TYPE,
    INCOMPLETE_PACKET,
    MISSING_REQUIRED_FIELD,
    FIELD_TYPE_MISMATCH,
    FIELD_VALUE_OUT_OF_RANGE,
    UNEXPECTED_DATA,
    INVALID_STATE_TRANSITION,
    ENCRYPTION_ERROR,
    CUSTOM_VALIDATION_FAILED
};

/**
 * @brief Tipo de campo num pacote
 */
enum class FieldType {
    BYTE,           // Unsigned 8-bit
    CHAR,           // Signed 8-bit
    SHORT,          // Signed 16-bit
    USHORT,         // Unsigned 16-bit
    INT,            // Signed 32-bit
    UINT,           // Unsigned 32-bit
    INT64,          // Signed 64-bit
    UINT64,         // Unsigned 64-bit
    FLOAT,          // 32-bit floating point
    DOUBLE,         // 64-bit floating point
    STRING,         // Null-terminated string
    BYTE_ARRAY,     // Array of bytes
    POSITION_2D,    // 2D coordinates (x, y)
    POSITION_3D,    // 3D coordinates (x, y, z)
    BOOLEAN,        // Boolean value
    FLAGS,          // Bitfield
    ENUM,           // Enumeration
    TIMESTAMP,      // Timestamp
    CUSTOM          // Custom data type
};

/**
 * @brief Definição de um campo em um pacote
 */
struct FieldDefinition {
    std::string name;
    FieldType type;
    uint32_t offset;            // Offset em bytes dentro do pacote
    uint32_t size;              // Tamanho em bytes
    bool isArray;               // Se é um array
    uint32_t arraySize;         // Tamanho do array (se aplicável)
    uint32_t arraySizeFieldOffset; // Offset do campo que contém o tamanho do array
    std::string comment;        // Descrição do campo
    bool isRequired;            // Se o campo é obrigatório
    
    // Valores de referência para validação
    std::optional<int64_t> minValue;
    std::optional<int64_t> maxValue;
    std::vector<int64_t> validValues;
    
    // Informações específicas de tipo
    std::unordered_map<std::string, std::string> typeInfo;
    
    FieldDefinition() : type(FieldType::BYTE), offset(0), size(1),
                      isArray(false), arraySize(0), arraySizeFieldOffset(0),
                      isRequired(true) {}
};

/**
 * @brief Definição de um pacote
 */
struct PacketDefinition {
    uint16_t id;                      // ID do pacote
    std::string name;                 // Nome do pacote
    std::vector<FieldDefinition> fields; // Campos do pacote
    PacketDirection direction;        // Direção do pacote
    uint32_t minSize;                 // Tamanho mínimo
    uint32_t maxSize;                 // Tamanho máximo (0 = sem limite)
    bool hasChecksum;                 // Se o pacote tem checksum
    uint32_t checksumOffset;          // Offset do checksum
    std::string description;          // Descrição do pacote
    bool isEncrypted;                 // Se o pacote é encriptado
    
    // Informações de estado
    struct StateInfo {
        std::string requiredClientState;  // Estado em que o cliente deve estar
        std::string requiredServerState;  // Estado em que o servidor deve estar
        std::string resultingClientState; // Estado resultante do cliente
        std::string resultingServerState; // Estado resultante do servidor
    } stateInfo;
    
    // Validação customizada
    std::string validationRule;       // Regra de validação (para lógica complexa)
    
    // Metadados
    std::unordered_map<std::string, std::string> metadata;
    
    PacketDefinition() : id(0), direction(PacketDirection::UNKNOWN),
                       minSize(0), maxSize(0), hasChecksum(false),
                       checksumOffset(0), isEncrypted(false) {}
    
    // Métodos utilitários
    const FieldDefinition* getFieldByName(const std::string& fieldName) const;
    const FieldDefinition* getFieldByOffset(uint32_t fieldOffset) const;
    uint32_t calculateTotalSize() const;
    bool hasField(const std::string& fieldName) const;
};

/**
 * @brief Valor de um campo extraído de um pacote
 */
struct FieldValue {
    using ValueType = std::variant<
        uint8_t,              // BYTE
        int8_t,               // CHAR
        int16_t,              // SHORT
        uint16_t,             // USHORT
        int32_t,              // INT
        uint32_t,             // UINT
        int64_t,              // INT64
        uint64_t,             // UINT64
        float,                // FLOAT
        double,               // DOUBLE
        std::string,          // STRING
        std::vector<uint8_t>, // BYTE_ARRAY
        std::pair<int32_t, int32_t>,   // POSITION_2D
        std::tuple<int32_t, int32_t, int32_t>, // POSITION_3D
        bool,                 // BOOLEAN
        std::bitset<32>,      // FLAGS
        uint32_t,             // ENUM
        std::chrono::system_clock::time_point, // TIMESTAMP
        std::any              // CUSTOM
    >;
    
    std::string name;
    FieldType type;
    ValueType value;
    bool isValid;
    std::string validationError;
    
    // Métodos auxiliares para acessar valores
    template<typename T>
    T getValue(const T& defaultValue = T()) const {
        try {
            return std::get<T>(value);
        } catch (const std::bad_variant_access&) {
            return defaultValue;
        }
    }
    
    std::string toString() const;
    
    static FieldValue createByte(const std::string& name, uint8_t value);
    static FieldValue createChar(const std::string& name, int8_t value);
    static FieldValue createShort(const std::string& name, int16_t value);
    static FieldValue createUShort(const std::string& name, uint16_t value);
    static FieldValue createInt(const std::string& name, int32_t value);
    static FieldValue createUInt(const std::string& name, uint32_t value);
    static FieldValue createInt64(const std::string& name, int64_t value);
    static FieldValue createUInt64(const std::string& name, uint64_t value);
    static FieldValue createFloat(const std::string& name, float value);
    static FieldValue createDouble(const std::string& name, double value);
    static FieldValue createString(const std::string& name, const std::string& value);
    static FieldValue createByteArray(const std::string& name, const std::vector<uint8_t>& value);
    static FieldValue createPosition2D(const std::string& name, int32_t x, int32_t y);
    static FieldValue createPosition3D(const std::string& name, int32_t x, int32_t y, int32_t z);
    static FieldValue createBoolean(const std::string& name, bool value);
    static FieldValue createFlags(const std::string& name, uint32_t value);
    static FieldValue createEnum(const std::string& name, uint32_t value);
    static FieldValue createTimestamp(const std::string& name, std::chrono::system_clock::time_point value);
    
    template<typename T>
    static FieldValue createCustom(const std::string& name, const T& value) {
        FieldValue field;
        field.name = name;
        field.type = FieldType::CUSTOM;
        field.value = std::make_any<T>(value);
        field.isValid = true;
        return field;
    }
};

/**
 * @brief Pacote decodificado com seus campos
 */
struct DecodedPacket {
    uint16_t id;
    std::string name;
    PacketDirection direction;
    std::chrono::system_clock::time_point timestamp;
    std::vector<FieldValue> fields;
    std::vector<uint8_t> rawData;
    ValidationState validationState;
    std::string validationMessage;
    bool isEncrypted;
    uint32_t sequenceNumber; // Número de sequência no stream
    
    DecodedPacket() : id(0), direction(PacketDirection::UNKNOWN),
                    validationState(ValidationState::VALID), 
                    isEncrypted(false), sequenceNumber(0) {}
    
    // Métodos auxiliares
    const FieldValue* getFieldByName(const std::string& fieldName) const;
    bool hasField(const std::string& fieldName) const;
    
    template<typename T>
    std::optional<T> getFieldValue(const std::string& fieldName) const {
        const FieldValue* field = getFieldByName(fieldName);
        if (field) {
            try {
                return std::get<T>(field->value);
            } catch (const std::bad_variant_access&) {
                return std::nullopt;
            }
        }
        return std::nullopt;
    }
    
    std::string toString() const;
};

/**
 * @brief Captura de pacote de rede
 */
struct PacketCapture {
    std::string source;
    std::string destination;
    std::chrono::system_clock::time_point timestamp;
    std::vector<uint8_t> data;
    PacketDirection direction;
    uint32_t captureId;
    std::string sessionId;
    
    PacketCapture() : direction(PacketDirection::UNKNOWN), captureId(0) {}
};

/**
 * @brief Template inferido para um tipo de pacote
 */
struct PacketTemplate {
    uint16_t id;
    std::string name;
    std::vector<FieldDefinition> fields;
    PacketDirection direction;
    float confidence; // 0.0 a 1.0
    uint32_t samplesAnalyzed;
    std::string description;
    
    PacketTemplate() : id(0), direction(PacketDirection::UNKNOWN),
                     confidence(0.0f), samplesAnalyzed(0) {}
};

/**
 * @brief Estatísticas de uso de protocolo
 */
struct ProtocolStatistics {
    uint32_t totalPacketsAnalyzed;
    uint32_t validPackets;
    uint32_t invalidPackets;
    std::unordered_map<uint16_t, uint32_t> packetTypeDistribution;
    std::unordered_map<ValidationState, uint32_t> validationErrorDistribution;
    uint32_t encryptedPackets;
    uint32_t unknownPackets;
    
    struct PacketTypeStats {
        uint32_t count;
        uint32_t minSize;
        uint32_t maxSize;
        double averageSize;
        uint32_t validCount;
        uint32_t invalidCount;
    };
    
    std::unordered_map<uint16_t, PacketTypeStats> detailedStats;
    
    ProtocolStatistics() : totalPacketsAnalyzed(0), validPackets(0),
                         invalidPackets(0), encryptedPackets(0),
                         unknownPackets(0) {}
};

/**
 * @brief Candidato a campo em um pacote detectado
 */
struct FieldCandidate {
    uint32_t offset;
    FieldType probableType;
    uint32_t size;
    float confidence; // 0.0 a 1.0
    std::vector<FieldType> possibleTypes;
    std::string description;
    
    // Estatísticas do campo
    uint32_t uniqueValues;
    bool isConstant;
    std::vector<uint8_t> commonValue;
    double entropy;
    
    FieldCandidate() : offset(0), probableType(FieldType::BYTE), size(1),
                     confidence(0.0f), uniqueValues(0), isConstant(false),
                     entropy(0.0) {}
};

/**
 * @brief Opções para análise de protocolo
 */
struct ProtocolAnalysisOptions {
    bool detectFieldTypes;
    bool inferPacketStructures;
    bool analyzePacketSequences;
    bool detectEncryption;
    bool validatePackets;
    bool generateCodeStructures;
    bool enableDeepAnalysis;
    
    uint32_t minSamplesForInference;
    float fieldConfidenceThreshold;
    float packetConfidenceThreshold;
    uint32_t maxFieldsPerPacket;
    
    bool ignoreUnknownPackets;
    bool ignoreInvalidPackets;
    
    ProtocolAnalysisOptions() : detectFieldTypes(true), inferPacketStructures(true),
                              analyzePacketSequences(true), detectEncryption(true),
                              validatePackets(true), generateCodeStructures(true),
                              enableDeepAnalysis(false), minSamplesForInference(5),
                              fieldConfidenceThreshold(0.7f), packetConfidenceThreshold(0.8f),
                              maxFieldsPerPacket(50), ignoreUnknownPackets(false),
                              ignoreInvalidPackets(false) {}
};

/**
 * @brief Resultado da análise de protocolo
 */
struct ProtocolAnalysisResult {
    std::vector<PacketTemplate> inferredPackets;
    std::vector<std::pair<uint16_t, uint16_t>> packetSequences;
    ProtocolStatistics statistics;
    bool encryptionDetected;
    std::string encryptionType;
    std::vector<std::string> headerInformation;
    std::string protocolDescription;
    
    // Código gerado
    std::string generatedStructures;
    std::string generatedParsingCode;
    std::string generatedValidationCode;
    
    ProtocolAnalysisResult() : encryptionDetected(false) {}
};

/**
 * @brief Analisador de protocolo de rede
 */
class ProtocolAnalyzer {
public:
    ProtocolAnalyzer();
    ~ProtocolAnalyzer();
    
    // Carregamento de definições
    bool loadPacketDefinitions(const std::string& filePath);
    bool registerPacketDefinition(const PacketDefinition& definition);
    bool updatePacketDefinition(const PacketDefinition& definition);
    bool removePacketDefinition(uint16_t packetId);
    
    // Acesso a definições
    PacketDefinition getPacketDefinition(uint16_t packetId) const;
    std::vector<PacketDefinition> getAllPacketDefinitions() const;
    bool hasPacketDefinition(uint16_t packetId) const;
    
    // Captura e processamento de pacotes
    void addPacketCapture(const PacketCapture& capture);
    void addRawPacket(const std::vector<uint8_t>& data, 
                     PacketDirection direction,
                     const std::string& sessionId = "");
    
    DecodedPacket decodePacket(const std::vector<uint8_t>& data, 
                              PacketDirection direction = PacketDirection::UNKNOWN);
    
    ValidationState validatePacket(const DecodedPacket& packet);
    
    // Análise de protocolo
    void analyzeCaptures();
    ProtocolAnalysisResult getAnalysisResult() const;
    void setAnalysisOptions(const ProtocolAnalysisOptions& options);
    ProtocolAnalysisOptions getAnalysisOptions() const;
    
    // Inferência de estruturas
    std::vector<PacketTemplate> getInferredPacketTemplates() const;
    std::vector<FieldCandidate> getFieldCandidates(uint16_t packetId) const;
    
    // Exportação
    void exportProtocolSpecification(const std::string& outputPath) const;
    void generatePacketStructures(const std::string& outputPath, 
                                const std::string& language = "cpp") const;
    
    // Estatísticas
    ProtocolStatistics getStatistics() const;
    
    // Eventos
    core::EventBus::SubscriptionID subscribeToPacketEvents(std::function<void(const DecodedPacket&)> callback);
    core::EventBus::SubscriptionID subscribeToAnalysisProgress(std::function<void(float)> callback);
    
private:
    std::unordered_map<uint16_t, PacketDefinition> m_packetDefinitions;
    std::vector<PacketCapture> m_captures;
    std::vector<DecodedPacket> m_decodedPackets;
    
    // Campos inferidos e templates
    std::unordered_map<uint16_t, std::vector<FieldCandidate>> m_fieldCandidates;
    std::vector<PacketTemplate> m_inferredTemplates;
    
    // Análise de sequência
    std::unordered_map<uint16_t, std::unordered_map<uint16_t, uint32_t>> m_packetSequences;
    
    // Estatísticas
    ProtocolStatistics m_statistics;
    
    // Opções de análise
    ProtocolAnalysisOptions m_options;
    
    // Resultado de análise
    ProtocolAnalysisResult m_analysisResult;
    
    // Event bus para notificações
    std::shared_ptr<core::EventBus> m_eventBus;
    
    // Métodos de análise interna
    void analyzePacket(const DecodedPacket& packet);
    void updateStatistics(const DecodedPacket& packet);
    void correlatePackets();
    void identifyVariableFields();
    void inferDataTypes();
    void detectEncryption();
    void generateResults();
    
    // Métodos auxiliares
    uint16_t extractPacketId(const std::vector<uint8_t>& data);
    void updateAnalysisProgress(float progress);
    std::string generateStructureDefinition(const PacketTemplate& tmpl, const std::string& language) const;
    
    // Detectores de tipo de campo
    bool isStringField(const std::vector<uint8_t>& data, uint32_t offset, uint32_t& length);
    bool isNumericField(const std::vector<uint8_t>& data, uint32_t offset, FieldType& detectedType);
    bool isPositionField(const std::vector<uint8_t>& data, uint32_t offset, bool& is3D);
    double calculateFieldEntropy(const std::vector<std::vector<uint8_t>>& samplesData, uint32_t offset, uint32_t size);
};

} // namespace network
} // namespace wydstudio

#endif // WYDSTUDIO_PROTOCOL_ANALYZER_H
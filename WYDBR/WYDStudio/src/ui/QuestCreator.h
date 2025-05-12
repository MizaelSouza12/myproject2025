#ifndef WYDSTUDIO_QUEST_CREATOR_H
#define WYDSTUDIO_QUEST_CREATOR_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <map>
#include <variant>
#include <nlohmann/json.hpp>
#include "../core/WYDStudioFacade.h"

namespace wydstudio {
namespace ui {

using json = nlohmann::json;

/**
 * @brief Tipo de nó no fluxograma de quest
 */
enum class QuestNodeType {
    START,          // Ponto de início
    DIALOG,         // Diálogo com NPC
    ITEM_CHECK,     // Verifica se jogador tem item
    ITEM_COLLECT,   // Jogador precisa coletar item
    ITEM_DELIVER,   // Jogador entrega item ao NPC
    KILL_MOBS,      // Jogador precisa matar monstros
    LOCATION_VISIT, // Jogador precisa visitar local
    LEVEL_CHECK,    // Verifica nível do jogador
    SKILL_CHECK,    // Verifica habilidade do jogador
    GOLD_CHECK,     // Verifica dinheiro do jogador
    GOLD_REWARD,    // Recompensa em dinheiro
    ITEM_REWARD,    // Recompensa em item
    EXP_REWARD,     // Recompensa em experiência
    SKILL_REWARD,   // Recompensa em habilidade
    CHOICE,         // Jogador faz uma escolha
    WAIT_TIME,      // Espera por um período
    REPUTATION,     // Afeta reputação com facção
    CONDITION,      // Condição personalizada
    ACTION,         // Ação personalizada
    END_SUCCESS,    // Fim com sucesso
    END_FAILURE     // Fim com falha
};

/**
 * @brief Tipo de conexão entre nós
 */
enum class QuestConnectionType {
    DEFAULT,   // Conexão padrão
    SUCCESS,   // Condição atendida
    FAILURE,   // Condição não atendida
    CHOICE_1,  // Primeira escolha
    CHOICE_2,  // Segunda escolha
    CHOICE_3,  // Terceira escolha
    CHOICE_4,  // Quarta escolha
    CUSTOM     // Conexão customizada
};

/**
 * @brief Requisito para iniciar quest
 */
enum class QuestRequirementType {
    LEVEL,          // Nível mínimo
    QUEST_COMPLETED, // Outra quest completa
    ITEM_OWNED,     // Possuir item
    CLASS,          // Classe específica
    SKILL_LEVEL,    // Nível de habilidade
    REPUTATION,     // Reputação com facção
    GOLD,           // Quantidade de gold
    GUILD_MEMBER,   // Ser membro de guilda
    CUSTOM          // Requisito personalizado
};

/**
 * @brief Requisito específico para quest
 */
struct QuestRequirement {
    QuestRequirementType type;
    std::string name;
    std::string description;
    
    // Dados específicos do requisito
    union {
        int32_t levelRequired;
        int32_t questId;
        int32_t itemId;
        int32_t classId;
        int32_t skillId;
        int32_t reputationAmount;
        int64_t goldAmount;
    } data;
    
    // Dados adicionais para requisitos custom
    std::map<std::string, std::string> customData;
};

/**
 * @brief Recompensa de quest
 */
struct QuestReward {
    enum class Type {
        ITEM,       // Item como recompensa
        GOLD,       // Dinheiro como recompensa
        EXPERIENCE, // Experiência como recompensa
        SKILL,      // Habilidade como recompensa
        REPUTATION, // Reputação como recompensa
        TITLE,      // Título como recompensa
        CUSTOM      // Recompensa personalizada
    };
    
    Type type;
    std::string name;
    std::string description;
    
    // Dados específicos da recompensa
    struct {
        std::variant<
            int32_t,     // itemId, skillId, titleId
            int64_t,     // goldAmount, expAmount
            std::string  // customData
        > value;
        
        int32_t quantity; // Para itens
    } data;
};

/**
 * @brief Nó no fluxograma de quest
 */
struct QuestNode {
    int32_t id;
    QuestNodeType type;
    std::string title;
    std::string description;
    
    // Posição no fluxograma visual (para UI)
    int32_t x, y;
    
    // Dados específicos do tipo de nó
    struct NodeData {
        // Para diálogo
        struct {
            std::string npcName;
            std::string dialogText;
            std::vector<std::string> dialogOptions;
        } dialog;
        
        // Para verificação de item
        struct {
            int32_t itemId;
            int32_t quantity;
            bool removeItem;
        } itemCheck;
        
        // Para morte de monstros
        struct {
            int32_t mobId;
            int32_t quantity;
            bool specificMob; // Se false, qualquer mob do tipo conta
        } killMobs;
        
        // Para visita a local
        struct {
            int32_t mapId;
            int32_t x, y;
            int32_t radius;
        } location;
        
        // Para verificação de nível
        struct {
            int32_t requiredLevel;
        } levelCheck;
        
        // Para verificação de habilidade
        struct {
            int32_t skillId;
            int32_t requiredLevel;
        } skillCheck;
        
        // Para verificação/recompensa de dinheiro
        struct {
            int64_t amount;
        } gold;
        
        // Para recompensa de item
        struct {
            int32_t itemId;
            int32_t quantity;
        } itemReward;
        
        // Para recompensa de experiência
        struct {
            int64_t amount;
        } expReward;
        
        // Para recompensa de habilidade
        struct {
            int32_t skillId;
            int32_t level;
        } skillReward;
        
        // Para escolha
        struct {
            std::vector<std::string> options;
        } choice;
        
        // Para espera de tempo
        struct {
            int32_t seconds;
            bool realTime; // Se true, tempo real; se false, tempo de jogo
        } waitTime;
        
        // Para reputação
        struct {
            std::string factionName;
            int32_t amount;
        } reputation;
        
        // Para condição/ação personalizada
        struct {
            std::string scriptCode;
            std::map<std::string, std::string> parameters;
        } custom;
    } data;
    
    // Conexões para outros nós
    struct Connection {
        int32_t targetNodeId;
        QuestConnectionType type;
        std::string label;
    };
    
    std::vector<Connection> connections;
};

/**
 * @brief Representação simplificada de uma quest para a interface
 */
struct QuestUIModel {
    int32_t id;
    std::string name;
    std::string description;
    
    // Informações gerais
    struct {
        std::string displayName; // Nome visível para jogadores
        std::string shortDescription; // Descrição curta para listagem
        std::string longDescription; // Descrição detalhada
        int32_t recommendedLevel;
        bool repeatable;
        int32_t repeatCooldown; // Em segundos, 0 = sem cooldown
        bool autoStart; // Se a quest começa automaticamente
        bool hidden; // Se a quest é oculta (não aparece na lista)
        std::string category; // Categoria da quest
        std::string difficulty; // Dificuldade (fácil, médio, difícil, etc.)
    } info;
    
    // NPCs relacionados
    struct {
        int32_t startNpcId; // NPC que inicia a quest
        std::string startNpcName;
        int32_t endNpcId; // NPC que finaliza a quest
        std::string endNpcName;
    } npcs;
    
    // Requisitos para iniciar
    std::vector<QuestRequirement> requirements;
    
    // Recompensas
    std::vector<QuestReward> rewards;
    
    // Fluxograma da quest
    std::vector<QuestNode> flowchart;
    int32_t startNodeId;
    
    // Metadados
    struct {
        int32_t createdTimestamp;
        int32_t lastModifiedTimestamp;
        std::string author;
        std::string version;
        std::map<std::string, std::string> extraInfo;
    } metadata;
};

/**
 * @brief Resultado de operações relacionadas a quests
 */
struct QuestOperationResult {
    bool success;
    std::string message;
    std::optional<QuestUIModel> quest;
};

/**
 * @brief Filtros de pesquisa para quests
 */
struct QuestFilter {
    std::optional<std::string> nameContains;
    std::optional<std::string> category;
    std::optional<std::string> difficulty;
    std::optional<int32_t> minLevel;
    std::optional<int32_t> maxLevel;
    std::optional<bool> isRepeatable;
    std::optional<bool> isHidden;
    std::optional<int32_t> relatedNpcId;
    std::optional<int32_t> relatedItemId;
};

/**
 * @brief Criador visual de quests
 */
class QuestCreator {
public:
    /**
     * @brief Construtor
     * @param facade Referência ao facade principal do WYDStudio
     */
    QuestCreator(const std::shared_ptr<core::WYDStudioFacade>& facade);
    
    /**
     * @brief Destrutor
     */
    ~QuestCreator();
    
    /**
     * @brief Carrega lista de quests do servidor
     * @return true se carregado com sucesso
     */
    bool loadQuests();
    
    /**
     * @brief Obtém quest pelo ID
     * @param questId ID da quest
     * @return Modelo da quest ou nulo se não encontrada
     */
    std::optional<QuestUIModel> getQuestById(int32_t questId);
    
    /**
     * @brief Busca quests com filtragem
     * @param filter Filtros de busca
     * @return Lista de quests que correspondem aos filtros
     */
    std::vector<QuestUIModel> searchQuests(const QuestFilter& filter);
    
    /**
     * @brief Cria uma nova quest com valores padrão
     * @param name Nome da nova quest
     * @param category Categoria da quest
     * @return Resultado da operação contendo a nova quest
     */
    QuestOperationResult createNewQuest(const std::string& name, const std::string& category);
    
    /**
     * @brief Cria uma nova quest a partir de um template
     * @param name Nome da nova quest
     * @param templateId ID do template a usar
     * @return Resultado da operação contendo a nova quest
     */
    QuestOperationResult createQuestFromTemplate(const std::string& name, int32_t templateId);
    
    /**
     * @brief Salva alterações em uma quest
     * @param quest Modelo da quest modificada
     * @return Resultado da operação
     */
    QuestOperationResult saveQuest(const QuestUIModel& quest);
    
    /**
     * @brief Duplica uma quest existente
     * @param questId ID da quest a ser duplicada
     * @param newName Nome da quest duplicada
     * @return Resultado da operação contendo a quest duplicada
     */
    QuestOperationResult duplicateQuest(int32_t questId, const std::string& newName);
    
    /**
     * @brief Remove uma quest
     * @param questId ID da quest a ser removida
     * @return Resultado da operação
     */
    QuestOperationResult removeQuest(int32_t questId);
    
    /**
     * @brief Adiciona um nó ao fluxograma da quest
     * @param questId ID da quest
     * @param node Nó a adicionar
     * @return Resultado da operação
     */
    QuestOperationResult addNode(int32_t questId, const QuestNode& node);
    
    /**
     * @brief Atualiza um nó no fluxograma da quest
     * @param questId ID da quest
     * @param node Nó atualizado
     * @return Resultado da operação
     */
    QuestOperationResult updateNode(int32_t questId, const QuestNode& node);
    
    /**
     * @brief Remove um nó do fluxograma da quest
     * @param questId ID da quest
     * @param nodeId ID do nó a remover
     * @return Resultado da operação
     */
    QuestOperationResult removeNode(int32_t questId, int32_t nodeId);
    
    /**
     * @brief Adiciona uma conexão entre nós
     * @param questId ID da quest
     * @param sourceNodeId ID do nó de origem
     * @param targetNodeId ID do nó de destino
     * @param type Tipo de conexão
     * @param label Rótulo para a conexão
     * @return Resultado da operação
     */
    QuestOperationResult addConnection(
        int32_t questId, 
        int32_t sourceNodeId, 
        int32_t targetNodeId,
        QuestConnectionType type,
        const std::string& label);
    
    /**
     * @brief Remove uma conexão entre nós
     * @param questId ID da quest
     * @param sourceNodeId ID do nó de origem
     * @param targetNodeId ID do nó de destino
     * @return Resultado da operação
     */
    QuestOperationResult removeConnection(
        int32_t questId, 
        int32_t sourceNodeId, 
        int32_t targetNodeId);
    
    /**
     * @brief Adiciona um requisito à quest
     * @param questId ID da quest
     * @param requirement Requisito a adicionar
     * @return Resultado da operação
     */
    QuestOperationResult addRequirement(int32_t questId, const QuestRequirement& requirement);
    
    /**
     * @brief Remove um requisito da quest
     * @param questId ID da quest
     * @param requirementIndex Índice do requisito a remover
     * @return Resultado da operação
     */
    QuestOperationResult removeRequirement(int32_t questId, int32_t requirementIndex);
    
    /**
     * @brief Adiciona uma recompensa à quest
     * @param questId ID da quest
     * @param reward Recompensa a adicionar
     * @return Resultado da operação
     */
    QuestOperationResult addReward(int32_t questId, const QuestReward& reward);
    
    /**
     * @brief Remove uma recompensa da quest
     * @param questId ID da quest
     * @param rewardIndex Índice da recompensa a remover
     * @return Resultado da operação
     */
    QuestOperationResult removeReward(int32_t questId, int32_t rewardIndex);
    
    /**
     * @brief Gera uma miniatura do fluxograma da quest
     * @param questId ID da quest
     * @param width Largura da miniatura
     * @param height Altura da miniatura
     * @return Dados da imagem em formato PNG
     */
    std::vector<uint8_t> generateQuestFlowchartImage(int32_t questId, int32_t width, int32_t height);
    
    /**
     * @brief Gera texto de diálogo otimizado para uma quest
     * @param questId ID da quest
     * @param nodeId ID do nó para gerar diálogo
     * @param context Contexto para o diálogo
     * @return Texto gerado
     */
    std::string generateOptimizedDialogue(
        int32_t questId, 
        int32_t nodeId, 
        const std::string& context);
    
    /**
     * @brief Sugere melhoria para descrição de quest
     * @param questId ID da quest
     * @return Descrição melhorada
     */
    std::string suggestImprovedDescription(int32_t questId);
    
    /**
     * @brief Sugere balanceamento de recompensas para a quest
     * @param questId ID da quest
     * @return Lista de recompensas balanceadas
     */
    std::vector<QuestReward> suggestBalancedRewards(int32_t questId);
    
    /**
     * @brief Exporta uma quest para um arquivo
     * @param questId ID da quest a exportar
     * @param exportPath Caminho do arquivo de exportação
     * @return Resultado da operação
     */
    QuestOperationResult exportQuest(int32_t questId, const std::string& exportPath);
    
    /**
     * @brief Importa uma quest de um arquivo
     * @param importPath Caminho do arquivo de importação
     * @return Resultado da operação
     */
    QuestOperationResult importQuest(const std::string& importPath);
    
    /**
     * @brief Verifica a integridade e jogabilidade de uma quest
     * @param questId ID da quest a verificar
     * @return Lista de problemas encontrados (vazia se OK)
     */
    std::vector<std::string> validateQuest(int32_t questId);
    
    /**
     * @brief Obtém a lista de todas as categorias de quest disponíveis
     * @return Lista de categorias
     */
    std::vector<std::string> getQuestCategories();
    
    /**
     * @brief Obtém a lista de todas as dificuldades de quest disponíveis
     * @return Lista de dificuldades
     */
    std::vector<std::string> getQuestDifficulties();
    
    /**
     * @brief Obtém a lista de todos os templates de quest disponíveis
     * @return Lista de templates
     */
    std::vector<std::pair<int32_t, std::string>> getQuestTemplates();
    
    /**
     * @brief Obtém a lista de todos os tipos de nós de quest disponíveis
     * @return Lista de tipos de nós
     */
    std::vector<std::pair<QuestNodeType, std::string>> getNodeTypes();
    
    /**
     * @brief Obtém a lista de todos os tipos de conexões disponíveis
     * @return Lista de tipos de conexões
     */
    std::vector<std::pair<QuestConnectionType, std::string>> getConnectionTypes();
    
    /**
     * @brief Recarrega todas as quests do disco
     * @return true se recarregado com sucesso
     */
    bool reloadQuests();
    
    /**
     * @brief Aplica mudanças ao servidor sem reiniciar
     * @return true se aplicado com sucesso
     */
    bool applyChangesToLiveServer();
    
    /**
     * @brief Registra callback para notificações de alterações em quests
     * @param callback Função de callback
     * @return ID de registro (para cancelamento posterior)
     */
    uint32_t registerChangeCallback(std::function<void(const QuestUIModel&)> callback);
    
    /**
     * @brief Cancela registro de callback
     * @param callbackId ID do callback a cancelar
     */
    void unregisterChangeCallback(uint32_t callbackId);
    
    /**
     * @brief Obtém o histórico de operações na quest (para undo/redo)
     * @param questId ID da quest
     * @return Lista de operações, da mais recente para a mais antiga
     */
    std::vector<std::string> getQuestEditHistory(int32_t questId);
    
    /**
     * @brief Desfaz a última operação na quest
     * @param questId ID da quest
     * @return Resultado da operação
     */
    QuestOperationResult undoLastQuestOperation(int32_t questId);
    
    /**
     * @brief Refaz a última operação desfeita
     * @param questId ID da quest
     * @return Resultado da operação
     */
    QuestOperationResult redoQuestOperation(int32_t questId);
    
    /**
     * @brief Simula a execução da quest para verificar fluxo de progresso
     * @param questId ID da quest
     * @return Relatório da simulação
     */
    std::string simulateQuestExecution(int32_t questId);

private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Referência ao facade principal
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    
    // Cache de quests
    std::map<int32_t, QuestUIModel> m_questCache;
    
    // Histórico de operações por quest
    std::map<int32_t, std::vector<std::string>> m_questHistory;
    std::map<int32_t, int32_t> m_historyPosition; // Posição atual para redo
    
    // Validação de quests
    bool validateQuestIntegrity(const QuestUIModel& quest);
    
    // Gerenciamento de arquivos
    std::string getQuestsFilePath();
    
    // Utilitários de serialização
    json serializeQuestToJson(const QuestUIModel& quest);
    QuestUIModel deserializeQuestFromJson(const json& json);
    
    // Geração de IDs únicos
    int32_t generateUniqueNodeId(const QuestUIModel& quest);
};

}} // namespace wydstudio::ui

#endif // WYDSTUDIO_QUEST_CREATOR_H
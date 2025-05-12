#ifndef WYDSTUDIO_UI_DESIGNER_H
#define WYDSTUDIO_UI_DESIGNER_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <map>
#include <chrono>
#include <nlohmann/json.hpp>
#include "../core/WYDStudioFacade.h"
#include "../binary/BinaryHandler.h"

namespace wydstudio {
namespace ui {

using json = nlohmann::json;

/**
 * @brief Tipos de componentes da interface
 */
enum class UIComponentType {
    STATUS_BAR,       // Barra de status (HP, MP, etc)
    ACTION_BAR,       // Barra de ações/habilidades
    INVENTORY,        // Inventário
    EQUIPMENT,        // Equipamento
    CHARACTER_INFO,   // Informações do personagem
    SKILLS,           // Habilidades
    MINIMAP,          // Minimapa
    CHAT,             // Chat
    QUEST_LOG,        // Registro de quests
    TRADE,            // Comércio
    SHOP,             // Loja
    STORAGE,          // Armazenamento
    GUILD,            // Interface de guilda
    PARTY,            // Interface de grupo
    MENU,             // Menu principal
    DIALOG,           // Diálogo com NPC
    POPUP,            // Janela popup
    TOOLTIP,          // Tooltip
    CUSTOM            // Componente personalizado
};

/**
 * @brief Tipos de elementos da interface
 */
enum class UIElementType {
    PANEL,            // Painel ou janela
    BUTTON,           // Botão
    ICON,             // Ícone
    TEXT,             // Texto
    PROGRESS_BAR,     // Barra de progresso
    SLIDER,           // Slider
    CHECKBOX,         // Checkbox
    RADIO_BUTTON,     // Botão de rádio
    TEXTFIELD,        // Campo de texto
    DROPDOWN,         // Menu dropdown
    LIST,             // Lista
    GRID,             // Grade (inventário, etc)
    TAB,              // Aba
    SCROLLBAR,        // Barra de rolagem
    IMAGE,            // Imagem
    CONTAINER,        // Contêiner para outros elementos
    SEPARATOR,        // Separador
    TOOLTIP,          // Tooltip
    CUSTOM            // Elemento personalizado
};

/**
 * @brief Posição e tamanho de um elemento UI
 */
struct UIRect {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
    
    // Para posicionamento relativo ou âncoras
    std::string horizontalAlignment; // "left", "center", "right", "stretch"
    std::string verticalAlignment;   // "top", "middle", "bottom", "stretch"
    std::string parentAnchor;        // "topleft", "topcenter", "topright", etc.
    std::string childAnchor;         // "topleft", "topcenter", "topright", etc.
    
    // Margens
    int32_t marginLeft;
    int32_t marginTop;
    int32_t marginRight;
    int32_t marginBottom;
    
    // Para elementos redimensionáveis
    bool resizable;
    int32_t minWidth;
    int32_t minHeight;
    int32_t maxWidth;
    int32_t maxHeight;
    
    // Para correspondência com coordenadas do jogo
    float scaleX;
    float scaleY;
};

/**
 * @brief Aparência visual de um elemento UI
 */
struct UIStyle {
    // Cores
    std::string backgroundColor;
    std::string foregroundColor;
    std::string borderColor;
    std::string textColor;
    std::string highlightColor;
    
    // Transparência
    float opacity;
    float backgroundOpacity;
    
    // Fontes
    std::string fontFamily;
    int32_t fontSize;
    bool fontBold;
    bool fontItalic;
    int32_t fontSpacing;
    
    // Bordas
    int32_t borderSize;
    std::string borderStyle; // "solid", "dashed", "dotted", "none"
    int32_t cornerRadius;
    
    // Textura de fundo
    std::string backgroundTexture;
    std::string backgroundImageSlice; // "9-slice" para redimensionamento de texturas
    
    // Efeitos
    bool useShadow;
    int32_t shadowOffsetX;
    int32_t shadowOffsetY;
    int32_t shadowBlur;
    std::string shadowColor;
    
    bool useGlow;
    int32_t glowRadius;
    std::string glowColor;
    
    // Estados visuais
    struct StateStyle {
        std::string backgroundColor;
        std::string foregroundColor;
        std::string borderColor;
        std::string textColor;
        float opacity;
        std::string backgroundTexture;
    };
    
    StateStyle hoverStyle;
    StateStyle activeStyle;
    StateStyle disabledStyle;
    
    // Animações
    bool useAnimations;
    int32_t transitionDuration;
    std::string transitionEasing; // "linear", "ease", "ease-in", "ease-out", "ease-in-out"
};

/**
 * @brief Conteúdo dinâmico de um elemento UI
 */
struct UIContent {
    // Texto
    std::string text;
    std::string textKey; // Chave para internacionalização
    std::string textBindingPath; // Caminho para binding de dados
    
    // Imagem/Ícone
    std::string imagePath;
    std::string iconName;
    
    // Valores para barras de progresso, etc.
    float currentValue;
    float minValue;
    float maxValue;
    std::string valueSuffix;
    std::string valueBindingPath;
    std::string maxValueBindingPath;
    
    // Formatação
    std::string numberFormat; // "integer", "decimal:2", "percent", "currency", etc.
    bool useThousandsSeparator;
    
    // Especial para listas e grids
    std::string itemsSourcePath;
    std::string itemTemplate;
    
    // Especial para tooltips
    int32_t tooltipDelay;
    std::string tooltipPosition; // "top", "bottom", "left", "right"
};

/**
 * @brief Comportamento e eventos de um elemento UI
 */
struct UIBehavior {
    // Interatividade
    bool isInteractive;
    bool isDraggable;
    bool isDropTarget;
    
    // Visibilidade condicional
    std::string visibilityCondition;
    std::vector<std::string> showInContexts;
    std::vector<std::string> hideInContexts;
    
    // Foco
    bool canReceiveFocus;
    int32_t tabIndex;
    
    // Auto-hide
    bool autoHide;
    int32_t autoHideDelay;
    float autoHideOpacity;
    
    // Clique e toque
    bool useClickSound;
    std::string clickSoundName;
    std::string clickAction;
    std::string onClickFunction;
    
    // Teclas de atalho
    std::string shortcutKey;
    bool useShortcutModifier;
    bool shortcutCtrl;
    bool shortcutAlt;
    bool shortcutShift;
    
    // Arrastar e soltar
    std::string dragType;
    std::string dropAcceptTypes;
    std::string onDragFunction;
    std::string onDropFunction;
    
    // Animações de entrada/saída
    std::string entryAnimation; // "fade", "slide", "scale", "none"
    std::string exitAnimation;  // "fade", "slide", "scale", "none"
    int32_t animationDuration;
    
    // Dicas na interface
    bool showTooltip;
    std::string tooltipText;
    std::string tooltipBindingPath;
    
    // Outros eventos
    std::string onMouseEnterFunction;
    std::string onMouseLeaveFunction;
    std::string onFocusFunction;
    std::string onBlurFunction;
    std::string onValueChangeFunction;
};

/**
 * @brief Definição completa de um elemento UI
 */
struct UIElement {
    // Identificação
    std::string id;
    std::string name;
    UIElementType type;
    
    // Layout
    UIRect rect;
    
    // Aparência
    UIStyle style;
    
    // Conteúdo
    UIContent content;
    
    // Comportamento
    UIBehavior behavior;
    
    // Hierarquia
    std::string parentId;
    std::vector<std::string> childIds;
    int32_t zOrder;
    
    // Metadados
    bool isTemplate;
    std::string templateId;
    std::string category;
    std::string description;
    
    // Estado atual
    bool visible;
    bool enabled;
    
    // Dados específicos do elemento
    std::map<std::string, std::string> customProperties;
};

/**
 * @brief Definição completa de um componente UI
 */
struct UIComponent {
    // Identificação
    std::string id;
    std::string name;
    UIComponentType type;
    
    // Layout geral
    UIRect rect;
    
    // Estilo geral
    UIStyle style;
    
    // Comportamento geral
    UIBehavior behavior;
    
    // Elementos que compõem este componente
    std::vector<UIElement> elements;
    
    // Dados de arquivo
    std::string sourceFile; // Arquivo .wyt de origem
    int32_t version;
    
    // Metadados
    std::string category;
    std::string description;
    std::string author;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point modifiedAt;
    
    // Estado atual
    bool visible;
    bool enabled;
    
    // Contextos em que é visível
    std::vector<std::string> contexts;
    
    // Configurações especiais
    std::map<std::string, std::string> customProperties;
};

/**
 * @brief Tema completo para a UI
 */
struct UITheme {
    // Identificação
    std::string id;
    std::string name;
    
    // Cores principais
    std::string primaryColor;
    std::string secondaryColor;
    std::string accentColor;
    std::string backgroundColor;
    std::string textColor;
    
    // Estilos de componentes
    std::map<std::string, UIStyle> componentStyles;
    
    // Estilos de elementos
    std::map<std::string, UIStyle> elementStyles;
    
    // Textura de fundo global
    std::string backgroundTexture;
    
    // Fonte principal
    std::string fontFamily;
    int32_t defaultFontSize;
    
    // Metadados
    std::string author;
    std::string description;
    std::chrono::system_clock::time_point createdAt;
};

/**
 * @brief Layout completo da UI
 */
struct UILayout {
    // Identificação
    std::string id;
    std::string name;
    
    // Componentes
    std::vector<UIComponent> components;
    
    // Tema aplicado
    std::string themeId;
    
    // Resolução para a qual foi projetado
    int32_t designWidth;
    int32_t designHeight;
    
    // Configurações de escalabilidade
    bool autoScale;
    std::string scaleMode; // "preserve-width", "preserve-height", "stretch", "fit"
    
    // Metadados
    std::string author;
    std::string description;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point modifiedAt;
    
    // Versão
    int32_t version;
};

/**
 * @brief Resultado de operações relacionadas à UI
 */
struct UIOperationResult {
    bool success;
    std::string message;
    std::variant<
        std::monostate,
        UIComponent,
        UIElement,
        UITheme,
        UILayout
    > data;
};

/**
 * @brief Editor/Designer de UI do WYDStudio
 */
class UIDesigner {
public:
    /**
     * @brief Construtor
     * @param facade Referência ao facade principal do WYDStudio
     */
    UIDesigner(const std::shared_ptr<core::WYDStudioFacade>& facade);
    
    /**
     * @brief Destrutor
     */
    ~UIDesigner();
    
    /**
     * @brief Inicializa o designer de UI
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    // ===== COMPONENTES =====
    
    /**
     * @brief Obtém todos os componentes disponíveis
     * @param includeSystem Incluir componentes do sistema
     * @return Lista de componentes
     */
    std::vector<UIComponent> getAllComponents(bool includeSystem = true);
    
    /**
     * @brief Obtém um componente pelo ID
     * @param componentId ID do componente
     * @return Componente ou nulo se não encontrado
     */
    std::optional<UIComponent> getComponentById(const std::string& componentId);
    
    /**
     * @brief Cria um novo componente
     * @param name Nome do componente
     * @param type Tipo do componente
     * @return Resultado da operação contendo o novo componente
     */
    UIOperationResult createComponent(const std::string& name, UIComponentType type);
    
    /**
     * @brief Duplica um componente existente
     * @param componentId ID do componente
     * @param newName Nome do componente duplicado
     * @return Resultado da operação contendo o componente duplicado
     */
    UIOperationResult duplicateComponent(const std::string& componentId, const std::string& newName);
    
    /**
     * @brief Salva alterações em um componente
     * @param component Componente modificado
     * @return Resultado da operação
     */
    UIOperationResult saveComponent(const UIComponent& component);
    
    /**
     * @brief Remove um componente
     * @param componentId ID do componente
     * @return Resultado da operação
     */
    UIOperationResult removeComponent(const std::string& componentId);
    
    /**
     * @brief Importa componente de arquivo .wyt
     * @param filePath Caminho do arquivo
     * @return Resultado da operação contendo o componente importado
     */
    UIOperationResult importComponentFromWYT(const std::string& filePath);
    
    /**
     * @brief Exporta componente para arquivo .wyt
     * @param componentId ID do componente
     * @param filePath Caminho do arquivo
     * @return Resultado da operação
     */
    UIOperationResult exportComponentToWYT(const std::string& componentId, const std::string& filePath);
    
    // ===== ELEMENTOS =====
    
    /**
     * @brief Cria um novo elemento dentro de um componente
     * @param componentId ID do componente
     * @param name Nome do elemento
     * @param type Tipo do elemento
     * @param parentElementId ID do elemento pai (vazio para raiz)
     * @return Resultado da operação contendo o novo elemento
     */
    UIOperationResult createElement(const std::string& componentId, const std::string& name, 
                                   UIElementType type, const std::string& parentElementId = "");
    
    /**
     * @brief Atualiza um elemento existente
     * @param componentId ID do componente
     * @param element Elemento modificado
     * @return Resultado da operação
     */
    UIOperationResult updateElement(const std::string& componentId, const UIElement& element);
    
    /**
     * @brief Remove um elemento
     * @param componentId ID do componente
     * @param elementId ID do elemento
     * @return Resultado da operação
     */
    UIOperationResult removeElement(const std::string& componentId, const std::string& elementId);
    
    /**
     * @brief Move um elemento na hierarquia
     * @param componentId ID do componente
     * @param elementId ID do elemento
     * @param newParentId ID do novo pai (vazio para raiz)
     * @param index Posição no novo pai (-1 para fim)
     * @return Resultado da operação
     */
    UIOperationResult moveElement(const std::string& componentId, const std::string& elementId,
                                 const std::string& newParentId, int32_t index = -1);
    
    // ===== TEMAS =====
    
    /**
     * @brief Obtém todos os temas disponíveis
     * @return Lista de temas
     */
    std::vector<UITheme> getAllThemes();
    
    /**
     * @brief Obtém um tema pelo ID
     * @param themeId ID do tema
     * @return Tema ou nulo se não encontrado
     */
    std::optional<UITheme> getThemeById(const std::string& themeId);
    
    /**
     * @brief Cria um novo tema
     * @param name Nome do tema
     * @return Resultado da operação contendo o novo tema
     */
    UIOperationResult createTheme(const std::string& name);
    
    /**
     * @brief Salva alterações em um tema
     * @param theme Tema modificado
     * @return Resultado da operação
     */
    UIOperationResult saveTheme(const UITheme& theme);
    
    /**
     * @brief Remove um tema
     * @param themeId ID do tema
     * @return Resultado da operação
     */
    UIOperationResult removeTheme(const std::string& themeId);
    
    /**
     * @brief Aplica um tema a um layout
     * @param layoutId ID do layout
     * @param themeId ID do tema
     * @return Resultado da operação
     */
    UIOperationResult applyThemeToLayout(const std::string& layoutId, const std::string& themeId);
    
    // ===== LAYOUTS =====
    
    /**
     * @brief Obtém todos os layouts disponíveis
     * @return Lista de layouts
     */
    std::vector<UILayout> getAllLayouts();
    
    /**
     * @brief Obtém um layout pelo ID
     * @param layoutId ID do layout
     * @return Layout ou nulo se não encontrado
     */
    std::optional<UILayout> getLayoutById(const std::string& layoutId);
    
    /**
     * @brief Cria um novo layout
     * @param name Nome do layout
     * @return Resultado da operação contendo o novo layout
     */
    UIOperationResult createLayout(const std::string& name);
    
    /**
     * @brief Salva alterações em um layout
     * @param layout Layout modificado
     * @return Resultado da operação
     */
    UIOperationResult saveLayout(const UILayout& layout);
    
    /**
     * @brief Remove um layout
     * @param layoutId ID do layout
     * @return Resultado da operação
     */
    UIOperationResult removeLayout(const std::string& layoutId);
    
    /**
     * @brief Adiciona um componente a um layout
     * @param layoutId ID do layout
     * @param componentId ID do componente
     * @return Resultado da operação
     */
    UIOperationResult addComponentToLayout(const std::string& layoutId, const std::string& componentId);
    
    /**
     * @brief Remove um componente de um layout
     * @param layoutId ID do layout
     * @param componentId ID do componente
     * @return Resultado da operação
     */
    UIOperationResult removeComponentFromLayout(const std::string& layoutId, const std::string& componentId);
    
    // ===== EXPORTAÇÃO/APLICAÇÃO =====
    
    /**
     * @brief Exporta layout para arquivos do cliente
     * @param layoutId ID do layout
     * @param outputPath Pasta de saída
     * @return Resultado da operação
     */
    UIOperationResult exportLayoutToClient(const std::string& layoutId, const std::string& outputPath);
    
    /**
     * @brief Aplica layout ao servidor
     * @param layoutId ID do layout
     * @return Resultado da operação
     */
    UIOperationResult applyLayoutToServer(const std::string& layoutId);
    
    /**
     * @brief Gera preview de um componente ou layout
     * @param id ID do componente ou layout
     * @param isLayout True se for layout, false se for componente
     * @return Dados da imagem preview
     */
    std::vector<uint8_t> generatePreview(const std::string& id, bool isLayout);
    
    // ===== UTILITÁRIOS =====
    
    /**
     * @brief Verifica se um componente está em uso em algum layout
     * @param componentId ID do componente
     * @return True se em uso, false caso contrário
     */
    bool isComponentInUse(const std::string& componentId);
    
    /**
     * @brief Importa recursos visuais (imagens, texturas)
     * @param filePath Caminho do arquivo
     * @param resourceType Tipo de recurso ("texture", "icon", "background", etc.)
     * @return Resultado da operação
     */
    UIOperationResult importVisualResource(const std::string& filePath, const std::string& resourceType);
    
    /**
     * @brief Obtém recursos visuais disponíveis
     * @param resourceType Tipo de recurso ("texture", "icon", "background", etc.)
     * @return Lista de recursos
     */
    std::vector<std::string> getVisualResources(const std::string& resourceType);
    
private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Referência ao facade principal
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    
    // Cache de componentes, temas e layouts
    std::map<std::string, UIComponent> m_componentCache;
    std::map<std::string, UITheme> m_themeCache;
    std::map<std::string, UILayout> m_layoutCache;
    
    // Conversor binário para .wyt
    bool convertWYTToJson(const std::string& wytPath, json& outputJson);
    bool convertJsonToWYT(const json& jsonData, const std::string& wytPath);
    
    // Geração de IDs únicos
    std::string generateUniqueId(const std::string& prefix);
    
    // Serialização/Desserialização
    json serializeComponent(const UIComponent& component);
    UIComponent deserializeComponent(const json& json);
    
    json serializeTheme(const UITheme& theme);
    UITheme deserializeTheme(const json& json);
    
    json serializeLayout(const UILayout& layout);
    UILayout deserializeLayout(const json& json);
    
    // Validação
    bool validateComponent(const UIComponent& component, std::string& errorMessage);
    bool validateElement(const UIElement& element, std::string& errorMessage);
    bool validateTheme(const UITheme& theme, std::string& errorMessage);
    bool validateLayout(const UILayout& layout, std::string& errorMessage);
};

}} // namespace wydstudio::ui

#endif // WYDSTUDIO_UI_DESIGNER_H
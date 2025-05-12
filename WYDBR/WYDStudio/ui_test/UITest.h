#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <iostream>

// Forward declarations para classes de UI
namespace WYDStudio {
    class UIElement;
    class UIWindow;
    class UIButton;
    class UITextInput;
    class UILabel;
    class UIListBox;
    class UIPanel;
}

namespace WYDStudio {

/**
 * @enum UIElementType
 * @brief Tipos de elementos de interface
 */
enum class UIElementType {
    WINDOW,
    BUTTON,
    TEXT_INPUT,
    LABEL,
    LIST_BOX,
    PANEL,
    CHECKBOX,
    RADIO_BUTTON,
    PROGRESS_BAR,
    SLIDER,
    MENU,
    TAB,
    TREE_VIEW
};

/**
 * @class UIElement
 * @brief Classe base para todos os elementos de UI
 */
class UIElement {
public:
    UIElement(const std::string& id, int x, int y, int width, int height)
        : m_id(id), m_x(x), m_y(y), m_width(width), m_height(height),
          m_visible(true), m_enabled(true) {}
    
    virtual ~UIElement() {}
    
    // Métodos básicos
    virtual UIElementType GetType() const = 0;
    virtual void Render() = 0;
    virtual bool HandleEvent(const std::string& eventType, void* eventData) = 0;
    
    // Getters e setters
    const std::string& GetId() const { return m_id; }
    int GetX() const { return m_x; }
    int GetY() const { return m_y; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    bool IsVisible() const { return m_visible; }
    bool IsEnabled() const { return m_enabled; }
    
    void SetPosition(int x, int y) { m_x = x; m_y = y; }
    void SetSize(int width, int height) { m_width = width; m_height = height; }
    void SetVisible(bool visible) { m_visible = visible; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    // Test Stub: Simulação de renderização para teste
    virtual void TestRender() {
        std::cout << "Renderizando UI: " << GetId() << " [" 
                  << GetX() << "," << GetY() << "," 
                  << GetWidth() << "x" << GetHeight() << "]" << std::endl;
    }

protected:
    std::string m_id;
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    bool m_visible;
    bool m_enabled;
};

/**
 * @class UIWindow
 * @brief Janela de interface que pode conter outros elementos
 */
class UIWindow : public UIElement {
public:
    UIWindow(const std::string& id, const std::string& title, 
             int x, int y, int width, int height)
        : UIElement(id, x, y, width, height), m_title(title) {}
    
    UIElementType GetType() const override { return UIElementType::WINDOW; }
    
    void Render() override {
        // Na implementação real, renderizaria a janela e os elementos filhos
        for (auto& child : m_children) {
            child->Render();
        }
    }
    
    bool HandleEvent(const std::string& eventType, void* eventData) override {
        // Propaga eventos para elementos filhos
        for (auto& child : m_children) {
            if (child->HandleEvent(eventType, eventData)) {
                return true;
            }
        }
        return false;
    }
    
    // Adicionar/remover elementos filhos
    void AddElement(std::shared_ptr<UIElement> element) {
        m_children.push_back(element);
    }
    
    void RemoveElement(const std::string& elementId) {
        m_children.erase(
            std::remove_if(m_children.begin(), m_children.end(),
                          [&elementId](const std::shared_ptr<UIElement>& element) {
                              return element->GetId() == elementId;
                          }),
            m_children.end());
    }
    
    // Test Stub: Simulação de renderização para teste
    void TestRender() override {
        std::cout << "Renderizando Janela: " << GetId() << " [" 
                  << GetX() << "," << GetY() << "," 
                  << GetWidth() << "x" << GetHeight() << "]" << std::endl;
        std::cout << "  Título: " << m_title << std::endl;
        std::cout << "  Elementos filhos: " << m_children.size() << std::endl;
        
        for (auto& child : m_children) {
            std::cout << "  ";
            child->TestRender();
        }
    }
    
private:
    std::string m_title;
    std::vector<std::shared_ptr<UIElement>> m_children;
};

/**
 * @class UIButton
 * @brief Botão de interface
 */
class UIButton : public UIElement {
public:
    UIButton(const std::string& id, const std::string& text, 
             int x, int y, int width, int height)
        : UIElement(id, x, y, width, height), m_text(text) {}
    
    UIElementType GetType() const override { return UIElementType::BUTTON; }
    
    void Render() override {
        // Na implementação real, renderizaria o botão
    }
    
    bool HandleEvent(const std::string& eventType, void* eventData) override {
        if (eventType == "click" && m_clickHandler) {
            m_clickHandler();
            return true;
        }
        return false;
    }
    
    // Definir manipulador de clique
    void SetClickHandler(std::function<void()> handler) {
        m_clickHandler = handler;
    }
    
    // Test Stub: Simulação de renderização para teste
    void TestRender() override {
        std::cout << "Renderizando Botão: " << GetId() << " [" 
                  << GetX() << "," << GetY() << "," 
                  << GetWidth() << "x" << GetHeight() << "]" << std::endl;
        std::cout << "  Texto: " << m_text << std::endl;
        std::cout << "  Handler: " << (m_clickHandler ? "Definido" : "Não definido") << std::endl;
    }
    
    // Test Stub: Simular clique para teste
    void TestClick() {
        std::cout << "Clicando no botão: " << GetId() << " (" << m_text << ")" << std::endl;
        if (m_clickHandler) {
            m_clickHandler();
        } else {
            std::cout << "  Nenhum manipulador de clique definido!" << std::endl;
        }
    }
    
private:
    std::string m_text;
    std::function<void()> m_clickHandler;
};

/**
 * @class UITextInput
 * @brief Campo de entrada de texto
 */
class UITextInput : public UIElement {
public:
    UITextInput(const std::string& id, const std::string& placeholder, 
                int x, int y, int width, int height)
        : UIElement(id, x, y, width, height), 
          m_text(""), m_placeholder(placeholder) {}
    
    UIElementType GetType() const override { return UIElementType::TEXT_INPUT; }
    
    void Render() override {
        // Na implementação real, renderizaria o campo de texto
    }
    
    bool HandleEvent(const std::string& eventType, void* eventData) override {
        if (eventType == "textChanged" && m_changeHandler) {
            m_changeHandler(m_text);
            return true;
        }
        return false;
    }
    
    // Getters e setters
    const std::string& GetText() const { return m_text; }
    void SetText(const std::string& text) { 
        m_text = text; 
        if (m_changeHandler) {
            m_changeHandler(m_text);
        }
    }
    
    // Definir manipulador de mudança
    void SetChangeHandler(std::function<void(const std::string&)> handler) {
        m_changeHandler = handler;
    }
    
    // Test Stub: Simulação de renderização para teste
    void TestRender() override {
        std::cout << "Renderizando Campo de Texto: " << GetId() << " [" 
                  << GetX() << "," << GetY() << "," 
                  << GetWidth() << "x" << GetHeight() << "]" << std::endl;
        std::cout << "  Texto: '" << (m_text.empty() ? m_placeholder : m_text) << "'" << std::endl;
        std::cout << "  Handler: " << (m_changeHandler ? "Definido" : "Não definido") << std::endl;
    }
    
    // Test Stub: Simular entrada de texto
    void TestInput(const std::string& newText) {
        std::cout << "Digitando no campo: " << GetId() << std::endl;
        std::cout << "  Texto anterior: '" << m_text << "'" << std::endl;
        std::cout << "  Novo texto: '" << newText << "'" << std::endl;
        
        SetText(newText);
    }
    
private:
    std::string m_text;
    std::string m_placeholder;
    std::function<void(const std::string&)> m_changeHandler;
};

/**
 * @class UILabel
 * @brief Rótulo de texto
 */
class UILabel : public UIElement {
public:
    UILabel(const std::string& id, const std::string& text, 
            int x, int y, int width, int height)
        : UIElement(id, x, y, width, height), m_text(text) {}
    
    UIElementType GetType() const override { return UIElementType::LABEL; }
    
    void Render() override {
        // Na implementação real, renderizaria o rótulo
    }
    
    bool HandleEvent(const std::string& eventType, void* eventData) override {
        // Rótulos normalmente não processam eventos
        return false;
    }
    
    // Getters e setters
    const std::string& GetText() const { return m_text; }
    void SetText(const std::string& text) { m_text = text; }
    
    // Test Stub: Simulação de renderização para teste
    void TestRender() override {
        std::cout << "Renderizando Rótulo: " << GetId() << " [" 
                  << GetX() << "," << GetY() << "," 
                  << GetWidth() << "x" << GetHeight() << "]" << std::endl;
        std::cout << "  Texto: '" << m_text << "'" << std::endl;
    }
    
private:
    std::string m_text;
};

/**
 * @class UIListBox
 * @brief Lista de itens selecionáveis
 */
class UIListBox : public UIElement {
public:
    UIListBox(const std::string& id, int x, int y, int width, int height)
        : UIElement(id, x, y, width, height), m_selectedIndex(-1) {}
    
    UIElementType GetType() const override { return UIElementType::LIST_BOX; }
    
    void Render() override {
        // Na implementação real, renderizaria a lista
    }
    
    bool HandleEvent(const std::string& eventType, void* eventData) override {
        if (eventType == "selectionChanged" && m_selectionHandler) {
            m_selectionHandler(m_selectedIndex);
            return true;
        }
        return false;
    }
    
    // Manipulação de itens
    void AddItem(const std::string& item) {
        m_items.push_back(item);
    }
    
    void RemoveItem(int index) {
        if (index >= 0 && index < static_cast<int>(m_items.size())) {
            m_items.erase(m_items.begin() + index);
            if (m_selectedIndex == index) {
                m_selectedIndex = -1;
            } else if (m_selectedIndex > index) {
                m_selectedIndex--;
            }
        }
    }
    
    void ClearItems() {
        m_items.clear();
        m_selectedIndex = -1;
    }
    
    // Getters e setters
    int GetSelectedIndex() const { return m_selectedIndex; }
    
    std::string GetSelectedItem() const {
        if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_items.size())) {
            return m_items[m_selectedIndex];
        }
        return "";
    }
    
    void SetSelectedIndex(int index) {
        if (index >= -1 && index < static_cast<int>(m_items.size())) {
            m_selectedIndex = index;
            if (m_selectionHandler) {
                m_selectionHandler(m_selectedIndex);
            }
        }
    }
    
    // Definir manipulador de seleção
    void SetSelectionHandler(std::function<void(int)> handler) {
        m_selectionHandler = handler;
    }
    
    // Test Stub: Simulação de renderização para teste
    void TestRender() override {
        std::cout << "Renderizando Lista: " << GetId() << " [" 
                  << GetX() << "," << GetY() << "," 
                  << GetWidth() << "x" << GetHeight() << "]" << std::endl;
        std::cout << "  Itens: " << m_items.size() << std::endl;
        std::cout << "  Selecionado: " << (m_selectedIndex >= 0 ? std::to_string(m_selectedIndex) : "Nenhum") << std::endl;
        
        for (size_t i = 0; i < m_items.size(); i++) {
            std::cout << "    " << i << ": " << m_items[i] 
                      << (static_cast<int>(i) == m_selectedIndex ? " [SELECIONADO]" : "") 
                      << std::endl;
        }
    }
    
    // Test Stub: Simular seleção para teste
    void TestSelect(int index) {
        std::cout << "Selecionando item na lista: " << GetId() << std::endl;
        std::cout << "  Índice anterior: " << m_selectedIndex << std::endl;
        std::cout << "  Novo índice: " << index << std::endl;
        
        SetSelectedIndex(index);
    }
    
private:
    std::vector<std::string> m_items;
    int m_selectedIndex;
    std::function<void(int)> m_selectionHandler;
};

/**
 * @class UIPanel
 * @brief Painel que pode conter outros elementos
 */
class UIPanel : public UIElement {
public:
    UIPanel(const std::string& id, int x, int y, int width, int height)
        : UIElement(id, x, y, width, height) {}
    
    UIElementType GetType() const override { return UIElementType::PANEL; }
    
    void Render() override {
        // Na implementação real, renderizaria o painel e os elementos filhos
        for (auto& child : m_children) {
            child->Render();
        }
    }
    
    bool HandleEvent(const std::string& eventType, void* eventData) override {
        // Propaga eventos para elementos filhos
        for (auto& child : m_children) {
            if (child->HandleEvent(eventType, eventData)) {
                return true;
            }
        }
        return false;
    }
    
    // Adicionar/remover elementos filhos
    void AddElement(std::shared_ptr<UIElement> element) {
        m_children.push_back(element);
    }
    
    void RemoveElement(const std::string& elementId) {
        m_children.erase(
            std::remove_if(m_children.begin(), m_children.end(),
                          [&elementId](const std::shared_ptr<UIElement>& element) {
                              return element->GetId() == elementId;
                          }),
            m_children.end());
    }
    
    // Test Stub: Simulação de renderização para teste
    void TestRender() override {
        std::cout << "Renderizando Painel: " << GetId() << " [" 
                  << GetX() << "," << GetY() << "," 
                  << GetWidth() << "x" << GetHeight() << "]" << std::endl;
        std::cout << "  Elementos filhos: " << m_children.size() << std::endl;
        
        for (auto& child : m_children) {
            std::cout << "    ";
            child->TestRender();
        }
    }
    
private:
    std::vector<std::shared_ptr<UIElement>> m_children;
};

} // namespace WYDStudio
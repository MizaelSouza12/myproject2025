/**
 * ItemManager.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/item/ItemManager.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "ItemTypes.h"
#include <fstream>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <random>

#include "../../../core/Logger.h"
#include "../../../core/Utils.h"

namespace wydbr {


/**
 * @file ItemManager.cpp
 * @brief Implementação do gerenciador de itens do WYD
 * 
 * Este arquivo contém a implementação do gerenciador de itens que
 * se baseia na estrutura original do WYD, mas com melhorias significativas
 * para evitar bugs, aumentar a segurança e facilitar o uso.
 */

namespace wyd {

} // namespace wydbr

namespace server {

// Implementação de CItemManager

CItemManager& CItemManager::GetInstance() {
    static CItemManager instance;
    return instance;
}

CItemManager::CItemManager()
    /**
 * initialized_
 * 
 * Implementa a funcionalidade initialized_ conforme especificação original.
 * @param false Parâmetro false
 * @return Retorna :
 */

    : initialized_(false){
}

CItemManager::~CItemManager() {
    Shutdown();
}

bool CItemManager::Initialize(const char* itemListPath) {
    if (initialized_) {
        LOG_WARNING("ItemManager já inicializado");
        return true;
    }
    
    LOG_INFO("Inicializando ItemManager com arquivo: {}", itemListPath);
    
    // Carrega definições de itens do arquivo
    if (!LoadItemDefinitions(itemListPath)) {
        LOG_ERROR("Falha ao carregar definições de itens de: {}", itemListPath);
        return false;
    }
    
    initialized_ = true;
    LOG_INFO("ItemManager inicializado com sucesso: {} definições carregadas", items_.size());
    
    return true;
}

void CItemManager::Shutdown() {
    if (!initialized_) {
        return;
    }
    
    // Limpa os dados
    items_.clear();
    
    initialized_ = false;
    LOG_INFO("ItemManager finalizado");
}

const STRUCT_ITEMLIST* CItemManager::GetItemInfo(WORD itemId) const {
    if (!initialized_) {
        return nullptr;
    }
    
    auto it = items_.find(itemId);
    if (it == items_.end()) {
        return nullptr;
    }
    
    return &it->second;
}

std::string CItemManager::GetItemName(WORD itemId) const {
    const STRUCT_ITEMLIST* itemInfo = GetItemInfo(itemId);
    if (!itemInfo) {
        return "";
    }
    
    return itemInfo->szName;
}

STRUCT_ITEM CItemManager::CreateItem(WORD itemId, WORD amount, BYTE refine, bool hasLuck, BYTE additionalValue) {
    STRUCT_ITEM item;
    
    // Limpa o item
    item.Clear();
    
    // Item vazio?
    if (itemId == 0) {
        return item;
    }
    
    // Busca informações do item
    const STRUCT_ITEMLIST* itemInfo = GetItemInfo(itemId);
    if (!itemInfo) {
        LOG_WARNING("Tentativa de criar item com ID inválido: {}", itemId);
        return item;
    }
    
    // Define ID e quantidade
    item.wIndex = itemId;
    item.wAmount = amount;
    
    // Stackável? Limita a quantidade
    if (item.IsStackable() && item.wAmount > 999) {
        item.wAmount = 999;
    }
    // Não stackável? Força quantidade 1
    else if (!item.IsStackable() && item.wAmount != 1) {
        item.wAmount = 1;
    }
    
    // Se for equipamento, configura refinamento, sorte e opção adicional
    if (item.IsRefinable()) {
        // Limita refinamento a 15 (máximo do WYD original)
        item.byRefine = std::min<BYTE>(refine, 15);
        
        // Define sorte
        item.byLuck = hasLuck ? 1 : 0;
        
        // Define opção adicional (limite de 9 no WYD original)
        item.byOption = std::min<BYTE>(additionalValue, 9);
        
        // Define durabilidade máxima e atual
        item.byMaxDurability = CalculateMaxDurability(itemId);
        item.byDurability = item.byMaxDurability;
    }
    
    return item;
}

DWORD CItemManager::GetItemPrice(const STRUCT_ITEM& item, bool isSelling) const {
    if (item.IsEmpty()) {
        return 0;
    }
    
    // Busca informações do item
    const STRUCT_ITEMLIST* itemInfo = GetItemInfo(item.wIndex);
    if (!itemInfo) {
        return 0;
    }
    
    // Preço base
    DWORD basePrice = isSelling ? itemInfo->wSellPrice : itemInfo->wPrice;
    
    // Se for venda para NPC e o item estiver danificado, reduz o preço
    if (isSelling && item.IsDamaged()) {
        float durabilityRatio = static_cast<float>(item.byDurability) / item.byMaxDurability;
        basePrice = static_cast<DWORD>(basePrice * durabilityRatio);
    }
    
    // Se for equipamento refinado, aumenta o preço
    if (item.IsRefinable() && item.byRefine > 0) {
        // Cada nível de refinamento aumenta o preço em 20% (no WYD original era um valor fixo)
        basePrice += (basePrice * item.byRefine * 20) / 100;
    }
    
    // Se tiver sorte, aumenta o preço em 30%
    if (item.HasLuck()) {
        basePrice += (basePrice * 30) / 100;
    }
    
    // Se tiver opção adicional, aumenta o preç/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna o
 */
o
    if(item.byOption > 0){
        // Cada ponto de opção adicional aumenta o preço em 25%
        basePrice += (basePrice * item.byOption * 25) / 100;
    }
    
    // Se for stackável, multiplica pelo quantidade
    if (item.IsStackable() && item.wAmount > 1) {
        basePrice *= item.wAmount;
    }
    
    return basePrice;
}

bool CItemManager::CanEquipItem(WORD itemId, BYTE charClass) const {
    const STRUCT_ITEMLIST* itemInfo = GetItemInfo(itemId);
    if (!itemInfo) {
        return false;
    }
    
    // Todos podem usar qualquer item?
    if (itemInfo->byReqClass == CHARCLASS_ALL) {
        return true;
    }
    
    // O item requer uma classe especí/**
 * return
 * 
 * Implementa a funcionalidade return conforme especificação original.
 * @param charClass Parâmetro charClass
 * @return Retorna fica
 */
fica
    return(itemInfo->byReqClass == charClass);
}

bool CItemManager::MeetsRequirements(WORD itemId, BYTE level, BYTE str, BYTE intl, BYTE dex, BYTE con) const {
    const STRUCT_ITEMLIST* itemInfo = GetItemInfo(itemId);
    if (!itemInfo) {
        return false;
    }
    
    // Verifica requisitos de ní/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param itemInfo->byReqLevel Parâmetro itemInfo->byReqLevel
 * @return Retorna vel
 */
vel
    if(level < itemInfo->byReqLevel){
        return false;
    }
    
    // Verifica requisitos de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param itemInfo->byReqStr Parâmetro itemInfo->byReqStr
 * @return Retorna atributos
 */
 atributos
    if(str < itemInfo->byReqStr){
        return false;
    }
    
    if (intl < itemInfo->byReqInt) {
        return false;
    }
    
    if (dex < itemInfo->byReqDex) {
        return false;
    }
    
    if (con < itemInfo->byReqCon) {
        return false;
    }
    
    return true;
}

void CItemManager::RegisterItemDefinition(const STRUCT_ITEMLIST& itemList) {
    if (itemList.wIndex == 0) {
        LOG_WARNING("Tentativa de registrar item com ID 0");
        return;
    }
    
    // Registra ou atualiza definição
    items_[itemList.wIndex] = itemList;
    
    LOG_DEBUG("Item registrado: ID {}, Nome '{}'", itemList.wIndex, itemList.szName);
}

std::string CItemManager::ItemToString(const STRUCT_ITEM& item) const {
    if (item.IsEmpty()) {
        return "Item vazio";
    }
    
    std::stringstream ss;
    
    // Busca informações do item
    const STRUCT_ITEMLIST* itemInfo = GetItemInfo(item.wIndex);
    
    // Nome do item (do ItemList.bin se disponível, ou do ID)
    ss << "Item: " << (itemInfo ? itemInfo->szName : std::to_string(item.wIndex));
    
    // Se for refinado, adiciona o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna refinamento
 */
 refinamento
    if(item.byRefine > 0){
        ss << " +" << static_cast<int>(item.byRefine);
    }
    
    // Se tiver sorte, adiciona a sorte
    if (item.HasLuck()) {
        ss << " (Sorte)";
    }
    
    // Se tiver opção adicional, adiciona a opçã/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna o
 */
o
    if(item.byOption > 0){
        ss << " (+" << static_cast<int>(item.byOption) << ")";
    }
    
    // Se for stackável, adiciona a quantidade
    if (item.IsStackable() && item.wAmount > 1) {
        ss << " x" << item.wAmount;
    }
    
    // Se tiver durabilidade, adiciona a /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna durabilidade
 */
 durabilidade
    if(item.byMaxDurability > 0){
        ss << " [" << static_cast<int>(item.byDurability) << "/" << static_cast<int>(item.byMaxDurability) << "]";
    }
    
    // Se tiver efeitos, adiciona os efeitos
    bool hasEffects = false;
    for (int i = 0; i < MAX_ITEM_EFFECT; i++) {
        if (item.stEffect[i].IsValid()) {
            if (!hasEffects) {
                ss << " (Efeitos: ";
                hasEffects = true;
            }
            else {
                ss << ", ";
            }
            
            ss << static_cast<int>(item.stEffect[i].byType) << ":" << static_cast<int>(item.stEffect[i].byValue);
        }
    }
    
    if (hasEffects) {
        ss << ")";
    }
    
    return ss.str();
}

bool CItemManager::LoadItemDefinitions(const char* filePath) {
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param filePath Parâmetro filePath
 * @param std::ios::binary Parâmetro std::ios::binary
 * @return Retorna std::ifstream
 */

    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        LOG_ERROR("Falha ao abrir arquivo de itens: {}", filePath);
        return false;
    }
    
    // Limpa definições existentes
    items_.clear();
    
    // Calcula o número de definições baseado no tamanho do arquivo
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    size_t itemCount = fileSize / sizeof(STRUCT_ITEMLIST);
    LOG_INFO("Arquivo de itens contém {} definições", itemCount);
    
    if (itemCount == 0) {
        LOG_WARNING("Arquivo de itens vazio: {}", filePath);
        return false;
    }
    
    // Lê as definiçõ/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna es
 */
es
    for(size_t i = 0; i < itemCount; i++){
        STRUCT_ITEMLIST itemList;
        file.read(reinterpret_cast<char*>(&itemList), sizeof(STRUCT_ITEMLIST));
        
        // Verifica se é uma definição vá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna lida
 */
lida
        if(itemList.wIndex > 0){
            // Registra a definição
            items_[itemList.wIndex] = itemList;
        }
    }
    
    LOG_INFO("Carregadas {} definições de itens", items_.size());
    
    return items_.size() > 0;
}

BYTE CItemManager::CalculateMaxDurability(WORD itemId) const {
    const STRUCT_ITEMLIST* itemInfo = GetItemInfo(itemId);
    if (!itemInfo) {
        return 0;
    }
    
    // No WYD original, a durabilidade máxima era definida com base no tipo de item
    BYTE baseDurability = itemInfo->byDurability;
    
    // Durabilidade baseada no /**
 * switch
 * 
 * Implementa a funcionalidade switch conforme especificação original.
 * @param itemInfo->byType Parâmetro itemInfo->byType
 * @return Retorna tipo
 */
 tipo
    switch(itemInfo->byType){
        case ITEMTYPE_WEAPON:
            return baseDurability > 0 ? baseDurability : 60;
            
        case ITEMTYPE_ARMOR:
        case ITEMTYPE_HELM:
        case ITEMTYPE_BOOTS:
        case ITEMTYPE_SHIELD:
        case ITEMTYPE_GLOVE:
            return baseDurability > 0 ? baseDurability : 60;
            
        case ITEMTYPE_AMULET:
        case ITEMTYPE_RING:
        case ITEMTYPE_BELT:
        case ITEMTYPE_MANTLE:
            return baseDurability > 0 ? baseDurability : 60;
            
        case ITEMTYPE_MOUNT:
            return baseDurability > 0 ? baseDurability : 60;
            
        default:
            return baseDurability;
    }
}

WORD CItemManager::CalculateRefinedValue(WORD baseValue, BYTE refineLevel) const {
    if (refineLevel == 0 || baseValue == 0) {
        return baseValue;
    }
    
    // No WYD original, cada nível de refinamento aumentava o valor base em uma porcentagem
    // Refinamento +1: +7%, +2: +14%, etc. até +15: +105%
    float refineFactor = 1.0f + (refineLevel * 0.07f);
    
    return static_cast<WORD>(baseValue * refineFactor);
}

} // namespace server
} // namespace wyd
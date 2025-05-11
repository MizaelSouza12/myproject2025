class InventorySystem {
  constructor() {
    this.active = false;
    this.inventories = new Map();
  }

  async initialize() {
    console.log('Inicializando sistema de inventário...');
    this.active = true;
    return true;
  }

  async shutdown() {
    console.log('Desligando sistema de inventário...');
    this.active = false;
    return true;
  }

  // Métodos específicos do sistema de inventário
  addItem(playerId, item) {
    if (!this.active) throw new Error('Sistema de inventário não está ativo');
    if (!this.inventories.has(playerId)) {
      this.inventories.set(playerId, []);
    }
    this.inventories.get(playerId).push(item);
  }

  removeItem(playerId, itemId) {
    if (!this.active) throw new Error('Sistema de inventário não está ativo');
    const inventory = this.inventories.get(playerId);
    if (!inventory) return false;
    const index = inventory.findIndex(item => item.id === itemId);
    if (index === -1) return false;
    inventory.splice(index, 1);
    return true;
  }
}

module.exports = new InventorySystem(); 
module.exports = {
  name: 'inventory',
  dependencies: [],
  async initialize() {
    console.log('Inventory system inicializado');
    // Lógica de inicialização do inventário
    return true;
  },
  async shutdown() {
    console.log('Inventory system desligado');
    // Lógica de desligamento do inventário
    return true;
  }
}; 
module.exports = {
  name: 'combat',
  dependencies: ['inventory'],
  async initialize() {
    console.log('Combat system inicializado');
    // Lógica de inicialização do combate
    return true;
  },
  async shutdown() {
    console.log('Combat system desligado');
    // Lógica de desligamento do combate
    return true;
  }
}; 
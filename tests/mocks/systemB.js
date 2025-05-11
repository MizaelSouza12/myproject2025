/**
 * Mock do Sistema B para testes
 * Depende do Sistema A
 */

module.exports = function() {
  return {
    initialize: () => { throw new Error('Falha de inicialização'); },
    shutdown: () => true
  };
}; 
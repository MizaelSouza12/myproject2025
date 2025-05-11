/**
 * Configuração global para testes
 */

// Limpa o cache de módulos antes de cada teste
beforeEach(() => {
  jest.resetModules();
});

// Configura timeout global para testes
jest.setTimeout(30000);

// Configura variáveis de ambiente para testes
process.env.NODE_ENV = 'test';
process.env.TEST_MODE = 'true';

// Suprime logs durante os testes
global.console = {
  ...console,
  log: jest.fn(),
  error: jest.fn(),
  warn: jest.fn(),
  info: jest.fn(),
  debug: jest.fn(),
}; 
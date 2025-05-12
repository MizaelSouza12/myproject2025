# WYDBR 2.0

## Visão Geral
O projeto WYDBR 2.0 é uma reimplementação dos sistemas críticos baseada em engenharia reversa. O projeto visa implementar vários módulos fundamentais para o funcionamento do sistema.

## Sistemas Principais
O projeto está dividido em 5 partes principais:

1. **Sistemas de Servidor**
   - Sistema de Mundo
   - Sistema de Combate Avançado
   - Sistema de NPC

2. **Sistemas de Banco de Dados**
   - Sistema de Persistência
   - Sistema de Inventário
   - Sistema de Quests

3. **Sistemas de Segurança**
   - Sistema Anti-Cheat
   - Sistema de Autenticação
   - Sistema de Validação

4. **Sistemas de Rede**
   - Sistema de Sincronização
   - Sistema de Matchmaking
   - Sistema de Chat

5. **Integração**
   - Testes de Integração
   - Documentação
   - Implantação

## Implementação Atual
Atualmente, estamos implementando os sistemas utilizando scripts geradores que criam os arquivos necessários para cada componente. Devido ao tamanho e complexidade, cada sistema é gerado separadamente.

### Scripts Disponíveis
- `generate_world_system.sh`: Gera os arquivos para o Sistema de Mundo

## Como Usar
1. Execute o script para gerar os arquivos do sistema desejado:
   ```
   bash generate_world_system.sh
   ```

2. Compile os arquivos gerados:
   ```
   make world_system
   ```

3. Integre o sistema compilado ao projeto principal.

## Próximos Passos
- Implementar script para Sistema de Combate Avançado
- Implementar script para Sistema Anti-Cheat
- Implementar script para Sistema de Persistência
- Integrar todos os sistemas

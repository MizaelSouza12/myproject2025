# WYDBR Server - Release

## Sobre

Esta pasta contém os executáveis e arquivos de configuração necessários para executar um servidor WYDBR completo. Os binários estão compilados e prontos para uso em ambiente de produção.

## Requisitos de Sistema

- **Sistema Operacional**: Windows Server 2019/2022 ou Linux (Ubuntu 20.04+/CentOS 8+)
- **Processador**: Intel Xeon ou AMD EPYC com no mínimo 8 cores
- **Memória**: 16GB RAM mínimo, 32GB+ recomendado
- **Armazenamento**: SSD com 50GB+ de espaço livre
- **Rede**: Conexão dedicada com no mínimo 100Mbps, 1Gbps recomendado
- **Dependências**: 
  - Windows: Visual C++ Redistributable 2022
  - Linux: glibc 2.35+, libstdc++6

## Estrutura de Pastas

- **/bin**: Executáveis dos servidores (TMSrv, DBSrv, etc.)
- **/config**: Arquivos de configuração para cada servidor
- **/data**: Arquivos de dados do jogo e do servidor
- **/scripts**: Scripts de inicialização, manutenção e backup
- **/logs**: Pasta para armazenamento de logs (será preenchida durante execução)

## Inicialização do Servidor

### No Windows:
1. Execute `scripts/startup.bat` como administrador
2. Verifique os logs para confirmar que todos os serviços iniciaram corretamente

### No Linux:
1. Execute `./scripts/startup.sh`
2. Monitore os logs com `tail -f logs/tmsrv/current.log`

## Configuração

Antes de iniciar o servidor pela primeira vez:

1. Edite os arquivos em `/config` para configurar endereços IP, portas e taxas do servidor
2. Verifique `config/network.conf` para configurações de rede
3. Ajuste `config/rates.conf` para definir taxas de experiência, drop, etc.
4. Configure bancos de dados e autenticação em `config/db.conf`

## Monitoramento

Para monitorar o servidor:

1. Utilize `AdminTool.exe` na pasta `/bin` para administração em tempo real
2. Verifique os logs na pasta `/logs` para cada tipo de servidor
3. Monitore o consumo de recursos do sistema

## Manutenção

### Backups
O script `scripts/backup.sh` (ou `.bat`) realiza backups automáticos de:
- Banco de dados
- Contas de usuários
- Configurações
- Logs importantes

### Reinicialização
Para reiniciar o servidor:
1. Execute `scripts/shutdown.sh` (ou `.bat`)
2. Aguarde a finalização segura de todos os processos
3. Execute `scripts/startup.sh` (ou `.bat`)

### Atualização
Para atualizar o servidor:
1. Faça backup primeiro!
2. Execute `scripts/shutdown.sh` (ou `.bat`)
3. Substitua os binários na pasta `/bin`
4. Atualize os arquivos de dados em `/data`
5. Execute `scripts/startup.sh` (ou `.bat`)

## Segurança

- Mantenha o acesso ao servidor restrito apenas a administradores autorizados
- Utilize firewalls para limitar o acesso às portas do servidor
- Altere senhas padrão em todos os arquivos de configuração
- Revise regularmente os logs em busca de atividades suspeitas

## Suporte

Para obter suporte técnico:
- Consulte a documentação detalhada em `/docs`
- E-mail: [suporte@wydbrasil.com.br]
- Discord: [canal de suporte técnico]
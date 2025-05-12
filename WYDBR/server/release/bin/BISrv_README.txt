# BISrv - Servidor de Billing do WYDBR 2.0

## Descrição

O BISrv (Billing Integration Server) é o componente responsável pelo gerenciamento de transações financeiras, controle de moedas do jogo, loja virtual e integração com gateways de pagamento no WYDBR 2.0. Este servidor gerencia tanto a moeda premium (cash) quanto as moedas obtidas dentro do jogo, garante a segurança das transações e mantém registro detalhado de todas as operações financeiras.

## Características Principais

- Sistema multicamada para processamento seguro de transações
- Suporte a múltiplos gateways de pagamento (PayPal, Stripe, PagSeguro, etc.)
- Detecção avançada de fraudes e comportamentos suspeitos
- Sistema de cashback e recompensas para fidelização
- Gerenciamento de assinaturas e cobranças recorrentes
- Controle de créditos premium e moedas do jogo
- API RESTful para integrações externas
- Sistema de cupons e promoções
- Notificações em tempo real para jogadores
- Histórico e relatórios detalhados de transações
- Conformidade com regulamentações financeiras e LGPD/GDPR

## Arquitetura

O BISrv segue uma arquitetura modular dividida em:

1. **Transaction Core**
   - Processamento atômico de transações
   - Garantia de consistência
   - Mecanismos de rollback

2. **Payment Gateway Integrator**
   - Adaptadores para múltiplos gateways
   - Sistema de fallback
   - Normalização de APIs externas

3. **Virtual Store Manager**
   - Catálogo de produtos
   - Sistema de preços dinâmicos
   - Gerenciamento de promoções

4. **Fraud Detection System**
   - Análise comportamental
   - Detecção de padrões anormais
   - Regras configuráveis de segurança

5. **Reporting Engine**
   - Geração de relatórios
   - Análise de dados financeiros
   - Exportação em múltiplos formatos

## Requisitos de Sistema

- Sistema Operacional: Windows Server 2016+ ou Linux (Ubuntu 18.04+, CentOS 7+)
- CPU: 4+ cores, 2.5+ GHz
- RAM: 8+ GB (16+ GB recomendado para alta demanda)
- Armazenamento: SSD com 100+ GB (para logs e banco de dados local)
- Banco de Dados: MySQL 5.7+ ou MariaDB 10.3+ (dedicado recomendado)
- Dependências: Boost 1.70+, OpenSSL 1.1.1+, libcurl 7.68+, nlohmann/json 3.7.0+

## Compilação

### Windows

1. Instale o Visual Studio 2019 ou superior com suporte a C++
2. Instale o CMake 3.15 ou superior
3. Clone o repositório do WYDBR
4. Configure as dependências utilizando vcpkg:
   ```
   vcpkg install boost:x64-windows openssl:x64-windows curl:x64-windows nlohmann-json:x64-windows
   ```
5. Configure o projeto:
   ```
   mkdir build
   cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=[caminho para vcpkg]/scripts/buildsystems/vcpkg.cmake
   ```
6. Compile o projeto:
   ```
   cmake --build . --config Release
   ```
7. O executável será gerado em `build/bin/Release/BISrv.exe`

### Linux

1. Instale as dependências:
   ```
   sudo apt-get update
   sudo apt-get install build-essential cmake libboost-all-dev libssl-dev libcurl4-openssl-dev nlohmann-json3-dev
   ```
2. Clone o repositório do WYDBR
3. Configure o projeto:
   ```
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```
4. Compile o projeto:
   ```
   make -j$(nproc)
   ```
5. O executável será gerado em `build/bin/BISrv`

## Configuração

O servidor utiliza o arquivo `config/BISrv.conf` para sua configuração. Os principais parâmetros são:

- **Server.IP**: Endereço IP para bindar o servidor (padrão: 0.0.0.0)
- **Server.Port**: Porta para bindar o servidor (padrão: 7518)
- **Server.ThreadCount**: Número de threads de trabalho (padrão: número de cores)
- **Server.AdminPort**: Porta para interface de administração (padrão: 7519)

- **Database.Host**: Endereço do servidor de banco de dados
- **Database.Port**: Porta do servidor de banco de dados (padrão: 3306)
- **Database.User**: Usuário do banco de dados
- **Database.Password**: Senha do banco de dados
- **Database.Name**: Nome do banco de dados
- **Database.MaxConnections**: Número máximo de conexões ao banco de dados (padrão: 10)

- **Billing.Currency**: Moeda padrão (USD, EUR, BRL, etc.)
- **Billing.ItemExpirationDays**: Dias para expiração de itens (0 = sem expiração)
- **Billing.RefundWindow**: Janela de tempo em horas para reembolsos (padrão: 24)
- **Billing.TransactionTimeout**: Timeout de transações em segundos (padrão: 60)
- **Billing.RequireTwoFactor**: Exigir autenticação de dois fatores para transações acima de certo valor
- **Billing.HighValueThreshold**: Valor mínimo para considerar transação de alto valor

- **Fraud.EnableDetection**: Habilita detecção de fraude (padrão: true)
- **Fraud.MaxTransactionsPerDay**: Limite de transações diárias por conta
- **Fraud.MaxValuePerTransaction**: Valor máximo por transação
- **Fraud.MaxValuePerDay**: Valor máximo diário por conta
- **Fraud.SuspiciousIPs**: Lista de IPs suspeitos ou regiões bloqueadas

- **PaymentGateways**: Seção de configuração para cada gateway:
  - **PayPal.Enabled**: Habilita pagamento via PayPal (true/false)
  - **PayPal.ClientID**: ID de cliente da API PayPal
  - **PayPal.Secret**: Segredo da API PayPal
  - **PayPal.Sandbox**: Usar ambiente de sandbox (true/false)
  
  - **Stripe.Enabled**: Habilita pagamento via Stripe (true/false)
  - **Stripe.ApiKey**: Chave da API Stripe
  - **Stripe.WebhookSecret**: Segredo do webhook Stripe

  - **PagSeguro.Enabled**: Habilita pagamento via PagSeguro (true/false)
  - **PagSeguro.Email**: Email da conta PagSeguro
  - **PagSeguro.Token**: Token de acesso PagSeguro

- **VirtualStore.RefreshInterval**: Intervalo em minutos para atualização do catálogo
- **VirtualStore.PriceAdjustmentEnabled**: Habilita ajuste dinâmico de preços
- **VirtualStore.FeaturedItemsCount**: Número de itens em destaque

- **Log.Level**: Nível de detalhamento dos logs (DEBUG, INFO, WARNING, ERROR)
- **Log.Path**: Caminho para os arquivos de log
- **Log.TransactionLog**: Caminho para logs específicos de transações
- **Log.RotationSize**: Tamanho máximo do arquivo de log antes de rotacionar (MB)

- **Security.EnableEncryption**: Habilita criptografia de dados sensíveis
- **Security.EncryptionKey**: Chave para criptografia de dados
- **Security.AllowedIPs**: Lista de IPs administrativos com acesso privilegiado
- **Security.BruteForceProtection**: Proteção contra tentativas em força bruta

## Inicialização

Para iniciar o servidor:

```
./BISrv --config=/caminho/para/BISrv.conf
```

Parâmetros adicionais de linha de comando:

- `--help`: Exibe ajuda sobre parâmetros
- `--version`: Exibe a versão do servidor
- `--log-level=[level]`: Define o nível de log (sobrescreve a configuração)
- `--console`: Exibe logs no console além de gravá-los em arquivo
- `--maintenance`: Inicia em modo de manutenção (bloqueia transações financeiras)
- `--admin-only`: Permite apenas operações administrativas

## Interface Administrativa

O BISrv inclui uma interface administrativa Web acessível em:
http://[server-ip]:7519/admin

A interface permite:

- Visualização em tempo real das transações
- Relatórios financeiros e gráficos
- Gerenciamento do catálogo da loja virtual
- Configuração de promoções e descontos
- Monitoramento de atividades suspeitas
- Resolução de disputas e reembolsos
- Configuração de parâmetros do servidor

## Integração com Gateways de Pagamento

Para adicionar um novo gateway de pagamento:

1. Implemente a interface `IPaymentGateway` no arquivo source/billing/gateways/
2. Registre o gateway no sistema em source/billing/PaymentManager.cpp
3. Adicione as opções de configuração para o novo gateway no arquivo de configuração
4. Implemente os callbacks de notificação de pagamento (webhooks)

## Relatórios Financeiros

O BISrv gera automaticamente os seguintes relatórios:

- Transações diárias/mensais/anuais
- Receita por tipo de produto
- Conversão de usuários gratuitos para pagantes
- Análise de campanhas promocionais
- Detecção de anomalias e fraudes
- Comportamento de compra por região/servidor

Os relatórios podem ser acessados via interface administrativa ou exportados nos formatos CSV, PDF e JSON.

## Troubleshooting

### Problemas comuns e soluções:

1. **Falhas em transações**
   - Verifique os logs de transação em `/logs/bisrv_transaction_[date].log`
   - Confirme se o gateway de pagamento está respondendo corretamente
   - Verifique se há problemas de conectividade de rede

2. **Erros de comunicação com banco de dados**
   - Verifique se o servidor de banco de dados está operacional
   - Confirme as credenciais de acesso no arquivo de configuração
   - Verifique se o esquema do banco de dados está atualizado

3. **Problemas com webhook de gateways**
   - Verifique se o IP do gateway está na lista de IPs permitidos
   - Confirme se a URL do webhook está configurada corretamente no painel do gateway
   - Verifique logs específicos do gateway em `/logs/bisrv_gateway_[name]_[date].log`

4. **Lentidão na validação de pagamentos**
   - Aumente o número de threads de trabalho
   - Otimize as consultas ao banco de dados
   - Considere particionar o banco de transações para melhorar performance

5. **Falha ao iniciar o servidor**
   - Verifique se os arquivos de configuração estão corretamente formatados
   - Confirme se as portas especificadas não estão em uso por outros processos
   - Verifique permissões de arquivo para certificados e chaves

## Segurança

O BISrv implementa múltiplas camadas de segurança:

1. **Criptografia**
   - Todas as comunicações usam TLS 1.3
   - Dados sensíveis são encriptados em repouso
   - Chaves de API são rotacionadas periodicamente

2. **Autenticação**
   - Suporte a autenticação de dois fatores
   - Tokens de sessão com expiração curta
   - Verificações de IP e fingerprint do dispositivo

3. **Auditoria**
   - Todas as operações administrativas são registradas
   - Trilha de auditoria completa para cada transação
   - Alertas em tempo real para atividades suspeitas

4. **Compliance**
   - Conformidade com PCI-DSS para processamento de pagamentos
   - Implementação de requisitos GDPR/LGPD para dados pessoais
   - Políticas configuráveis de retenção de dados

## Suporte e Contato

Para suporte técnico, entre em contato com a equipe de desenvolvimento do WYDBR através do email: billing-support@wydbr.com

## Licença

Este software é proprietário e confidencial. Uso não autorizado é estritamente proibido. © 2023 WYDBR Team. 
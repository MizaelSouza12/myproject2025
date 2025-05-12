# Sistema de Segurança Baseado em Executáveis do WYDBR 2.0

## Visão Geral

O WYDBR 2.0 implementa um sistema de segurança avançado baseado em executáveis, eliminando completamente os arquivos em lote (`.bat`) em favor de uma solução mais moderna e segura. Este documento descreve a arquitetura, funcionamento e configuração deste sistema.

## Componentes Principais

### 1. WYDLauncher.exe

O WYDLauncher é o ponto de entrada principal para o WYDBR 2.0, responsável por:

- Verificar os requisitos do sistema
- Validar a integridade dos arquivos
- Configurar o ambiente de execução
- Iniciar o cliente do jogo

#### Verificação de Requisitos do Sistema

O WYDLauncher verifica automaticamente:

- **Sistema Operacional**: Verifica a compatibilidade com Windows
- **.NET Framework/Runtime**: Verifica se o .NET 4.8 ou superior está instalado
- **DirectX**: Verifica a presença do DirectX necessário para o jogo
- **Visual C++ Redistributable**: Verifica se o Visual C++ Redistributable 2015-2022 está instalado
- **Drivers de GPU**: Verifica se os drivers de vídeo estão atualizados
- **Espaço em Disco**: Garante que há espaço suficiente para o jogo

Se algum requisito estiver faltando, o launcher oferece a opção de baixar e instalar os componentes necessários.

### 2. WydConfig.exe

O WydConfig é o utilitário de configuração gráfica para o WYDBR 2.0, permitindo aos jogadores:

- Configurar o motor de renderização (DirectX 11, Vulkan, Compatibility Mode)
- Ajustar resolução e modo de exibição
- Definir qualidade gráfica (baixa, média, alta, ultra)
- Otimizar configurações de desempenho
- Personalizar configurações avançadas (para usuários experientes)

### 3. Sistema de Configuração

O sistema utiliza arquivos JSON para armazenar configurações:

- **graphics_config.json**: Armazena configurações gráficas e de desempenho
- **security_config.json**: Armazena configurações relacionadas à segurança

## Características de Segurança

### Proteção Contra Scripts Maliciosos

O sistema implementa:

- **Modo Executável Exclusivo**: Nenhum arquivo .bat é utilizado
- **Verificação de Assinatura**: Verifica a integridade dos executáveis
- **Verificação de Integridade de Arquivos**: Garante que arquivos críticos não foram modificados
- **Prevenção de Execução de Scripts**: Bloqueia a execução de scripts não autorizados

### Modo Seguro

O WYDLauncher pode ser iniciado com o parâmetro `-secure` para ativar o modo de segurança máxima:

```
WYDLauncher.exe -secure
```

No modo seguro:
- A verificação de assinatura é habilitada
- A verificação de integridade de arquivos é mais rigorosa
- A proteção contra execução de scripts é maximizada

## Implementação Técnica

### Versão Node.js para Desenvolvimento

Durante o desenvolvimento, implementamos versões em Node.js dos executáveis:

- **WYDLauncher.js**: Implementação Node.js do WYDLauncher.exe
- **WydConfig.js**: Implementação Node.js do WydConfig.exe

Estas versões são funcionalmente idênticas às versões compiladas em C#, mas servem como ambiente de desenvolvimento e teste.

### Versão C# para Produção

Para o ambiente de produção, os executáveis são compilados a partir do código-fonte C#:

- **WYDLauncher_Essential.cs**: Código-fonte do WYDLauncher.exe
- **WydConfig_Development.cs**: Código-fonte do WydConfig.exe

## Compilação dos Executáveis

Os executáveis são compilados usando o script `compilar_executaveis.sh`, que usa o compilador .NET para gerar executáveis Windows a partir do código-fonte C#.

## Configuração

### Arquivos de Configuração

- **config/graphics_config.json**: Configurações gráficas
  ```json
  {
    "version": "2.0.0",
    "graphics": {
      "renderer": "auto",
      "quality": "high",
      "resolution": {
        "width": 1920,
        "height": 1080,
        "fullscreen": true
      }
    }
  }
  ```

- **security/security_config.json**: Configurações de segurança
  ```json
  {
    "security": {
      "fileIntegrityCheck": true,
      "preventScriptExecution": true,
      "executableOnly": true,
      "signatureVerification": false
    }
  }
  ```

## Utilização

### Fluxo de Inicialização Padrão

1. Execute `WYDLauncher.exe`
2. O launcher verifica os requisitos do sistema
3. O launcher verifica a integridade dos arquivos
4. Se tudo estiver correto, o cliente do jogo é iniciado

### Configuração Gráfica

1. Execute `WydConfig.exe`
2. Ajuste as configurações conforme necessário
3. Salve as alterações
4. Inicie o jogo através do launcher

## Solução de Problemas

### Erro de Requisitos do Sistema

Se o launcher detectar algum requisito faltando:
1. Selecione a opção para instalar os componentes faltantes
2. Aguarde a conclusão da instalação
3. Reinicie o launcher

### Problemas de Inicialização

Se o jogo não iniciar:
1. Execute o launcher em modo de depuração: `WYDLauncher.exe -debug`
2. Verifique os logs em `logs/launcher.log`
3. Tente executar o reparo de arquivos: `WYDLauncher.exe -repair`

## Conclusão

O Sistema de Segurança Baseado em Executáveis do WYDBR 2.0 representa uma evolução significativa na segurança e confiabilidade, eliminando completamente o uso de arquivos .bat e proporcionando uma experiência mais moderna e segura para os jogadores.
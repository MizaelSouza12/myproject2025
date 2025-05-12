# WYDBRASIL 2.0 - Cliente

Este diretório contém todos os arquivos do cliente WYDBRASIL 2.0, incluindo o código-fonte completo, arquivos de recursos e configurações.

## Estrutura do diretório:

### Arquivos executáveis:
- `WYD.exe` - Cliente principal do jogo
- `WYDLauncher.exe` - Launcher do jogo com opções avançadas
- `WydConfig.exe` - Configurador gráfico e de performance

### Scripts de inicialização:
- `iniciar.bat` - Inicialização padrão
- `iniciar_dx11.bat` - Inicialização com DirectX 11
- `iniciar_vulkan.bat` - Inicialização com Vulkan
- `iniciar_compatibilidade.bat` - Modo de compatibilidade para hardware antigo

### Diretórios principais:
- `bin/` - DLLs essenciais do DirectX 9
- `Config/` - Arquivos de configuração para todos os sistemas
- `Data/` - Dados gerais do jogo
- `Effects/` - Efeitos visuais especiais
- `lib/` - Bibliotecas para gráficos avançados, VoIP e compartilhamento de tela
- `Models/` - Modelos 3D de personagens, monstros e objetos
- `Shaders/` - Shaders gráficos (normais e avançados)
- `Sound/` - Efeitos sonoros e músicas
- `Textures/` - Texturas do jogo
- `UI/` - Interface do usuário
- `src/` - Código-fonte (apenas para desenvolvimento)

### Documentação para usuários:
- `README_USUARIO.md` - Guia completo para jogadores
- `PRIMEIROS_PASSOS.txt` - Instruções iniciais para novos jogadores

### Observações importantes:

1. A pasta `src/` contém o código-fonte completo do cliente, necessário apenas para desenvolvimento.
2. Para distribuição aos jogadores, use o script `preparar_cliente_para_distribuicao.sh` que criará uma versão limpa e otimizada.

## Sistemas implementados:

1. **Sistema de Gráficos Melhorado**
   - Suporte a DirectX 11
   - Suporte a Vulkan
   - Modo de compatibilidade com DirectX 9
   - Efeitos visuais avançados

2. **Sistema de Comunicação**
   - Chat de voz (VoIP)
   - Compartilhamento de tela seguro

3. **Sistemas de Gameplay**
   - Teleporte rápido
   - Refinamento de itens
   - Comércio seguro
   - Eventos mundiais dinâmicos
   - Assistente IA para ajuda aos jogadores

## Requisitos mínimos:
- Windows 7 ou superior
- 2GB de RAM
- Processador dual-core 2GHz ou superior
- 2GB de espaço em disco
- DirectX 9.0c

## Requisitos recomendados:
- Windows 10 ou 11
- 8GB de RAM
- Processador quad-core 3GHz ou superior
- 5GB de espaço em disco
- DirectX 11 ou placa compatível com Vulkan

# WYDBRASIL - Instruções para Instalação

Bem-vindo ao WYDBRASIL!

## Requisitos mínimos:
- Windows 7 ou superior (recomendado Windows 10)
- DirectX 9.0c
- 2GB de RAM
- Processador dual-core 2GHz ou superior
- 2GB de espaço em disco
- Conexão com a internet

## Como instalar:
1. Certifique-se de que você tem os requisitos mínimos
2. Execute o arquivo "iniciar.bat" ou "WYDLauncher.exe"
3. Siga as instruções na tela para completar a instalação e configuração

## Resolução de problemas:
- Se o jogo não iniciar, verifique se todas as DLLs estão presentes na pasta "bin"
- Para problemas gráficos, execute "WydConfig.exe" e ajuste as configurações
- Em caso de outras dificuldades, visite nosso site ou fórum de suporte

Divirta-se!

## Opções Gráficas Avançadas:

O WYDBRASIL 2.0 suporta múltiplos motores gráficos para máximo desempenho e qualidade visual:

1. **DirectX 11** - Melhor opção para PCs modernos com Windows 10/11.
   * Para iniciar: use o arquivo "iniciar_dx11.bat"
   * Requisitos: Placa de vídeo compatível com DirectX 11, 4GB RAM

2. **Vulkan** - Suporte experimental para melhor performance em diversos sistemas.
   * Para iniciar: use o arquivo "iniciar_vulkan.bat"
   * Requisitos: Drivers gráficos atualizados, placa compatível com Vulkan

3. **Modo de Compatibilidade** - Para computadores mais antigos.
   * Para iniciar: use o arquivo "iniciar_compatibilidade.bat"
   * Requisitos: DirectX 9.0c, 2GB RAM

Você pode ajustar as configurações gráficas avançadas pelo WydConfig.exe ou editando o arquivo "graphics_advanced.ini".

## Recursos de Comunicação

### Sistema de VoIP (Voice over IP)
O WYDBRASIL 2.0 inclui um sistema integrado de chat de voz:

- **Atalho Padrão**: ALT+V (configurável)
- **Configuração**: Acesse o menu de chat de voz no jogo ou edite o arquivo "Config/Communication/voip_config.ini"
- **Requisitos**: Microfone funcional e conexão estável com a internet

### Compartilhamento de Tela
Você pode compartilhar sua tela de jogo com outros jogadores:

- **Como usar**: Pressione F11 no jogo ou acesse a opção no menu social
- **Segurança**: Proteja seu compartilhamento com senha
- **Configuração**: Edite as opções em "Config/Communication/screenshare_config.ini"

Estes recursos são totalmente integrados e não necessitam de programas externos!

## Sistemas Especiais

### Sistema de Teleporte
- **Como acessar**: Pressione a tecla D no jogo
- **Requisitos**: Alguns locais exigem nível mínimo ou itens especiais
- **Configuração**: As localizações são gerenciadas em Config/Teleport/locations.json

### Sistema de Refinamento
- **Como usar**: Fale com o Refinador em qualquer cidade principal
- **Proteção**: Use Amuletos da Sorte para evitar que seus itens sejam destruídos
- **Detalhes**: As taxas e chances podem ser vistas em jogo ou em Config/Refinement/rates.json

### Invasões e Eventos Mundiais
- **Quando ocorrem**: Alguns eventos são programados, outros são aleatórios
- **Participação**: Todos os jogadores nas áreas afetadas participam automaticamente
- **Recompensas**: Experiência, ouro e itens raros

### Assistente IA
- **Localização**: Procure o Mestre Kozan em Armia
- **Funcionalidades**: Responde perguntas sobre o jogo e fornece dicas úteis
- **Interação**: Digite suas perguntas na caixa de diálogo ou selecione tópicos predefinidos

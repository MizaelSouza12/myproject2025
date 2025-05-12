# Verificação de Integridade do WYDBRASIL 2.0

Este documento permite verificar se todos os arquivos necessários estão presentes
e íntegros após o download.

## Arquivos Essenciais

Verifique se os seguintes arquivos estão presentes:

- Executáveis:
  - WYD.exe (4.1 MB)
  - WYDLauncher.exe (2.8 MB)
  - WydConfig.exe (58 KB)

- Scripts de inicialização:
  - iniciar.bat
  - iniciar_dx11.bat
  - iniciar_vulkan.bat
  - iniciar_compatibilidade.bat

- Arquivos de configuração:
  - WYDBR.ini
  - Graphics.ini
  - graphics_advanced.ini
  - Music.ini
  - Config.bin
  - WNotice.ini

## Pastas Essenciais

Verifique se as seguintes pastas estão presentes:

- Pastas de recursos:
  - Models/ (Modelos 3D)
  - Textures/ (Texturas)
  - Sound/ (Sons e músicas)
  - Shaders/ (Shaders gráficos)
  - UI/ (Interface do usuário)
  - Data/ (Dados do jogo)

- Pastas de bibliotecas:
  - bin/ (DLLs essenciais)
  - lib/ (Bibliotecas avançadas)
  - lib/dx11/ (DirectX 11)
  - lib/vulkan/ (Vulkan)
  - lib/opengl/ (OpenGL)
  - lib/voip/ (Chat de voz)
  - lib/screenshare/ (Compartilhamento de tela)

## Como verificar a integridade

1. Certifique-se de que todos os arquivos listados acima estão presentes
2. Verifique se os tamanhos dos executáveis correspondem aos valores indicados
3. Em caso de problemas, baixe novamente o cliente completo

## Solução de problemas comuns

- **Erro "DLL Ausente"**: Verifique se todas as pastas bin/ e lib/ estão presentes
- **Erro ao iniciar**: Tente utilizar outro script de inicialização (ex: iniciar_compatibilidade.bat)
- **Desempenho ruim**: Ajuste as configurações gráficas com WydConfig.exe

@echo off
REM Script de teste dos menus modernos WYDBR
REM Gerado automaticamente por verificar_e_aplicar_recursos_wydbr.sh

echo ===================================
echo  WYDBRASIL - Teste de Menus Modernos
echo ===================================
echo.

echo Verificando arquivos necessários...
echo.

REM Verificar arquivos de menu
if not exist "UI\moderna\ajustes_avancados.txt" echo ERRO: Arquivo de menu não encontrado: UI\moderna\ajustes_avancados.txt
if not exist "UI\moderna\compartilhamento.txt" echo ERRO: Arquivo de menu não encontrado: UI\moderna\compartilhamento.txt
if not exist "UI\moderna\acessar_tela.txt" echo ERRO: Arquivo de menu não encontrado: UI\moderna\acessar_tela.txt
if not exist "UI\moderna\chat_voz.txt" echo ERRO: Arquivo de menu não encontrado: UI\moderna\chat_voz.txt
if not exist "UI\moderna\amigos.txt" echo ERRO: Arquivo de menu não encontrado: UI\moderna\amigos.txt
if not exist "UI\moderna\perfil.txt" echo ERRO: Arquivo de menu não encontrado: UI\moderna\perfil.txt
if not exist "UI\moderna\teleporte.txt" echo ERRO: Arquivo de menu não encontrado: UI\moderna\teleporte.txt
if not exist "UI\moderna\indicadores.txt" echo ERRO: Arquivo de menu não encontrado: UI\moderna\indicadores.txt

echo.
echo Verificando arquivo de interface...
if not exist "UI\interface.txt" echo ERRO: Arquivo de interface não encontrado: UI\interface.txt

echo.
echo Verificando arquivo de atalhos...
if not exist "config\atalhos.ini" echo ERRO: Arquivo de atalhos não encontrado: config\atalhos.ini

echo.
echo Verificando executáveis...
if not exist "bin\WYD.exe" echo ERRO: Executável não encontrado: bin\WYD.exe
if not exist "bin\WYDLauncher.exe" echo ERRO: Executável não encontrado: bin\WYDLauncher.exe

echo.
echo ===================================
echo Instruções de Teste:
echo ===================================
echo.
echo Para testar os novos menus, execute o jogo e use os seguintes atalhos:
echo - ALT+A: Menu de Ajustes Avançados
echo - ALT+C: Menu de Compartilhamento de Tela
echo - ALT+R: Menu para Acessar Tela de outro jogador
echo - ALT+V: Ativar/desativar VOIP
echo - ALT+G: Menu de Chat de Voz
echo - D: Menu de Teleporte Rápido
echo.
echo Interações especiais:
echo - Clique na barra de vida: Abre o Menu de Perfil
echo - Clique direito no nome do usuário remoto: Abre menu com opções
echo - Mouse sobre indicadores de desempenho: Mostra tooltips detalhados
echo.

echo Pressione qualquer tecla para iniciar o jogo...
pause >nul

echo.
echo Iniciando o jogo...
cd bin
start WYD.exe

@echo off
echo Compilando gerador de correcoes WYDBR 2.0...

:: Verifica se o diretorio de saida existe, senao cria
if not exist build mkdir build

:: Compila o gerador de correcoes
g++ -std=c++17 main.cpp -o build/wydbr_fix_generator.exe -pthread -lstdc++fs

if %ERRORLEVEL% EQU 0 (
    echo Compilacao concluida com sucesso!
    echo Execute o gerador com: build\wydbr_fix_generator.exe [caminho_de_saida]
) else (
    echo Erro na compilacao!
    exit /b 1
) 
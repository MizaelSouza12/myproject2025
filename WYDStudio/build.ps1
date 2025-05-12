# Script de compilação do WYDStudio para Windows
param(
    [switch]$Release,
    [switch]$Clean,
    [switch]$Package,
    [switch]$Tests
)

# Configurar variáveis de ambiente
$ErrorActionPreference = "Stop"
$BuildType = if ($Release) { "Release" } else { "Debug" }
$BuildDir = "build-$BuildType"

# Verificar dependências
function Check-Dependencies {
    Write-Host "Verificando dependências..." -ForegroundColor Yellow
    
    # Verificar CMake
    if (-not (Get-Command "cmake" -ErrorAction SilentlyContinue)) {
        throw "CMake não encontrado. Por favor, instale o CMake."
    }
    
    # Verificar Qt
    if (-not (Test-Path Env:Qt6_DIR)) {
        throw "Qt6 não encontrado. Por favor, configure a variável de ambiente Qt6_DIR."
    }
    
    # Verificar Visual Studio
    if (-not (Get-Command "cl" -ErrorAction SilentlyContinue)) {
        throw "Compilador MSVC não encontrado. Por favor, execute este script em um Developer Command Prompt do Visual Studio."
    }
    
    Write-Host "Todas as dependências encontradas." -ForegroundColor Green
}

# Limpar diretório de build
function Clean-Build {
    if (Test-Path $BuildDir) {
        Write-Host "Limpando diretório de build anterior..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force $BuildDir
    }
}

# Configurar projeto
function Configure-Project {
    Write-Host "Configurando projeto..." -ForegroundColor Yellow
    
    New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
    Push-Location $BuildDir
    
    $CMakeArgs = @(
        "-G", "Visual Studio 17 2022",
        "-A", "x64",
        "-DCMAKE_BUILD_TYPE=$BuildType",
        "-DCMAKE_PREFIX_PATH=$Env:Qt6_DIR"
    )
    
    if ($Tests) {
        $CMakeArgs += "-DWYDSTUDIO_BUILD_TESTS=ON"
    }
    
    cmake $CMakeArgs ..
    
    if ($LASTEXITCODE -ne 0) {
        throw "Falha na configuração do CMake."
    }
    
    Pop-Location
    Write-Host "Projeto configurado com sucesso." -ForegroundColor Green
}

# Compilar projeto
function Build-Project {
    Write-Host "Compilando projeto..." -ForegroundColor Yellow
    
    Push-Location $BuildDir
    cmake --build . --config $BuildType --parallel
    
    if ($LASTEXITCODE -ne 0) {
        throw "Falha na compilação."
    }
    
    Pop-Location
    Write-Host "Projeto compilado com sucesso." -ForegroundColor Green
}

# Executar testes
function Run-Tests {
    if ($Tests) {
        Write-Host "Executando testes..." -ForegroundColor Yellow
        
        Push-Location $BuildDir
        ctest -C $BuildType --output-on-failure
        
        if ($LASTEXITCODE -ne 0) {
            throw "Falha nos testes."
        }
        
        Pop-Location
        Write-Host "Testes executados com sucesso." -ForegroundColor Green
    }
}

# Criar pacote de instalação
function Create-Package {
    if ($Package) {
        Write-Host "Criando pacote de instalação..." -ForegroundColor Yellow
        
        Push-Location $BuildDir
        cpack -C $BuildType
        
        if ($LASTEXITCODE -ne 0) {
            throw "Falha na criação do pacote."
        }
        
        Pop-Location
        Write-Host "Pacote criado com sucesso." -ForegroundColor Green
    }
}

# Função principal
function Main {
    try {
        Check-Dependencies
        
        if ($Clean) {
            Clean-Build
        }
        
        Configure-Project
        Build-Project
        Run-Tests
        Create-Package
        
        Write-Host "`nBuild completa com sucesso!" -ForegroundColor Green
    }
    catch {
        Write-Host "`nErro durante a build: $_" -ForegroundColor Red
        exit 1
    }
}

# Executar
Main 
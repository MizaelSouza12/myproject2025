# Requisitos de Sistema - WYDBRASIL 2.0

## Requisitos Mínimos

Para garantir a melhor experiência com o WYDBRASIL 2.0, seu sistema deve atender aos seguintes requisitos mínimos:

### Sistema Operacional
- Windows 10 (64 bits) ou superior
- Windows 8.1 (64 bits) com atualizações mais recentes
- Windows 7 SP1 (64 bits) com atualizações mais recentes

### Hardware
- Processador: Intel Core i3-6100 / AMD FX-4350 ou superior
- Memória RAM: 4 GB
- Placa de vídeo: NVIDIA GeForce GTX 660 ou AMD Radeon HD 7850 (2 GB VRAM)
- Armazenamento: 5 GB de espaço livre em disco
- Conexão de Internet: 2 Mbps de velocidade para jogo online

### Componentes de Software
- **.NET Framework/Runtime**: .NET Framework 4.8 ou .NET 6.0+
- **DirectX**: DirectX 11
- **Visual C++ Redistributable**: Visual C++ Redistributable 2015-2022
- **DirectX End-User Runtime**: Atualização mais recente

## Requisitos Recomendados

Para uma experiência ideal com gráficos aprimorados e desempenho superior:

### Hardware
- Processador: Intel Core i5-8400 / AMD Ryzen 5 2600 ou superior
- Memória RAM: 8 GB
- Placa de vídeo: NVIDIA GeForce GTX 1060 ou AMD Radeon RX 580 (4 GB VRAM)
- Armazenamento: 10 GB de espaço livre em disco SSD
- Conexão de Internet: 10 Mbps de velocidade para jogo online

## Verificação Automática

O WYDLauncher verifica automaticamente os seguintes componentes do seu sistema:

### 1. .NET Framework/Runtime
- Verifica se o .NET Framework 4.8 ou .NET 6.0+ está instalado
- Se necessário, oferece download e instalação automática

### 2. DirectX
- Verifica se o DirectX adequado está instalado
- Detecta DLLs essenciais do DirectX no sistema
- Oferece instalação do DirectX End-User Runtime se necessário

### 3. Visual C++ Redistributable
- Verifica se o Visual C++ Redistributable 2015-2022 está instalado
- Oferece download e instalação automática se necessário

### 4. Drivers de GPU
- Verifica se os drivers da placa de vídeo estão atualizados
- Alerta sobre drivers desatualizados (mais de 3 anos)
- Fornece links para download dos drivers mais recentes

### 5. Espaço em Disco
- Verifica se há pelo menos 5 GB de espaço livre em disco
- Alerta se o espaço disponível estiver abaixo do recomendado

## Mensagens de Erro Comuns

### Erro: .NET Framework não encontrado
**Solução**: Permitir que o WYDLauncher baixe e instale o .NET Framework 4.8 automaticamente, ou instalá-lo manualmente a partir do [site oficial da Microsoft](https://dotnet.microsoft.com/download/dotnet-framework/net48).

### Erro: DirectX incompleto ou desatualizado
**Solução**: Permitir que o WYDLauncher baixe e instale o DirectX End-User Runtime, ou instalá-lo manualmente a partir do [site oficial da Microsoft](https://www.microsoft.com/download/details.aspx?id=35).

### Erro: Visual C++ Redistributable ausente
**Solução**: Permitir que o WYDLauncher baixe e instale o Visual C++ Redistributable, ou instalá-lo manualmente a partir do [site oficial da Microsoft](https://aka.ms/vs/17/release/vc_redist.x64.exe).

### Erro: Drivers de GPU desatualizados
**Solução**: Atualizar os drivers da placa de vídeo a partir do site do fabricante:
- NVIDIA: [https://www.nvidia.com/Download/index.aspx](https://www.nvidia.com/Download/index.aspx)
- AMD: [https://www.amd.com/en/support](https://www.amd.com/en/support)
- Intel: [https://www.intel.com/content/www/us/en/download/driver.html](https://www.intel.com/content/www/us/en/download/driver.html)

### Erro: Espaço em disco insuficiente
**Solução**: Liberar espaço em disco removendo arquivos desnecessários ou mover o jogo para uma unidade com mais espaço disponível.

## Modos de Compatibilidade

O WYDBRASIL 2.0 oferece múltiplos modos de renderização para atender a diferentes configurações de hardware:

### Modo DirectX 11 (Padrão)
- Melhor qualidade visual
- Requer placas de vídeo compatíveis com DirectX 11
- Recomendado para hardware moderno

### Modo Vulkan
- Melhor desempenho em hardware mais recente
- Suporte amplo a recursos avançados
- Compatível com AMD, NVIDIA e Intel recentes

### Modo de Compatibilidade
- Suporte para hardware mais antigo
- Menor exigência de recursos
- Recomendado para computadores com especificações abaixo do mínimo

O WYDLauncher detecta automaticamente o melhor modo para seu hardware, mas você pode alterá-lo manualmente através do WydConfig.

## Configuração de Hardware Automática

O WydConfig detecta automaticamente as especificações do seu hardware e aplica as configurações gráficas mais adequadas. Você pode:

1. Usar as configurações automáticas (recomendado)
2. Escolher entre predefinições (Baixa, Média, Alta, Ultra)
3. Personalizar cada configuração manualmente

Para usuários com hardware abaixo do recomendado, o modo de "Baixa Especificação" pode ser ativado para melhorar significativamente o desempenho.
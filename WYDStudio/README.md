# WYDStudio

WYDStudio é uma ferramenta avançada de administração para servidores WYD (With Your Destiny), oferecendo uma interface moderna e completa para gerenciamento, desenvolvimento e monitoramento.

## Características

- 🎮 Interface gráfica moderna com tema dark
- 🛠️ Editores visuais para itens, mobs, mapas, skills e quests
- 📊 Monitoramento em tempo real do servidor
- 🤖 Sistema de IA integrado para automação e assistência
- 👥 Gerenciamento avançado de jogadores
- 💰 Controle econômico do servidor
- 🎯 Sistema de eventos automatizado
- 🔒 Sistema de permissões granular
- 📦 Integração com TMSrv/DBSrv

## Requisitos do Sistema

- Windows 10/11 64-bit
- Visual Studio 2022 com C++ build tools
- Qt 6.5.0 ou superior
- CMake 3.16 ou superior
- MySQL 8.0 ou superior
- OpenSSL 1.1.1 ou superior

## Instalação

### 1. Dependências

1. Instale o Visual Studio 2022 com:
   - Desenvolvimento para Desktop com C++
   - CMake tools
   - Windows SDK

2. Instale o Qt 6:
   - Baixe o Qt Online Installer
   - Selecione Qt 6.5.0 ou superior para MSVC 2022 64-bit
   - Instale Qt Charts

3. Instale o MySQL Server:
   ```powershell
   winget install Oracle.MySQL
   ```

4. Configure as variáveis de ambiente:
   ```powershell
   setx Qt6_DIR "C:\Qt\6.5.0\msvc2022_64"
   setx PATH "%PATH%;%Qt6_DIR%\bin"
   ```

### 2. Compilação

1. Clone o repositório:
   ```powershell
   git clone https://github.com/wydbr/wydstudio.git
   cd wydstudio
   ```

2. Execute o script de build:
   ```powershell
   # Build de desenvolvimento
   .\build.ps1

   # Build de release com pacote de instalação
   .\build.ps1 -Release -Package
   ```

### 3. Instalação

1. Instale usando o pacote gerado:
   ```powershell
   .\build-Release\WYDStudio-1.0.0-win64.exe
   ```

2. Configure o banco de dados:
   ```sql
   mysql -u root -p < sql/schema.sql
   ```

## Uso

### Primeira Execução

1. Inicie o WYDStudio
2. Faça login com as credenciais padrão:
   - Usuário: admin
   - Senha: admin123
3. Configure as conexões com TMSrv e DBSrv
4. Configure as permissões de usuários

### Funcionalidades Principais

#### Editor de Itens
- Interface visual para edição de itens
- Visualização 3D dos equipamentos
- Sistema de templates
- Importação/Exportação em massa

#### Editor de Mobs
- Editor visual de mobs e NPCs
- Sistema de IA para comportamentos
- Configuração de drops e experiência
- Gerenciamento de spawns

#### Editor de Mapas
- Editor visual de mapas
- Sistema de colisão
- Configuração de portais
- Gerenciamento de eventos

#### Monitoramento
- Métricas em tempo real
- Gráficos de desempenho
- Alertas configuráveis
- Histórico de eventos

#### Administração
- Gerenciamento de contas
- Sistema de banimento
- Controle econômico
- Logs detalhados

#### Sistema de IA
- Assistente inteligente
- Detecção de bots
- Análise de comportamento
- Automação de tarefas

## Contribuição

1. Fork o projeto
2. Crie uma branch para sua feature (`git checkout -b feature/AmazingFeature`)
3. Commit suas mudanças (`git commit -m 'Add some AmazingFeature'`)
4. Push para a branch (`git push origin feature/AmazingFeature`)
5. Abra um Pull Request

## Licença

Este projeto está licenciado sob a licença MIT - veja o arquivo [LICENSE](LICENSE) para detalhes.

## Suporte

- 📧 Email: suporte@wydbr.com
- 💬 Discord: [WYDBR Community](https://discord.gg/wydbr)
- 📚 Documentação: [docs.wydbr.com](https://docs.wydbr.com)
- 🐛 Issues: [GitHub Issues](https://github.com/wydbr/wydstudio/issues)
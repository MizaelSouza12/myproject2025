# Ferramentas de Compatibilidade WYDBR

Este diretório contém ferramentas para garantir a compatibilidade do cliente WYDBR 2.0
com todos os sistemas modernos implementados, mantendo compatibilidade binária.

## 1. Verificação de Compatibilidade

Para verificar se o cliente está pronto para distribuição:

```bash
cd /caminho/para/WYDBR/tools
node verify_compatibility.js
```

Este script verifica se todos os arquivos necessários estão presentes e se as
configurações estão corretas. Um relatório detalhado será exibido.

## 2. Correção de Compatibilidade Gráfica

Se o cliente não estiver com suporte a gráficos modernos, execute:

```bash
cd /caminho/para/WYDBR/tools/patches
node wyd_graphics_patcher.js
```

Este script adiciona suporte aos novos motores gráficos (DirectX 11, Vulkan, OpenGL)
no executável WYD.exe sem modificar sua estrutura original.

## 3. Preparação para Distribuição

Depois de verificar e corrigir a compatibilidade, prepare a versão final:

```bash
cd /caminho/para/WYDBR
./preparar_cliente_para_distribuicao.sh
```

Isto criará uma pasta `cliente_distribuicao` com todos os arquivos necessários,
prontos para serem distribuídos aos jogadores.

## Observações Importantes

- Estes scripts não modificam a estrutura interna do WYD.exe, apenas adicionam
  uma seção de compatibilidade que será lida pelas novas DLLs.
  
- Sempre faça backup antes de aplicar correções.

- Após aplicar as correções, teste todas as funcionalidades para garantir que
  tudo está funcionando corretamente.

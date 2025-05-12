/**
 * WYDBR - Verificador de Compatibilidade de Cliente
 *
 * Este script verifica se os arquivos do cliente estão
 * de acordo com as especificações necessárias para suportar
 * todos os sistemas implementados.
 */

const fs = require('fs');
const path = require('path');
const crypto = require('crypto');

const CLIENTE_DIR = path.join(__dirname, '..', 'cliente');

// Lista de verificações
const checks = [
  {
    name: 'Verificar WYD.exe',
    check: () => {
      const exePath = path.join(CLIENTE_DIR, 'WYD.exe');
      const exists = fs.existsSync(exePath);
      const size = exists ? fs.statSync(exePath).size : 0;
      return {
        status: exists && size > 1000000, // Deve ter pelo menos 1MB
        message: exists ? `OK (Tamanho: ${(size/1024/1024).toFixed(2)} MB)` : 'FALTANDO'
      };
    }
  },
  {
    name: 'Verificar WYDLauncher.exe',
    check: () => {
      const exePath = path.join(CLIENTE_DIR, 'WYDLauncher.exe');
      const exists = fs.existsSync(exePath);
      const size = exists ? fs.statSync(exePath).size : 0;
      return {
        status: exists && size > 1000000,
        message: exists ? `OK (Tamanho: ${(size/1024/1024).toFixed(2)} MB)` : 'FALTANDO'
      };
    }
  },
  {
    name: 'Verificar WydConfig.exe',
    check: () => {
      const exePath = path.join(CLIENTE_DIR, 'WydConfig.exe');
      const exists = fs.existsSync(exePath);
      const size = exists ? fs.statSync(exePath).size : 0;
      return {
        status: exists && size > 10000,
        message: exists ? `OK (Tamanho: ${(size/1024).toFixed(2)} KB)` : 'FALTANDO'
      };
    }
  },
  {
    name: 'Verificar arquivos de configuração',
    check: () => {
      const requiredConfigs = ['WYDBR.ini', 'Graphics.ini', 'Music.ini', 'Config.bin', 'WNotice.ini'];
      const missing = [];
      
      for (const config of requiredConfigs) {
        if (!fs.existsSync(path.join(CLIENTE_DIR, config))) {
          missing.push(config);
        }
      }
      
      return {
        status: missing.length === 0,
        message: missing.length === 0 ? 'OK' : `Faltando: ${missing.join(', ')}`
      };
    }
  },
  {
    name: 'Verificar DLLs necessárias',
    check: () => {
      const requiredDlls = ['d3d9.dll', 'd3dx9_43.dll', 'dinput8.dll', 'dsound.dll', 'xinput1_3.dll'];
      const missing = [];
      
      for (const dll of requiredDlls) {
        if (!fs.existsSync(path.join(CLIENTE_DIR, 'bin', dll))) {
          missing.push(dll);
        }
      }
      
      return {
        status: missing.length === 0,
        message: missing.length === 0 ? 'OK' : `Faltando: ${missing.join(', ')}`
      };
    }
  },
  {
    name: 'Verificar bibliotecas gráficas avançadas',
    check: () => {
      const requiredLibs = [
        'lib/dx11/d3d11.dll',
        'lib/dx11/dxgi.dll',
        'lib/vulkan/vulkan-1.dll',
        'lib/opengl/opengl32.dll',
        'lib/opengl/glew32.dll'
      ];
      const missing = [];
      
      for (const lib of requiredLibs) {
        if (!fs.existsSync(path.join(CLIENTE_DIR, lib))) {
          missing.push(lib);
        }
      }
      
      return {
        status: missing.length === 0,
        message: missing.length === 0 ? 'OK' : `Faltando: ${missing.join(', ')}`
      };
    }
  },
  {
    name: 'Verificar suporte a VoIP',
    check: () => {
      const requiredVoip = [
        'lib/voip/opus.dll',
        'lib/voip/audio_processing.dll',
        'Config/Communication/voip_config.ini'
      ];
      const missing = [];
      
      for (const file of requiredVoip) {
        if (!fs.existsSync(path.join(CLIENTE_DIR, file))) {
          missing.push(file);
        }
      }
      
      return {
        status: missing.length === 0,
        message: missing.length === 0 ? 'OK' : `Faltando: ${missing.join(', ')}`
      };
    }
  },
  {
    name: 'Verificar suporte a Compartilhamento de Tela',
    check: () => {
      const requiredFiles = [
        'lib/screenshare/screen_capture.dll',
        'lib/screenshare/secure_streaming.dll',
        'Config/Communication/screenshare_config.ini'
      ];
      const missing = [];
      
      for (const file of requiredFiles) {
        if (!fs.existsSync(path.join(CLIENTE_DIR, file))) {
          missing.push(file);
        }
      }
      
      return {
        status: missing.length === 0,
        message: missing.length === 0 ? 'OK' : `Faltando: ${missing.join(', ')}`
      };
    }
  },
  {
    name: 'Verificar Sistema de Teleporte',
    check: () => {
      const configPath = path.join(CLIENTE_DIR, 'Config/Teleport/locations.json');
      if (!fs.existsSync(configPath)) {
        return {
          status: false,
          message: 'Arquivo de configuração do sistema de teleporte não encontrado'
        };
      }
      
      try {
        const config = JSON.parse(fs.readFileSync(configPath, 'utf8'));
        return {
          status: config.locations && config.locations.length > 0,
          message: `OK (${config.locations.length} localizações configuradas)`
        };
      } catch (e) {
        return {
          status: false,
          message: `Erro ao ler arquivo de configuração: ${e.message}`
        };
      }
    }
  },
  {
    name: 'Verificar Sistema de Refinamento',
    check: () => {
      const configPath = path.join(CLIENTE_DIR, 'Config/Refinement/rates.json');
      if (!fs.existsSync(configPath)) {
        return {
          status: false,
          message: 'Arquivo de configuração do sistema de refinamento não encontrado'
        };
      }
      
      try {
        const config = JSON.parse(fs.readFileSync(configPath, 'utf8'));
        return {
          status: config.baseRates && config.protectionItems,
          message: 'OK (Configuração de refinamento válida)'
        };
      } catch (e) {
        return {
          status: false,
          message: `Erro ao ler arquivo de configuração: ${e.message}`
        };
      }
    }
  },
  {
    name: 'Verificar Pastas Essenciais',
    check: () => {
      const requiredDirs = ['Models', 'Textures', 'Sound', 'UI', 'Shaders', 'Data'];
      const missing = [];
      
      for (const dir of requiredDirs) {
        if (!fs.existsSync(path.join(CLIENTE_DIR, dir))) {
          missing.push(dir);
        }
      }
      
      return {
        status: missing.length === 0,
        message: missing.length === 0 ? 'OK' : `Faltando: ${missing.join(', ')}`
      };
    }
  },
  {
    name: 'Verificar Scripts de Inicialização',
    check: () => {
      const requiredScripts = ['iniciar.bat', 'iniciar_dx11.bat', 'iniciar_vulkan.bat', 'iniciar_compatibilidade.bat'];
      const missing = [];
      
      for (const script of requiredScripts) {
        if (!fs.existsSync(path.join(CLIENTE_DIR, script))) {
          missing.push(script);
        }
      }
      
      return {
        status: missing.length === 0,
        message: missing.length === 0 ? 'OK' : `Faltando: ${missing.join(', ')}`
      };
    }
  }
];

// Executar verificações
console.log('\n=================================');
console.log('RELATÓRIO DE COMPATIBILIDADE WYDBR');
console.log('=================================\n');

let passedChecks = 0;
let totalChecks = checks.length;

for (const [index, checkItem] of checks.entries()) {
  const result = checkItem.check();
  console.log(`[${index + 1}/${totalChecks}] ${checkItem.name}: ${result.status ? '✓' : '✗'} ${result.message}`);
  
  if (result.status) {
    passedChecks++;
  }
}

const percentage = (passedChecks / totalChecks * 100).toFixed(1);
console.log('\n=================================');
console.log(`RESUMO: ${passedChecks}/${totalChecks} verificações passaram (${percentage}%)`);
console.log(`STATUS: ${passedChecks === totalChecks ? 'PRONTO PARA DISTRIBUIÇÃO' : 'REQUER CORREÇÕES'}`);
console.log('=================================\n');

if (passedChecks !== totalChecks) {
  console.log('Recomendações para correção:');
  console.log('1. Execute o script preparar_cliente_para_distribuicao.sh');
  console.log('2. Verifique as DLLs ausentes ou corrompidas');
  console.log('3. Complete as configurações faltantes\n');
}

// Script Node.js para análise automatizada de problemas no WYDBR
// Gera relatório de arquivos faltantes, funções não implementadas, stubs, TODOs/FIXMEs, includes quebrados
// Cria arquivos e funções stubs automaticamente
// Expande para detectar ponteiros não inicializados, variáveis não usadas, memory leaks, funções não usadas, blocos comentados grandes, assert/abort/exit
// Uso: node generate_wydbr_fix_report.js

const fs = require('fs');
const path = require('path');

const ROOT = '.'; // Raiz do projeto
const REPORT = 'fix_report.txt';
const HEADER_EXT = ['.h', '.hpp'];
const SOURCE_EXT = ['.cpp', '.cc', '.cxx'];
const CODE_EXT = HEADER_EXT.concat(SOURCE_EXT);

let allFiles = [];
let allHeaders = [];
let allSources = [];
let includesMap = {};
let missingFiles = [];
let todos = [];
let stubs = [];
let notImplemented = [];
let brokenIncludes = [];
let missingFuncs = [];
let createdFiles = [];
let createdFuncs = [];
let uninitPointers = [];
let unusedVars = [];
let possibleLeaks = [];
let unusedFuncs = [];
let bigComments = [];
let asserts = [];

function walk(dir) {
  fs.readdirSync(dir).forEach(file => {
    const full = path.join(dir, file);
    if (fs.statSync(full).isDirectory()) {
      walk(full);
    } else {
      allFiles.push(full);
      if (HEADER_EXT.some(ext => file.endsWith(ext))) allHeaders.push(full);
      if (SOURCE_EXT.some(ext => file.endsWith(ext))) allSources.push(full);
    }
  });
}

function getAllIncludes(file) {
  const content = fs.readFileSync(file, 'utf8');
  const regex = /#include\s+["<]([^">]+)[">]/g;
  let match, result = [];
  while ((match = regex.exec(content)) !== null) {
    result.push(match[1]);
  }
  return result;
}

function checkMissingFiles() {
  let referenced = new Set();
  allFiles.forEach(file => {
    getAllIncludes(file).forEach(inc => referenced.add(inc));
  });
  referenced.forEach(inc => {
    if (!allFiles.some(f => f.endsWith(inc))) {
      missingFiles.push(inc);
    }
  });
}

function checkBrokenIncludes() {
  allFiles.forEach(file => {
    getAllIncludes(file).forEach(inc => {
      if (!allFiles.some(f => f.endsWith(inc))) {
        brokenIncludes.push(`${file}: ${inc}`);
      }
    });
  });
}

function findTodos() {
  allFiles.forEach(file => {
    const content = fs.readFileSync(file, 'utf8');
    const lines = content.split(/\r?\n/);
    lines.forEach((line, idx) => {
      if (/TODO|FIXME|NOT_IMPLEMENTED/i.test(line)) {
        todos.push(`${file}:${idx+1}: ${line.trim()}`);
      }
    });
  });
}

function findStubs() {
  allSources.forEach(file => {
    const content = fs.readFileSync(file, 'utf8');
    const regex = /(\w+::\w+\s*\([^)]*\)\s*\{)([^}]*)\}/g;
    let match;
    while ((match = regex.exec(content)) !== null) {
      const body = match[2].trim();
      if (body === '' || /^return\s+(true|false|0|nullptr);?$/.test(body)) {
        stubs.push(`${file}: função stub: ${match[1]} ... }`);
      }
    }
  });
}

function findNotImplementedFuncs() {
  // Para cada header, procurar funções não implementadas em .cpp
  allHeaders.forEach(header => {
    const content = fs.readFileSync(header, 'utf8');
    const regex = /(\w+)\s+(\w+)\s*\(([^)]*)\)\s*;/g;
    let match;
    while ((match = regex.exec(content)) !== null) {
      const ret = match[1], name = match[2], args = match[3];
      // Procurar implementação nos .cpp
      const implemented = allSources.some(src => {
        const srcContent = fs.readFileSync(src, 'utf8');
        const re = new RegExp(`${name}\\s*\\(`);
        return re.test(srcContent);
      });
      if (!implemented) {
        missingFuncs.push(`${header}: função não implementada: ${ret} ${name}(${args})`);
        createFuncStub(header, ret, name, args);
      }
    }
  });
}

function createFileIfMissing(filename, content) {
  if (!fs.existsSync(filename)) {
    fs.mkdirSync(path.dirname(filename), { recursive: true });
    fs.writeFileSync(filename, content, 'utf8');
    createdFiles.push(filename);
    console.log('Arquivo criado:', filename);
  }
}

function createHeaderStub(header) {
  const className = path.basename(header, path.extname(header));
  const guard = className.toUpperCase() + '_H';
  const content = `#ifndef ${guard}\n#define ${guard}\n\nclass ${className} {\npublic:\n    ${className}();\n    ~${className}();\n};\n\n#endif // ${guard}\n`;
  createFileIfMissing(header, content);
}

function createSourceStub(source, header) {
  const className = path.basename(header, path.extname(header));
  const content = `#include "${path.basename(header)}"\n\n${className}::${className}() {}\n${className}::~${className}() {}\n`;
  createFileIfMissing(source, content);
}

function createFuncStub(header, ret, name, args) {
  // Descobrir .cpp correspondente
  const dir = path.dirname(header);
  const className = path.basename(header, path.extname(header));
  let cpp = path.join(dir, className + '.cpp');
  if (!fs.existsSync(cpp)) {
    createSourceStub(cpp, header);
  }
  // Adicionar stub da função se não existir
  let src = fs.readFileSync(cpp, 'utf8');
  const funcSig = `${ret} ${className}::${name}(${args})`;
  if (!src.includes(funcSig)) {
    src += `\n${funcSig} {\n    // TODO: implementar\n}\n`;
    fs.writeFileSync(cpp, src, 'utf8');
    createdFuncs.push(`${cpp}: ${funcSig}`);
    console.log('Stub de função criado:', funcSig, 'em', cpp);
  }
}

function createMissingFiles() {
  missingFiles.forEach(f => {
    const ext = path.extname(f);
    if (HEADER_EXT.includes(ext)) {
      createHeaderStub(f);
    } else if (SOURCE_EXT.includes(ext)) {
      // Tenta criar .cpp com include do header correspondente
      const base = path.basename(f, ext);
      const dir = path.dirname(f);
      let header = path.join(dir, base + '.h');
      if (!fs.existsSync(header)) {
        createHeaderStub(header);
      }
      createSourceStub(f, header);
    }
  });
}

function findUninitPointers() {
  allFiles.forEach(file => {
    const content = fs.readFileSync(file, 'utf8');
    const lines = content.split(/\r?\n/);
    lines.forEach((line, idx) => {
      // Exemplo: Tipo* nome;
      if (/\w+\s*\*\s*\w+\s*;/.test(line) && !/\=/.test(line)) {
        uninitPointers.push(`${file}:${idx+1}: ${line.trim()}`);
      }
    });
  });
  console.log('Análise de ponteiros não inicializados concluída.');
}

function findUnusedVars() {
  // Busca simples: variáveis declaradas mas não usadas (pode gerar falso positivo)
  allFiles.forEach(file => {
    const content = fs.readFileSync(file, 'utf8');
    const varRegex = /\b(\w+)\s+(\w+)\s*(=|;)/g;
    let match;
    while ((match = varRegex.exec(content)) !== null) {
      const type = match[1], name = match[2];
      if (!['if','for','while','switch','return','delete','new','class','struct','typedef','public','private','protected','else','case','break','continue','goto','do','try','catch','throw','using','namespace','static','virtual','inline','const','volatile','friend','explicit','operator','template','typename','this','true','false','nullptr','NULL'].includes(type)) {
        // Se o nome não aparece mais no arquivo
        const rest = content.slice(varRegex.lastIndex);
        if (!new RegExp(`\\b${name}\\b`).test(rest)) {
          unusedVars.push(`${file}: variável possivelmente não usada: ${type} ${name}`);
        }
      }
    }
  });
  console.log('Análise de variáveis não usadas concluída.');
}

function findPossibleLeaks() {
  allFiles.forEach(file => {
    const content = fs.readFileSync(file, 'utf8');
    const lines = content.split(/\r?\n/);
    lines.forEach((line, idx) => {
      if (/new\s+\w+/.test(line) && !/std::unique_ptr|std::shared_ptr|delete|=\s*nullptr/.test(line)) {
        possibleLeaks.push(`${file}:${idx+1}: ${line.trim()}`);
      }
    });
  });
  console.log('Análise de possíveis memory leaks concluída.');
}

function findUnusedFuncs() {
  // Funções implementadas mas nunca chamadas (busca simples)
  let funcDefs = [];
  allSources.forEach(file => {
    const content = fs.readFileSync(file, 'utf8');
    const regex = /(\w+)\s+(\w+)::(\w+)\s*\(([^)]*)\)\s*\{/g;
    let match;
    while ((match = regex.exec(content)) !== null) {
      funcDefs.push({file, className: match[2], name: match[3]});
    }
  });
  funcDefs.forEach(f => {
    let used = false;
    allFiles.forEach(file => {
      const content = fs.readFileSync(file, 'utf8');
      if (new RegExp(`\\b${f.name}\\b`).test(content) && !file.endsWith('.h')) {
        used = true;
      }
    });
    if (!used) unusedFuncs.push(`${f.file}: função possivelmente não usada: ${f.className}::${f.name}`);
  });
  console.log('Análise de funções não usadas concluída.');
}

function findBigComments() {
  allFiles.forEach(file => {
    const content = fs.readFileSync(file, 'utf8');
    const regex = /\/\*([\s\S]*?)\*\//g;
    let match;
    while ((match = regex.exec(content)) !== null) {
      const lines = match[1].split(/\r?\n/).length;
      if (lines > 5) {
        bigComments.push(`${file}: bloco comentado grande (${lines} linhas)`);
      }
    }
  });
  console.log('Análise de blocos comentados grandes concluída.');
}

function findAsserts() {
  allFiles.forEach(file => {
    const content = fs.readFileSync(file, 'utf8');
    const lines = content.split(/\r?\n/);
    lines.forEach((line, idx) => {
      if (/\b(assert|abort|exit)\b/.test(line)) {
        asserts.push(`${file}:${idx+1}: ${line.trim()}`);
      }
    });
  });
  console.log('Análise de assert/abort/exit concluída.');
}

function main() {
  walk(ROOT);
  checkMissingFiles();
  checkBrokenIncludes();
  findTodos();
  findStubs();
  createMissingFiles();
  findNotImplementedFuncs();
  findUninitPointers();
  findUnusedVars();
  findPossibleLeaks();
  findUnusedFuncs();
  findBigComments();
  findAsserts();

  let report = [];
  report.push('==== RELATÓRIO DE PROBLEMAS WYDBR ====');
  report.push('\n== Arquivos referenciados mas ausentes ==');
  report = report.concat(missingFiles);
  report.push('\n== Includes quebrados ==');
  report = report.concat(brokenIncludes);
  report.push('\n== TODOs, FIXMEs, NOT_IMPLEMENTED ==');
  report = report.concat(todos);
  report.push('\n== Funções stub/vazias ==');
  report = report.concat(stubs);
  report.push('\n== Funções declaradas mas não implementadas ==');
  report = report.concat(missingFuncs);
  report.push('\n== Arquivos criados ==');
  report = report.concat(createdFiles);
  report.push('\n== Funções stub criadas ==');
  report = report.concat(createdFuncs);
  report.push('\n== Ponteiros não inicializados ==');
  report = report.concat(uninitPointers);
  report.push('\n== Variáveis possivelmente não usadas ==');
  report = report.concat(unusedVars);
  report.push('\n== Possíveis memory leaks (uso de new sem smart pointer/delete) ==');
  report = report.concat(possibleLeaks);
  report.push('\n== Funções possivelmente não usadas ==');
  report = report.concat(unusedFuncs);
  report.push('\n== Blocos comentados grandes ==');
  report = report.concat(bigComments);
  report.push('\n== Uso de assert/abort/exit ==');
  report = report.concat(asserts);

  fs.writeFileSync(REPORT, report.join('\n'), 'utf8');
  console.log('Relatório gerado em', REPORT);
}

main(); 
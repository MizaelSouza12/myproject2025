CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g -O2
LDFLAGS =
INCLUDES = -Iinclude -Iinclude/nlohmann

# Detectar o sistema operacional
ifeq ($(OS),Windows_NT)
	LDLIBS = -lws2_32
	EXEC_SUFFIX = .exe
	RM = del /Q
	MKDIR = mkdir
else
	LDLIBS =
	EXEC_SUFFIX = 
	RM = rm -f
	MKDIR = mkdir -p
endif

# Diretórios
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin
EXAMPLES_DIR = examples
DATA_DIR = data
DOCS_DIR = docs

# Arquivos fonte
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
EXAMPLES_SRC = $(wildcard $(EXAMPLES_DIR)/*.cpp)

# Objetos a compilar
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))
EXAMPLES_OBJ = $(patsubst $(EXAMPLES_DIR)/%.cpp,$(BUILD_DIR)/examples/%.o,$(EXAMPLES_SRC))

# Nome da biblioteca estática
LIB_NAME = libwydstudio.a

# Executáveis de exemplo
EXAMPLES_EXEC = $(patsubst $(EXAMPLES_DIR)/%.cpp,$(BIN_DIR)/%$(EXEC_SUFFIX),$(EXAMPLES_SRC))

# Regra padrão
all: directories lib examples

# Criar diretórios necessários
directories:
	$(MKDIR) $(BUILD_DIR)
	$(MKDIR) $(BUILD_DIR)/examples
	$(MKDIR) $(BIN_DIR)

# Compilação da biblioteca
lib: $(BUILD_DIR)/$(LIB_NAME)

$(BUILD_DIR)/$(LIB_NAME): $(OBJ_FILES)
	ar rcs $@ $^

# Compilação dos objetos da biblioteca
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compilação dos executáveis de exemplo
examples: $(EXAMPLES_EXEC)

$(BIN_DIR)/%$(EXEC_SUFFIX): $(BUILD_DIR)/examples/%.o $(BUILD_DIR)/$(LIB_NAME)
	$(CXX) $(LDFLAGS) $< -L$(BUILD_DIR) -lwydstudio $(LDLIBS) -o $@

# Compilação dos objetos dos exemplos
$(BUILD_DIR)/examples/%.o: $(EXAMPLES_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Limpeza
clean:
	$(RM) $(BUILD_DIR)/*.o $(BUILD_DIR)/examples/*.o $(BUILD_DIR)/$(LIB_NAME) $(EXAMPLES_EXEC)

# Executar todos os exemplos
run-examples: examples
	for exe in $(EXAMPLES_EXEC); do \
		echo "Running $$exe"; \
		./$$exe; \
		echo ""; \
	done

# Construção de exemplo específico
%.run: $(BIN_DIR)/%$(EXEC_SUFFIX)
	./$(BIN_DIR)/$*$(EXEC_SUFFIX)

# Teste do sistema de UI
ui-test:
	$(MAKE) -C ui_test

# Phony targets
.PHONY: all directories lib examples clean run-examples ui-test
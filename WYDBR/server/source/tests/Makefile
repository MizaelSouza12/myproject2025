# Makefile para o teste do sistema híbrido de habilidades

# Compilador
CXX = g++

# Flags de compilação
CXXFLAGS = -std=c++11 -Wall -I../include

# Diretórios
SRC_DIR = ../src
OBJ_DIR = ./obj
BIN_DIR = ./bin

# Arquivos
SRCS = test_hybrid_skill_system.cpp \
       $(SRC_DIR)/skill/skill_manager.cpp \
       $(SRC_DIR)/TM_Skill.cpp

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(SRCS)))

# Nome do executável
TARGET = $(BIN_DIR)/test_hybrid_skill

# Regra padrão
all: directories $(TARGET)

# Cria diretórios necessários
directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

# Compilação do executável
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regras para arquivos objeto
$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/skill/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Limpa os arquivos compilados
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Executa o teste
run: all
	@echo "Executando teste do sistema híbrido de habilidades..."
	@./$(TARGET)

.PHONY: all directories clean run

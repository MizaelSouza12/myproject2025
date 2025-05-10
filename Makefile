# Makefile para compilação rápida do WYDBR 2.0

# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2 -D_FORTIFY_SOURCE=2 -fstack-protector-strong
LDFLAGS = -lpthread -ldl

# Diretórios
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INCLUDE_DIR = include

# Arquivos
SERVER_SRCS = $(wildcard $(SRC_DIR)/server/*.cpp)
SERVER_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SERVER_SRCS))
SERVER_BIN = $(BIN_DIR)/TMSrv

STUDIO_SRCS = $(wildcard $(SRC_DIR)/admin/*.cpp)
STUDIO_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(STUDIO_SRCS))
STUDIO_BIN = $(BIN_DIR)/WYDStudio

COMMON_SRCS = $(wildcard $(SRC_DIR)/common/*.cpp)
COMMON_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(COMMON_SRCS))
COMMON_LIB = $(BIN_DIR)/libwydbr_common.a

# Alvos
all: directories $(SERVER_BIN) $(STUDIO_BIN)

directories:
        mkdir -p $(OBJ_DIR)/server $(OBJ_DIR)/admin $(OBJ_DIR)/common $(BIN_DIR)

$(COMMON_LIB): $(COMMON_OBJS)
        ar rcs $@ $^

$(SERVER_BIN): $(SERVER_OBJS) $(COMMON_LIB)
        $(CXX) -o $@ $^ $(LDFLAGS)

$(STUDIO_BIN): $(STUDIO_OBJS) $(COMMON_LIB)
        $(CXX) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
        mkdir -p $(dir $@)
        $(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
        rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all directories clean

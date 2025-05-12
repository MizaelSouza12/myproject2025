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
all: directories world_system combat_system anticheat_system

directories:
        mkdir -p $(OBJ_DIR)/server $(OBJ_DIR)/admin $(OBJ_DIR)/common $(BIN_DIR)
        mkdir -p $(OBJ_DIR)/world_system/core
        mkdir -p $(OBJ_DIR)/world_system/entities
        mkdir -p $(OBJ_DIR)/world_system/terrain
        mkdir -p $(OBJ_DIR)/world_system/physics
        mkdir -p $(OBJ_DIR)/world_system/weather
        mkdir -p $(OBJ_DIR)/combat_system/core
        mkdir -p $(OBJ_DIR)/combat_system/abilities
        mkdir -p $(OBJ_DIR)/combat_system/damage
        mkdir -p $(OBJ_DIR)/combat_system/effects
        mkdir -p $(OBJ_DIR)/combat_system/targeting
        mkdir -p $(OBJ_DIR)/anticheat_system/core
        mkdir -p $(OBJ_DIR)/anticheat_system/detection
        mkdir -p $(OBJ_DIR)/anticheat_system/prevention
        mkdir -p $(OBJ_DIR)/anticheat_system/validation
        mkdir -p $(OBJ_DIR)/anticheat_system/reporting

$(COMMON_LIB): $(COMMON_OBJS)
        ar rcs $@ $^

$(SERVER_BIN): $(SERVER_OBJS) $(COMMON_LIB)
        $(CXX) -o $@ $^ $(LDFLAGS)

$(STUDIO_BIN): $(STUDIO_OBJS) $(COMMON_LIB)
        $(CXX) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
        mkdir -p $(dir $@)
        $(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Sistema de Mundo
WORLD_SYSTEM_SOURCES = $(wildcard $(SRC_DIR)/world_system/core/*.cpp) \
                       $(wildcard $(SRC_DIR)/world_system/entities/*.cpp) \
                       $(wildcard $(SRC_DIR)/world_system/terrain/*.cpp) \
                       $(wildcard $(SRC_DIR)/world_system/physics/*.cpp) \
                       $(wildcard $(SRC_DIR)/world_system/weather/*.cpp)

WORLD_SYSTEM_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(WORLD_SYSTEM_SOURCES))

world_system: $(WORLD_SYSTEM_OBJECTS)
        $(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/world_system

# Sistema de Combate
COMBAT_SYSTEM_SOURCES = $(wildcard $(SRC_DIR)/combat_system/core/*.cpp) \
                        $(wildcard $(SRC_DIR)/combat_system/abilities/*.cpp) \
                        $(wildcard $(SRC_DIR)/combat_system/damage/*.cpp) \
                        $(wildcard $(SRC_DIR)/combat_system/effects/*.cpp) \
                        $(wildcard $(SRC_DIR)/combat_system/targeting/*.cpp)

COMBAT_SYSTEM_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(COMBAT_SYSTEM_SOURCES))

combat_system: $(COMBAT_SYSTEM_OBJECTS)
        $(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/combat_system

# Sistema Anti-Cheat
ANTICHEAT_SYSTEM_SOURCES = $(wildcard $(SRC_DIR)/anticheat_system/core/*.cpp) \
                          $(wildcard $(SRC_DIR)/anticheat_system/detection/*.cpp) \
                          $(wildcard $(SRC_DIR)/anticheat_system/prevention/*.cpp) \
                          $(wildcard $(SRC_DIR)/anticheat_system/validation/*.cpp) \
                          $(wildcard $(SRC_DIR)/anticheat_system/reporting/*.cpp)

ANTICHEAT_SYSTEM_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(ANTICHEAT_SYSTEM_SOURCES))

anticheat_system: $(ANTICHEAT_SYSTEM_OBJECTS)
        $(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/anticheat_system

clean:
        rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all directories world_system combat_system anticheat_system clean

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = 
LDFLAGS =
LDLIBS =

# Detectar o sistema operacional
ifeq ($(OS),Windows_NT)
	EXECUTABLE = UITestApp.exe
	RM = del /Q
else
	EXECUTABLE = UITestApp
	RM = rm -f
endif

# Arquivos fonte e objetos
SOURCES = UITestApp.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Regra padrão
all: $(EXECUTABLE)

# Compilação do executável
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@ $(LDLIBS)

# Regra para compilar arquivos .cpp em .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Limpar arquivos gerados
clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)

# Executar o teste
run: $(EXECUTABLE)
	./$(EXECUTABLE)

# Phony targets
.PHONY: all clean run
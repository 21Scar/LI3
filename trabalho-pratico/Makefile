# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-parameter -O3 $(shell pkg-config --cflags glib-2.0)
LDFLAGS = $(shell pkg-config --libs glib-2.0)

# Diretórios
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

# Lista dos arquivos .c
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/voos.c \
          $(SRC_DIR)/voosCatalog.c \
          $(SRC_DIR)/aeroportos.c \
          $(SRC_DIR)/aeroportosCatalog.c \
          $(SRC_DIR)/reservas.c \
          $(SRC_DIR)/reservasCatalog.c \
          $(SRC_DIR)/passageiros.c \
          $(SRC_DIR)/passageirosCatalog.c \
          $(SRC_DIR)/aeronaves.c \
          $(SRC_DIR)/aeronavesCatalog.c \
          $(SRC_DIR)/querys.c \
          $(SRC_DIR)/parserGenerico.c \
          $(SRC_DIR)/validation.c



# Objetos correspondentes aos arquivos .c
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

# Nome do executável
EXECUTABLE = programa-principal

# Executável interativo (usa todas as fontes excepto main.c, e acrescenta programa-interativo.c)
SOURCES_INTERACTIVE = $(filter-out $(SRC_DIR)/main.c, $(SOURCES)) $(SRC_DIR)/programa-interativo.c
OBJECTS_INTERACTIVE = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES_INTERACTIVE))
# Criar objeto de main.c com função renomeada para evitar conflito
OBJ_MAIN_FUNCTIONS = $(OBJ_DIR)/main_functions.o
OBJECTS_INTERACTIVE_WITH_MAIN = $(sort $(OBJECTS_INTERACTIVE) $(OBJ_MAIN_FUNCTIONS))
EXECUTABLE_INTERACTIVE = programa-interativo

# Regra padrão
all: $(EXECUTABLE) $(EXECUTABLE_INTERACTIVE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

$(EXECUTABLE_INTERACTIVE): $(OBJECTS_INTERACTIVE_WITH_MAIN)
	$(CC) $(CFLAGS) $(OBJECTS_INTERACTIVE_WITH_MAIN) -o $@ $(LDFLAGS)

$(OBJ_MAIN_FUNCTIONS): $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE_DIR) -Dmain=main_original_unused

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE_DIR)


# Remove arquivos .o e o executável
clean:
	rm -rf $(OBJ_DIR) $(EXECUTABLE) $(EXECUTABLE_INTERACTIVE)
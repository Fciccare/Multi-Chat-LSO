CC = gcc # Compilatore da usare
CFLAGS = -Wall -Wextra -Wshadow -O2 -g -pthread -fstack-clash-protection -fstack-protector-all -DLOG_USE_COLOR
LDFLAGS = -lm -ldl # flags per il linker

BIN_PATH := bin
SRC_PATH := src

SRC_EXT := c

SRC = src/server.c src/objects/*.c src/handler/*.c src/database/*.c src/database/library/sqlite3.c src/library/log.c

TARGET := $(BIN_PATH)/server

all: $(TARGET)
	@printf "\e[32m%b\e[0m" "Build completato\n"

# $@ indica il target (parte a sinistra dei :)
# $^ sta per "tutti gli elementi alla destra di :" (in questo caso $(SRC))
$(TARGET): $(SRC)
	@printf "\e[32m%b\e[0m" "Compilazione sorgenti\n"
	@mkdir -p $(BIN_PATH)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 

.PHONY: clean all

clean:
	@printf "\e[32m%b\e[0m" "Eliminazione eseguibile\n"
	rm -f $(TARGET)

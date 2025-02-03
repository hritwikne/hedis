# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g  # Enables warnings and debugging info

# Target executable
TARGET = bin/hedis-cli

# Source and object files
SRC = src/client.c src/conn_handler.c src/main.c src/msg_handler.c src/resp.c
OBJ = $(SRC:src/%.c=obj/%.o)

# Build the target
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Rule to build .o files from .c files
obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(OBJ) $(TARGET)

# Declare clean as a phony target to avoid conflicts with files named 'clean'
.PHONY: clean
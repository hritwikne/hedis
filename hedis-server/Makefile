# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g  # Enables warnings and debugging info
LDFLAGS = -lpthread        # Link with pthread library

# Target executable
TARGET = bin/hedis-server

# Source and object files
SRC = src/commands.c src/conn_handler.c src/context.c src/event_loop.c src/main.c src/msg_handler.c src/resp.c src/server.c src/sig_handler.c utils/hash_table.c utils/mem_utils.c utils/priority_queue.c utils/socket.c utils/utility.c
OBJ = $(SRC:src/%.c=obj/%.o)

# Build the target
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

# Rule to build .o files from .c files
obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(OBJ) $(TARGET)

# Declare clean as a phony target to avoid conflicts with files named 'clean'
.PHONY: clean
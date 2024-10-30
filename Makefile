# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g  # Enables warnings and debugging info
LDFLAGS = -lpthread        # Link with pthread library

# Target executable
TARGET = bin/single_threaded_1

# Source and object files
SRC = src/main.c src/server.c utils/socket.c src/event_loop.c src/conn_handler.c src/msg_handler.c src/sig_handler.c
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
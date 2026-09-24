# Compiler and flags
CC = gcc
CFLAGS = -Wall -ansi -pedantic -g

# --- Directories ---
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
TEST_DIR = tests

# Add the include directory to the compiler's search path
CFLAGS += -I$(INC_DIR)

# The final executable name
TARGET = assembler

# Find all .c files in the source directory
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Create a list of object files in the object directory
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

# The default rule
all: $(TARGET)

# Linking rule: Build the final executable from all object files
$(TARGET): $(OBJS)
	@echo "Linking..."
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	@echo "Build complete. Executable is '$(TARGET)'"

# Compilation rule: Build an object file in obj/ from a source file in src/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to create the object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# REVISED Cleanup rule: Now cleans both the root and the tests directory
clean:
	@echo "Cleaning project..."
	@echo "--> Removing executable '$(TARGET)'"
	rm -f $(TARGET)
	@echo "--> Removing object files from '$(OBJ_DIR)/'"
	rm -f $(OBJ_DIR)/*.o
	@echo "--> Removing output files from the tests directory..."
	rm -f $(TEST_DIR)/*.am $(TEST_DIR)/*.ob $(TEST_DIR)/*.ent $(TEST_DIR)/*.ext
	@echo "Cleanup complete."

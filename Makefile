# Makefile

NAME = Shubham_Mali

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3 -I./src


# --- Directories ---
SRC_DIR = src
BUILD_DIR = build
TARGET = reconstruction_$(NAME)

# --- Files ---
# Find all .cpp files in the source directory
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
# Create object file names in the build directory
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

# --- Rules ---

# Default rule: build the executable
all: $(TARGET)

# Rule to link the final executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Rule to compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR) # Create build directory if it doesn't exist
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean up build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Phony targets are not files
.PHONY: all clean
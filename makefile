# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -g 
LDFLAGS := -L/ucrt64/lib -lSDL2 -lSDL2main -lSDL2_mixer

# Folders
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin

# Source files
SRCS := $(SRC_DIR)/main.cpp $(SRC_DIR)/songLoader.cpp $(SRC_DIR)/utils.cpp $(SRC_DIR)/user.cpp $(SRC_DIR)/playlist.cpp
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
TARGET := $(BIN_DIR)/osuListener.exe

all:
	@echo "Help Make"
	@echo "make win -> Compile for Windows"
	@echo "make lin -> Compile for Linux"
	@echo "clean or distclean -> Destroy bin / build folder"

# Build for Windows
win: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

lin: $(TARGET)

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

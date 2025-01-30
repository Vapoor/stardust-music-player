CXX = g++
CXXFLAGS = -Wall -g
RM = rm -rf

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/songLoader.cpp
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
TARGET = $(BIN_DIR)/osuListener

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	$(RM) $(BUILD_DIR) $(BIN_DIR)

distclean: clean
	$(RM) *.swp

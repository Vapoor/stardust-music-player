# Compiler and flags - detect compiler
ifdef MSVC
    # Microsoft Visual C++ Compiler
    CXX = cl
    CXXFLAGS = /std:c++17 /EHsc /O2 /nologo
    DEBUG_FLAGS = /Zi /DDEBUG
    OBJ_EXT = obj
    EXE_EXT = .exe
    LIB_PREFIX = 
    LIB_EXT = .lib
    COMPILE_FLAG = /c
    OUTPUT_FLAG = /Fo:
else
    # GCC/MinGW
    CXX = g++
    CXXFLAGS = -std=c++17 -Wall -Wextra -O2
    DEBUG_FLAGS = -g -DDEBUG
    OBJ_EXT = o
    EXE_EXT = 
    LIB_PREFIX = lib
    LIB_EXT = .a
    COMPILE_FLAG = -c
    OUTPUT_FLAG = -o
endif

# Directories
SRC_DIR = src
HEADERS_DIR = headers
BUILD_DIR = build
BIN_DIR = bin

# FMOD configuration
FMOD_DIR = lib/FMOD\ Studio\ API\ Windows
FMOD_INC = $(FMOD_DIR)/api/core/inc
FMOD_LIB = $(FMOD_DIR)/api/core/lib/x64

# Include directories
ifdef MSVC
    INCLUDES = /I"$(HEADERS_DIR)"
else
    INCLUDES = -I$(HEADERS_DIR)
endif

# Check if FMOD exists and configure accordingly
ifneq ($(wildcard $(FMOD_INC)/fmod.h),)
    ifdef MSVC
        # MSVC configuration
        INCLUDES += /I"$(FMOD_INC)"
        LDFLAGS += /LIBPATH:"$(FMOD_LIB)" fmod_vc.lib
        CXXFLAGS += /DFMOD_AVAILABLE
    else
        # MinGW configuration - create import library if needed
        INCLUDES += -I$(FMOD_INC)
        
        # Check if import library exists, create if not
        ifeq ($(wildcard $(FMOD_LIB)/libfmod.dll.a),)
            $(info Creating MinGW import library for FMOD...)
            $(shell cd "$(FMOD_LIB)" && gendef fmod.dll 2>/dev/null)
            $(shell cd "$(FMOD_LIB)" && dlltool -D fmod.dll -d fmod.def -l libfmod.dll.a 2>/dev/null)
        endif
        
        # Link with the import library
        LDFLAGS += -L"$(FMOD_LIB)" -lfmod
        CXXFLAGS += -DFMOD_AVAILABLE
    endif
    FMOD_FOUND = true
    $(info FMOD found at $(FMOD_DIR) - Audio playback enabled)
else
    FMOD_FOUND = false
    $(warning FMOD not found at $(FMOD_DIR) - Audio will be simulated)
endif

# Source files and objects
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
ifdef MSVC
    OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.$(OBJ_EXT))
    TARGET = $(BIN_DIR)/osuListener$(EXE_EXT)
else
    OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.$(OBJ_EXT))
    TARGET = $(BIN_DIR)/osuListener$(EXE_EXT)
endif

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
ifdef MSVC
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
else
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
endif

# Main target
$(TARGET): directories $(OBJECTS)
ifdef MSVC
	$(CXX) $(OBJECTS) /Fe:$@ $(LDFLAGS)
else
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
endif
	@echo "Build complete: $@"
ifeq ($(FMOD_FOUND),true)
	@echo "Copying fmod.dll to bin directory..."
ifdef MSVC
	@if exist "$(FMOD_LIB)\fmod.dll" copy "$(FMOD_LIB)\fmod.dll" "$(BIN_DIR)\" >nul 2>&1
else
	@if [ -f "$(FMOD_LIB)/fmod.dll" ]; then \
		cp "$(FMOD_LIB)/fmod.dll" "$(BIN_DIR)/"; \
		echo "fmod.dll copied to bin directory"; \
	fi
endif
endif

# Compile source files
ifdef MSVC
$(BUILD_DIR)/%.$(OBJ_EXT): $(SRC_DIR)/%.cpp | directories
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(COMPILE_FLAG) $< $(OUTPUT_FLAG)$@
else
$(BUILD_DIR)/%.$(OBJ_EXT): $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(COMPILE_FLAG) $< $(OUTPUT_FLAG) $@
endif

# Debug build
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: directories $(TARGET)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)
	@echo "Clean complete"

# Clean and rebuild
rebuild: clean all

# Install FMOD DLL (Windows)
install-fmod:
ifeq ($(FMOD_FOUND),true)
	@if [ -f "$(FMOD_LIB)/fmod.dll" ]; then \
		cp "$(FMOD_LIB)/fmod.dll" "$(BIN_DIR)/"; \
		echo "fmod.dll installed to bin directory"; \
	else \
		echo "fmod.dll not found in $(FMOD_LIB)"; \
	fi
else
	@echo "FMOD not found - cannot install DLL"
endif

# Run the program
run: $(TARGET)
	cd $(BIN_DIR) && ./osuListener

# Show help
help:
	@echo "Available targets:"
	@echo "  all (default) - Build the project"
	@echo "  debug         - Build with debug symbols"
	@echo "  clean         - Remove build files"
	@echo "  rebuild       - Clean and build"
	@echo "  run           - Build and run the program"
	@echo "  install-fmod  - Copy FMOD DLL to bin directory"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "FMOD Status: $(if $(FMOD_FOUND),Found at $(FMOD_DIR),Not found)"

# Show current configuration
config:
	@echo "Build Configuration:"
	@echo "  Compiler: $(CXX)"
	@echo "  Flags: $(CXXFLAGS)"
	@echo "  Includes: $(INCLUDES)"
	@echo "  FMOD: $(if $(FMOD_FOUND),Enabled at $(FMOD_DIR),Disabled)"
	@echo "  Sources: $(SOURCES)"
	@echo "  Target: $(TARGET)"

# Phony targets
.PHONY: all clean rebuild debug run help config directories install-fmod

# Automatic dependency generation (optional advanced feature)
-include $(OBJECTS:.o=.d)

$(BUILD_DIR)/%.d: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MM -MT $(BUILD_DIR)/$*.o $< > $@
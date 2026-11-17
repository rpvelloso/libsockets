###############################################################################
# Makefile for ftpd — Linux implementation
#
# Usage:
#   make              Build ftpd executable
#   make clean        Remove build artifacts
###############################################################################

# Compiler
CXX      := g++

# C++ standard and flags
CXXFLAGS := -std=c++17 -Wall -Wextra -fPIC -pthread

# OpenSSL flags
OPENSSL_CXXFLAGS := $(shell pkg-config --cflags openssl 2>/dev/null || echo "")
OPENSSL_LDFLAGS  := $(shell pkg-config --libs openssl 2>/dev/null || echo "-lssl -lcrypto")

# Directories (relative to ftpd/)
SRC_DIR    := src
LINUX_DIR  := linux
GETOPT_DIR := ../getopt
LIBSOCKETS_DIR := ../libsockets
BUILD_DIR  := build

# Include paths
INCLUDES := -I$(SRC_DIR) -I$(LINUX_DIR) -I$(GETOPT_DIR) \
            -I$(LIBSOCKETS_DIR) -I$(LIBSOCKETS_DIR)/src -I$(LIBSOCKETS_DIR)/linux -I$(LIBSOCKETS_DIR)/linux/include

# ---------------------------------------------------------------------------
# Source files
# ---------------------------------------------------------------------------

# Core FTP sources (platform-independent)
CORE_SRCS := \
    $(SRC_DIR)/FTPClient.cpp \
    $(SRC_DIR)/FTPClientInfo.cpp \
    $(SRC_DIR)/FTPServer.cpp \
    $(SRC_DIR)/auth/Authentication.cpp \
    $(SRC_DIR)/auth/AuthenticationImpl.cpp \
    $(SRC_DIR)/filesystem/FileSystem.cpp \
    $(SRC_DIR)/filesystem/FileSystemImpl.cpp \
    $(SRC_DIR)/state/FTPClientState.cpp \
    $(SRC_DIR)/state/FTPClientConnected.cpp \
    $(SRC_DIR)/state/FTPClientLoggedIn.cpp \
    $(SRC_DIR)/state/FTPClientRename.cpp \
    $(SRC_DIR)/state/FTPClientTransfer.cpp

# Linux-specific sources
LINUX_SRCS := \
    $(LINUX_DIR)/auth/LinuxAuthentication.cpp \
    $(LINUX_DIR)/filesystem/LinuxFileSystem.cpp

# Main
MAIN_SRC := $(SRC_DIR)/main.cpp

# ---------------------------------------------------------------------------
# Object files
# ---------------------------------------------------------------------------

CORE_OBJS  := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/src/%.o,$(CORE_SRCS))
LINUX_OBJS := $(patsubst $(LINUX_DIR)/%.cpp,$(BUILD_DIR)/linux/%.o,$(LINUX_SRCS))
MAIN_OBJ   := $(BUILD_DIR)/src/main.o

# ---------------------------------------------------------------------------
# Targets
# ---------------------------------------------------------------------------

BIN_TARGET := $(BUILD_DIR)/ftpd

# Default target
all: $(BIN_TARGET)

# Executable
$(BIN_TARGET): $(CORE_OBJS) $(LINUX_OBJS) $(MAIN_OBJ) $(LIBSOCKETS_DIR)/build/libsockets-ssl.so
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(OPENSSL_CXXFLAGS) $^ -o $@ -pthread -lpam $(OPENSSL_LDFLAGS) -Wl,-rpath,'$$ORIGIN'

# ---------------------------------------------------------------------------
# Compilation rules
# ---------------------------------------------------------------------------

$(BUILD_DIR)/src/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/linux/%.o: $(LINUX_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ---------------------------------------------------------------------------
# Clean
# ---------------------------------------------------------------------------

clean:
	rm -rf $(BUILD_DIR)

# ---------------------------------------------------------------------------
# Phony targets
# ---------------------------------------------------------------------------

.PHONY: all clean

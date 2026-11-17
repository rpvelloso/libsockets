###############################################################################
# Makefile for libsockets — Linux implementation
#
# Usage:
#   make              Build shared library (no SSL)
#   make SSL=1        Build shared library with OpenSSL support
#   make netcat       Build netcat executable
#   make luaserver    Build LuaServer executable (requires Lua + OpenSSL)
#   make clean        Remove build artifacts
###############################################################################

# Compiler
CXX      := g++

# C++ standard and flags
CXXFLAGS := -std=c++17 -Wall -Wextra -fPIC -pthread

# Directories (relative to libsockets/)
SRC_DIR    := src
LINUX_DIR  := linux
GETOPT_DIR := ../getopt
BUILD_DIR  := build

# Include paths
INCLUDES := -I$(SRC_DIR) -I$(LINUX_DIR) -I$(LINUX_DIR)/include -I$(GETOPT_DIR)

# ---------------------------------------------------------------------------
# Optional SSL support  (set SSL=1 on the command line)
# ---------------------------------------------------------------------------
SSL ?= 0
ifeq ($(SSL),1)
  CXXFLAGS += $(shell pkg-config --cflags openssl 2>/dev/null || echo "")
  LDFLAGS  += $(shell pkg-config --libs openssl 2>/dev/null || echo "-lssl -lcrypto")
endif

# ---------------------------------------------------------------------------
# Source files
# ---------------------------------------------------------------------------

# Core library sources (platform-independent)
CORE_SRCS := \
    $(SRC_DIR)/Factory/SocketFactory.cpp \
    $(SRC_DIR)/Socket/Socket.cpp \
    $(SRC_DIR)/Socket/ClientSocket.cpp \
    $(SRC_DIR)/Socket/ServerSocket.cpp \
    $(SRC_DIR)/Socket/DatagramSocket.cpp \
    $(SRC_DIR)/Socket/SocketAddress.cpp \
    $(SRC_DIR)/Socket/SocketStream.cpp \
    $(SRC_DIR)/Socket/SocketState/SocketState.cpp \
    $(SRC_DIR)/Socket/SocketState/ClosedState.cpp \
    $(SRC_DIR)/Socket/SocketState/ConnectedState.cpp \
    $(SRC_DIR)/Socket/SocketState/DisconnectedState.cpp \
    $(SRC_DIR)/Socket/SocketState/ListeningState.cpp \
    $(SRC_DIR)/Multiplexer/Multiplexer.cpp \
    $(SRC_DIR)/Multiplexer/MultiplexerImpl.cpp \
    $(SRC_DIR)/ConnectionPool/ConnectionPool.cpp \
    $(SRC_DIR)/ConnectionPool/MultiplexedConnectionPoolImpl.cpp \
    $(SRC_DIR)/ConnectionPool/ThreadedConnectionPoolImpl.cpp \
    $(SRC_DIR)/Server/Server.cpp

# Linux-specific sources
LINUX_SRCS := \
    $(LINUX_DIR)/Factory/LinuxFDFactory.cpp \
    $(LINUX_DIR)/Factory/LinuxSocketFactory.cpp \
    $(LINUX_DIR)/Multiplexer/LinuxPoll.cpp \
    $(LINUX_DIR)/Multiplexer/LinuxSelect.cpp \
    $(LINUX_DIR)/Socket/LinuxSocket.cpp \
    $(LINUX_DIR)/Socket/LinuxSocketAddress.cpp

# SSL sources (included only when SSL=1)
SSL_SRCS := \
    $(SRC_DIR)/Socket/SSL/OpenSSL.cpp \
    $(SRC_DIR)/Socket/SSL/OpenSSLSocket.cpp

# ---------------------------------------------------------------------------
# Object files
# ---------------------------------------------------------------------------

CORE_OBJS  := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/src/%.o,$(CORE_SRCS))
LINUX_OBJS := $(patsubst $(LINUX_DIR)/%.cpp,$(BUILD_DIR)/linux/%.o,$(LINUX_SRCS))
SSL_OBJS   := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/src/%.o,$(SSL_SRCS))

# Library object set (core + linux + optional ssl)
LIB_OBJS := $(CORE_OBJS) $(LINUX_OBJS)
ifeq ($(SSL),1)
  LIB_OBJS += $(SSL_OBJS)
endif

# ---------------------------------------------------------------------------
# Targets
# ---------------------------------------------------------------------------

ifeq ($(SSL),1)
  LIB_TARGET := $(BUILD_DIR)/libsockets-ssl.so
else
  LIB_TARGET := $(BUILD_DIR)/libsockets.so
endif

NETCAT_BIN  := $(BUILD_DIR)/netcat
LUASERVER_BIN := $(BUILD_DIR)/luaserver

# Default target
all: $(LIB_TARGET)

# Shared library
$(LIB_TARGET): $(LIB_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) -shared -o $@ $^ $(LDFLAGS)

# Netcat executable
$(NETCAT_BIN): $(BUILD_DIR)/libsockets.so $(BUILD_DIR)/nc.o
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(BUILD_DIR)/nc.o -L$(BUILD_DIR) -lsockets -o $@ -pthread -Wl,-rpath,'$$ORIGIN'

# LuaServer executable (requires Lua + OpenSSL)
$(LUASERVER_BIN): $(BUILD_DIR)/libsockets-ssl.so $(BUILD_DIR)/LuaServer.o
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(BUILD_DIR)/LuaServer.o -L$(BUILD_DIR) -lsockets-ssl -o $@ -pthread \
	    $(shell pkg-config --libs openssl lua5.4 2>/dev/null || echo "-lssl -lcrypto -llua5.4") -Wl,-rpath,'$$ORIGIN'

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
# Convenience targets
# ---------------------------------------------------------------------------

lib: $(LIB_TARGET)

lib-ssl:
	$(MAKE) SSL=1 $(BUILD_DIR)/libsockets-ssl.so

netcat: $(NETCAT_BIN)

luaserver: $(LUASERVER_BIN)

# ---------------------------------------------------------------------------
# Clean
# ---------------------------------------------------------------------------

clean:
	rm -rf $(BUILD_DIR)

# ---------------------------------------------------------------------------
# Phony targets
# ---------------------------------------------------------------------------

.PHONY: all lib lib-ssl netcat luaserver clean

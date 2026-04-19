# Compiler and flags
CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
CFLAGS ?= -Wall -Werror -g
CXXFLAGS ?= -Wall -Werror -g
CPPFLAGS += -MMD -MP
LDFLAGS ?=

# Targets
TARGET = aesdsocket
TEST_TARGET = unit_tests

# Source files
APP_SRC = aesdsocket.c
APP_OBJ = $(APP_SRC:.c=.o)

LIB_SRC = socket_manager.c data_store.c signal_handler.c daemonizer.c
LIB_OBJ = $(LIB_SRC:.c=.o)

OBJ = $(APP_OBJ) $(LIB_OBJ)
DEPS = $(OBJ:.o=.d)

# Test source files
TEST_DIR = test
TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)
TEST_DEPS = $(TEST_SRCS:.cpp=.d)

# CppUTest configuration
CPPUTEST_PKG = cpputest
CPPFLAGS += $(shell pkg-config --cflags $(CPPUTEST_PKG) 2>/dev/null || echo -I/usr/local/include)
TEST_LDFLAGS = $(shell pkg-config --libs $(CPPUTEST_PKG) 2>/dev/null || echo -L/usr/local/lib -lCppUTest -lCppUTestExt)

# Default target
.PHONY: all default
all: $(TARGET)
default: $(TARGET)

# Compile main application
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Test target
.PHONY: test
test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS) $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $^ -o $@ $(TEST_LDFLAGS)

$(TEST_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Include dependencies
-include $(DEPS) $(TEST_DEPS)

# Clean target
.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJ) $(DEPS) $(TEST_TARGET) $(TEST_OBJS) $(TEST_DEPS)

### Simple Makefile for Game of Life (SFML)

# Compiler and flags
CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic

# Optional: in rare old GCC setups, set STD_FS_LIB=-lstdc++fs
STD_FS_LIB ?=

# Try to use pkg-config first
PKG_CONFIG ?= pkg-config
SFML_PKG ?= sfml-graphics
SFML_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(SFML_PKG) 2>/dev/null)
SFML_LIBS   := $(shell $(PKG_CONFIG) --libs   $(SFML_PKG) 2>/dev/null)

# If pkg-config is unavailable or returns nothing, fall back to Homebrew paths
INCLUDES ?=
LDFLAGS  ?=
LIBS     ?=

ifeq ($(strip $(SFML_CFLAGS)),)
  # Homebrew default install prefixes on Apple Silicon
  INCLUDES += -I/opt/homebrew/include
  LDFLAGS  += -L/opt/homebrew/lib
  LIBS     += -lsfml-graphics -lsfml-window -lsfml-system
else
  CXXFLAGS += $(SFML_CFLAGS)
  LIBS     += $(SFML_LIBS)
endif

TARGET := main
SRC    := main.cpp

.PHONY: all clean run gui test console diagnose

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@ $(LDFLAGS) $(LIBS) $(STD_FS_LIB)

# Run default (GUI) mode
run: $(TARGET)
	./$(TARGET)

# Run GUI with custom args, e.g.:
#   make gui GUI_ARGS="--step 0.1 input.txt"
GUI_ARGS ?=
gui: $(TARGET)
	./$(TARGET) $(GUI_ARGS)

# Run unit tests
test: $(TARGET)
	./$(TARGET) --test

# Run console mode, provide ITER and FILE
# Example: make console ITER=10 FILE=state.txt
console: $(TARGET)
	@if [ -z "$(ITER)" ] || [ -z "$(FILE)" ]; then \
	  echo "Usage: make console ITER=<n> FILE=<input.txt>"; \
	  exit 1; \
	fi; \
	./$(TARGET) --console $(ITER) $(FILE)

diagnose:
	@echo "CXX=$(CXX)"
	@echo "CXXFLAGS=$(CXXFLAGS)"
	@echo "INCLUDES=$(INCLUDES)"
	@echo "LDFLAGS=$(LDFLAGS)"
	@echo "LIBS=$(LIBS)"
	@echo "SFML_CFLAGS=$(SFML_CFLAGS)"
	@echo "SFML_LIBS=$(SFML_LIBS)"

clean:
	rm -f $(TARGET) *.o


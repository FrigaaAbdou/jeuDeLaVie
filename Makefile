CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I.
DEPFLAGS := -MMD -MP
LDFLAGS :=

SFML_CFLAGS := $(shell pkg-config --cflags sfml-graphics)
SFML_LIBS := $(shell pkg-config --libs sfml-graphics)

SRC := main.cpp \
       ConsoleRunner.cpp \
       GraphicRunner.cpp \
       Grid.cpp \
       GridExporter.cpp \
       InitialStateLoader.cpp \
       GameOfLife.cpp

OBJ := $(SRC:.cpp=.o)
DEP := $(OBJ:.o=.d)

all: jeu

jeu: $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) $(SFML_LIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) $(SFML_CFLAGS) -c $< -o $@

-include $(DEP)

.PHONY: clean
clean:
	rm -f $(OBJ) $(DEP) jeu

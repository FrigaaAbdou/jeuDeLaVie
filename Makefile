CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I.
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

all: jeu

jeu: $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) $(SFML_LIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SFML_CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJ) jeu

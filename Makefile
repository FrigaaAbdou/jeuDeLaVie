CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I. -Idomain -Iapplication -Iinfrastructure -Iui
DEPFLAGS := -MMD -MP
LDFLAGS :=

SFML_CFLAGS := $(shell pkg-config --cflags sfml-graphics)
SFML_LIBS := $(shell pkg-config --libs sfml-graphics)

SRC := ui/main.cpp \
       ui/ConsoleRunner.cpp \
       ui/GraphicRunner.cpp \
       domain/Grid.cpp \
       infrastructure/GridExporter.cpp \
       infrastructure/InitialStateLoader.cpp \
       domain/GameOfLife.cpp \
       application/SimulationService.cpp

OBJ := $(SRC:.cpp=.o)
DEP := $(OBJ:.o=.d)

all: jeu

test: tests/test_runner

tests/test_runner: tests/main.cpp domain/Grid.cpp domain/GameOfLife.cpp infrastructure/InitialStateLoader.cpp infrastructure/GridExporter.cpp
	$(CXX) $(CXXFLAGS) -I. $^ -o $@

jeu: $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) $(SFML_LIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) $(SFML_CFLAGS) -c $< -o $@

-include $(DEP)

.PHONY: clean
clean:
	rm -f $(OBJ) $(DEP) jeu

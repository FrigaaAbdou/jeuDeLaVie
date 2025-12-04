#pragma once
#include <string>

struct SimulationConfig {
    std::string inputFile;
    std::string outputBaseName;
    int maxIterations = 50;
    bool toroidal = false;
    bool graphicMode = false;
};

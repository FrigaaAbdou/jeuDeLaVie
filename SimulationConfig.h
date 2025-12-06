#pragma once
#include <string>

struct SimulationConfig {
    std::string inputFile;
    std::string outputBaseName;
    int maxIterations = 100;
    bool toroidal = false;
    bool graphicMode = false;
};

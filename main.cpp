#include "SimulationConfig.h"
#include "ConsoleRunner.h"
#include "GraphicRunner.h"
#include <iostream>

int main(int argc, char** argv) {
    SimulationConfig config;

    if (argc < 3) {
        std::cout << "Usage:\n"
                  << "  " << argv[0] << " console <input_file> <baseName> [maxIter]\n"
                  << "  " << argv[0] << " graphic <input_file> [maxIter]\n";
        return 0;
    }

    std::string mode = argv[1];
    config.inputFile = argv[2];

    if (mode == "console") {
        config.graphicMode = false;
        if (argc >= 4) config.outputBaseName = argv[3];
        else config.outputBaseName = "output";

        if (argc >= 5) config.maxIterations = std::stoi(argv[4]);

        ConsoleRunner::run(config);
    }
    else if (mode == "graphic") {
        config.graphicMode = true;
        if (argc >= 4) config.maxIterations = std::stoi(argv[3]);
        GraphicRunner::run(config);
    }
    else {
        std::cout << "Mode inconnu : " << mode << "\n";
    }

    return 0;
}

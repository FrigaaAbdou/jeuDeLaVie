#include "SimulationConfig.h"
#include "ConsoleRunner.h"
#include "GraphicRunner.h"
#include <iostream>

int main(int argc, char** argv) {
    SimulationConfig config;

    if (argc < 2) {
        std::cout << "Usage:\n"
                  << "  " << argv[0] << " console <input_file> <baseName> [maxIter]\n"
                  << "  " << argv[0] << " graphic [input_file] [maxIter]\n";
        return 0;
    }

    std::string mode = argv[1];

    if (mode == "console") {
        if (argc < 4) {
            std::cout << "Usage console: " << argv[0]
                      << " console <input_file> <baseName> [maxIter]\n";
            return 0;
        }
        config.inputFile = argv[2];
        config.graphicMode = false;
        config.outputBaseName = argv[3];
        if (argc >= 5) config.maxIterations = std::stoi(argv[4]);

        ConsoleRunner::run(config);
    }
    else if (mode == "graphic") {
        if (argc >= 3) config.inputFile = argv[2];
        config.graphicMode = true;
        if (argc >= 4) config.maxIterations = std::stoi(argv[3]);
        GraphicRunner::run(config);
    }
    else {
        std::cout << "Mode inconnu : " << mode << "\n";
    }

    return 0;
}

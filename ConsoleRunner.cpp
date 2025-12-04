#include "ConsoleRunner.h"
#include "InitialStateLoader.h"
#include "GameOfLife.h"
#include "ConwayRule.h"
#include "GridExporter.h"
#include <iostream>

void ConsoleRunner::run(const SimulationConfig& config) {
    try {
        Grid grid = InitialStateLoader::loadFromFile(config.inputFile,
                                                     config.toroidal);
        GameOfLife game(grid, std::make_unique<ConwayRule>(), config.maxIterations);

        int iter = 0;
        while (!game.hasFinished()) {
            game.step();
            ++iter;
            GridExporter::exportGrid(game.currentGrid(),
                                     config.outputBaseName, iter);
        }
        std::cout << "Simulation terminee en " << iter << " iterations.\n";
    }
    catch (const std::exception& ex) {
        std::cerr << "Erreur: " << ex.what() << "\n";
    }
}

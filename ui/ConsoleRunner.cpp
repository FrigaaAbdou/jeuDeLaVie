#include "ConsoleRunner.h"
#include "FileGridLoader.h"
#include "FileGridExporter.h"
#include "SimulationService.h"
#include "ConwayRule.h"
#include <iostream>

void ConsoleRunner::run(const SimulationConfig& config) {
    try {
        FileGridLoader loader;
        FileGridExporter exporter;
        SimulationService service(config, loader, &exporter, std::make_unique<ConwayRule>());

        int iter = 0;
        while (!service.hasFinished()) {
            service.step();
            ++iter;
        }
        std::cout << "Simulation terminee en " << iter << " iterations.\n";
    }
    catch (const std::exception& ex) {
        std::cerr << "Erreur: " << ex.what() << "\n";
    }
}

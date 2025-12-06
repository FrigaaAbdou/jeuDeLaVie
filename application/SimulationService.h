#pragma once
#include <memory>
#include <string>
#include "SimulationConfig.h"
#include "GameOfLife.h"
#include "IGridLoader.h"
#include "IGridExporter.h"
#include "Rule.h"

// Façade applicative orchestrant une simulation, indépendante de l'UI.
class SimulationService {
public:
    SimulationService(const SimulationConfig& config,
                      const IGridLoader& loader,
                      const IGridExporter* exporter,
                      std::unique_ptr<Rule> rule);

    void step(); // exécute une itération (et exporte si un exporter est fourni)

    const Grid& currentGrid() const;
    bool isStable() const;
    bool hasFinished() const;
    int currentIteration() const;

private:
    SimulationConfig m_config;
    const IGridLoader& m_loader;
    const IGridExporter* m_exporter; // peut être nul pour le mode graphique
    std::unique_ptr<Rule> m_rule;
    std::unique_ptr<GameOfLife> m_game;
};

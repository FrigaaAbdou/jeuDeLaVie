#include "SimulationService.h"
#include "ConwayRule.h"

SimulationService::SimulationService(const SimulationConfig& config,
                                     const IGridLoader& loader,
                                     const IGridExporter* exporter,
                                     std::unique_ptr<Rule> rule)
    : m_config(config),
      m_loader(loader),
      m_exporter(exporter),
      m_rule(std::move(rule))
{
    // Charger la grille initiale via le port de chargement
    Grid initial = m_loader.loadGrid(m_config.inputFile, m_config.toroidal);
    if (!m_rule) {
        m_rule = std::make_unique<ConwayRule>();
    }
    m_game = std::make_unique<GameOfLife>(initial, std::move(m_rule), m_config.maxIterations);
}

void SimulationService::step() {
    if (!m_game) return;
    m_game->step();
    if (m_exporter) {
        std::string base = m_config.outputBaseName.empty() ? "output" : m_config.outputBaseName;
        m_exporter->exportGrid(m_game->currentGrid(), base, m_game->currentIteration());
    }
}

const Grid& SimulationService::currentGrid() const {
    return m_game->currentGrid();
}

bool SimulationService::isStable() const {
    return m_game->isStable();
}

bool SimulationService::hasFinished() const {
    return m_game->hasFinished();
}

int SimulationService::currentIteration() const {
    return m_game->currentIteration();
}

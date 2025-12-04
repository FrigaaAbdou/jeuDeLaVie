#pragma once
#include <memory>
#include "Grid.h"
#include "Rule.h"

class GameOfLife {
private:
    Grid m_grid;
    Grid m_previousGrid;
    std::unique_ptr<Rule> m_rule;
    int m_maxIterations;
    int m_currentIteration;

public:
    GameOfLife(const Grid& initial,
               std::unique_ptr<Rule> rule,
               int maxIterations);

    void step();                 // une itération
    bool isStable() const;       // grille inchangée
    bool hasFinished() const;    // stable ou max atteint
    const Grid& currentGrid() const { return m_grid; }
};

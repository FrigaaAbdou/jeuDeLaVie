#include "GameOfLife.h"

GameOfLife::GameOfLife(const Grid& initial,
                       std::unique_ptr<Rule> rule,
                       int maxIterations)
    : m_grid(initial),
      m_previousGrid(initial),
      m_rule(std::move(rule)),
      m_maxIterations(maxIterations),
      m_currentIteration(0)
{
}

void GameOfLife::step() {
    // conserve la grille courante pour la detection de stabilite
    m_previousGrid = m_grid;

    // on repart d'une copie pour conserver la configuration (taille, mode torique)
    Grid next = m_grid;
    for (int r = 0; r < m_grid.rows(); ++r)
        for (int c = 0; c < m_grid.cols(); ++c) {
            CellState* nextState = m_rule->nextState(m_grid.at(r, c), m_grid);
            next.at(r, c).setState(nextState);
        }

    m_grid = std::move(next);
    ++m_currentIteration;
}

bool GameOfLife::isStable() const {
    return m_grid.equals(m_previousGrid);
}

bool GameOfLife::hasFinished() const {
    if (m_currentIteration == 0) return false; // aucune etape effectuee
    return isStable() || m_currentIteration >= m_maxIterations;
}
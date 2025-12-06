#pragma once
#include "Rule.h"
#include "AliveState.h"
#include "DeadState.h"

class ConwayRule : public Rule {
public:
    CellState* nextState(const Cell& cell,
                         const Grid& grid) const override {
        int alive = grid.countAliveNeighbors(cell.row(), cell.col());
        bool cur = cell.isAlive();

        if (cur && (alive == 2 || alive == 3))
            return new AliveState();
        if (!cur && alive == 3)
            return new AliveState();

        return new DeadState();
    }
};

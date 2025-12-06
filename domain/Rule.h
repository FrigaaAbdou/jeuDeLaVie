#pragma once
#include "Cell.h"
#include "Grid.h"

class Rule {
public:
    virtual ~Rule() = default;

    // retourne un NOUVEL état (utiliser new)
    virtual CellState* nextState(const Cell& cell,
                                 const Grid& grid) const = 0;
};

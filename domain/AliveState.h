#pragma once
#include "CellState.h"

class AliveState : public CellState {
public:
    bool isAlive() const override { return true; }
    CellState* clone() const override { return new AliveState(*this); }
};

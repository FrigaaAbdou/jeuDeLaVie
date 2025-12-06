#pragma once
#include "CellState.h"

class DeadState : public CellState {
public:
    bool isAlive() const override { return false; }
    CellState* clone() const override { return new DeadState(*this); }
};

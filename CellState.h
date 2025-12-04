#pragma once

class CellState {
public:
    virtual ~CellState() = default;

    virtual bool isAlive() const = 0;
    virtual CellState* clone() const = 0;
};

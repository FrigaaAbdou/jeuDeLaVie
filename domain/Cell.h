#pragma once
#include "CellState.h"

class Cell {
private:
    int m_row;
    int m_col;
    CellState* m_state;

public:
    Cell(int row, int col, CellState* state)
        : m_row(row), m_col(col), m_state(state) {}

    // deep copy
    Cell(const Cell& other)
        : m_row(other.m_row), m_col(other.m_col),
          m_state(other.m_state ? other.m_state->clone() : nullptr) {}

    // move
    Cell(Cell&& other) noexcept
        : m_row(other.m_row), m_col(other.m_col), m_state(other.m_state) {
        other.m_state = nullptr;
    }

    Cell& operator=(const Cell& other) {
        if (this != &other) {
            m_row = other.m_row;
            m_col = other.m_col;
            delete m_state;
            m_state = other.m_state ? other.m_state->clone() : nullptr;
        }
        return *this;
    }

    Cell& operator=(Cell&& other) noexcept {
        if (this != &other) {
            m_row = other.m_row;
            m_col = other.m_col;
            delete m_state;
            m_state = other.m_state;
            other.m_state = nullptr;
        }
        return *this;
    }

    ~Cell() {
        delete m_state;
    }

    int row() const { return m_row; }
    int col() const { return m_col; }

    bool isAlive() const {
        return m_state && m_state->isAlive();
    }

    void setState(CellState* newState) {
        delete m_state;
        m_state = newState;
    }

    const CellState* state() const { return m_state; }
};

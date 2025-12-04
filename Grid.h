#pragma once
#include <vector>
#include "Cell.h"

class Grid {
private:
    int m_rows;
    int m_cols;
    bool m_toroidal;
    std::vector<std::vector<Cell>> m_cells;

public:
    Grid(int rows = 0, int cols = 0, bool toroidal = false);

    int rows() const { return m_rows; }
    int cols() const { return m_cols; }

    Cell& at(int r, int c)       { return m_cells[r][c]; }
    const Cell& at(int r, int c) const { return m_cells[r][c]; }

    int countAliveNeighbors(int r, int c) const;
    bool equals(const Grid& other) const;
};

#include "Grid.h"
#include "AliveState.h"
#include "DeadState.h"

Grid::Grid(int rows, int cols, bool toroidal)
    : m_rows(rows), m_cols(cols), m_toroidal(toroidal)
{
    m_cells.reserve(m_rows);
    for (int r = 0; r < m_rows; ++r) {
        std::vector<Cell> row;
        row.reserve(m_cols);
        for (int c = 0; c < m_cols; ++c) {
            row.emplace_back(r, c, new DeadState());
        }
        m_cells.push_back(std::move(row));
    }
}

int Grid::countAliveNeighbors(int r, int c) const {
    int count = 0;
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;

            int rr = r + dr;
            int cc = c + dc;

            if (m_toroidal) {
                rr = (rr + m_rows) % m_rows;
                cc = (cc + m_cols) % m_cols;
            }

            if (rr >= 0 && rr < m_rows &&
                cc >= 0 && cc < m_cols &&
                m_cells[rr][cc].isAlive())
            {
                ++count;
            }
        }
    }
    return count;
}

bool Grid::equals(const Grid& other) const {
    if (m_rows != other.m_rows || m_cols != other.m_cols) return false;
    for (int r = 0; r < m_rows; ++r)
        for (int c = 0; c < m_cols; ++c)
            if (at(r,c).isAlive() != other.at(r,c).isAlive())
                return false;
    return true;
}

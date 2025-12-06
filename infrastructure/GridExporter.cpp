#include "GridExporter.h"
#include <fstream>

void GridExporter::exportGrid(const Grid& grid,
                              const std::string& baseName,
                              int iteration)
{
    std::string fileName = baseName + "_" + std::to_string(iteration) + ".txt";
    std::ofstream out(fileName);
    if (!out) return;

    out << grid.rows() << " " << grid.cols() << "\n";
    for (int r = 0; r < grid.rows(); ++r) {
        for (int c = 0; c < grid.cols(); ++c) {
            out << (grid.at(r,c).isAlive() ? 1 : 0);
            if (c + 1 < grid.cols()) out << ' ';
        }
        out << "\n";
    }
}

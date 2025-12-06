#pragma once
#include <string>
#include "Grid.h"

class GridExporter {
public:
    static void exportGrid(const Grid& grid,
                           const std::string& baseName,
                           int iteration);
};

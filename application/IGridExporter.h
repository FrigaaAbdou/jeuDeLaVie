#pragma once
#include <string>
#include "Grid.h"

// Port d'application : export d'une grille.
class IGridExporter {
public:
    virtual ~IGridExporter() = default;
    virtual void exportGrid(const Grid& grid,
                            const std::string& baseName,
                            int iteration) const = 0;
};

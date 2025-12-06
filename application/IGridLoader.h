#pragma once
#include <string>
#include "Grid.h"

// Port d'application : chargement d'une grille initiale.
class IGridLoader {
public:
    virtual ~IGridLoader() = default;
    virtual Grid loadGrid(const std::string& path, bool toroidal) const = 0;
};

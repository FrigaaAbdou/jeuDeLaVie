#pragma once
#include "IGridExporter.h"
#include "GridExporter.h"

// Infrastructure : implémentation d'export vers un fichier texte.
class FileGridExporter : public IGridExporter {
public:
    void exportGrid(const Grid& grid,
                    const std::string& baseName,
                    int iteration) const override {
        GridExporter::exportGrid(grid, baseName, iteration);
    }
};

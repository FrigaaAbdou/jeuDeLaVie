#pragma once
#include "IGridLoader.h"
#include "InitialStateLoader.h"

// Infrastructure : implémentation de chargement depuis un fichier texte.
class FileGridLoader : public IGridLoader {
public:
    Grid loadGrid(const std::string& path, bool toroidal) const override {
        return InitialStateLoader::loadFromFile(path, toroidal);
    }
};

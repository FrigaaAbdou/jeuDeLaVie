#include "InitialStateLoader.h"
#include "AliveState.h"
#include "DeadState.h"
#include <fstream>
#include <stdexcept>

Grid InitialStateLoader::loadFromFile(const std::string& path, bool toroidal) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Impossible d'ouvrir le fichier : " + path);
    }

    int rows = 0, cols = 0;
    in >> rows >> cols;
    if (!in) {
        throw std::runtime_error("Fichier invalide (dimensions)");
    }

    Grid grid(rows, cols, toroidal);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int v = 0;
            in >> v;
            if (!in) {
                throw std::runtime_error("Fichier invalide (etat cellules)");
            }
            if (v == 1) {
                grid.at(r, c).setState(new AliveState());
            } else {
                grid.at(r, c).setState(new DeadState());
            }
        }
    }

    return grid;
}

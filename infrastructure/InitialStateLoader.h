#pragma once
#include <string>
#include "Grid.h"

class InitialStateLoader {
public:
    static Grid loadFromFile(const std::string& path, bool toroidal = false);
};

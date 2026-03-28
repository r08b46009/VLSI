#pragma once

#include "Data.hpp"

#include <string>
#include <vector>

class Parser {
public:
    Parser() : maxDisplacement(0) {}

    LegalizerInput* parse(const char* auxPath);

private:
    std::string nodeFile;
    std::string plFile;
    std::string sclFile;
    int maxDisplacement;
    std::vector<Cell*> cells;
    std::vector<Cell*> terminals;
    std::vector<Row*> rows;

    void read(const std::string& filename);
};

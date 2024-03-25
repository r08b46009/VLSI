#pragma once
#include "Data.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

class Parser
{
    public:
    string nodeFile, plFile, sclFile;
    int maxDisplacement;
    vector<Cell *> cells, terminals;
    vector<Row *> rows;

    void read(string const &filename);
    Parser() : maxDisplacement(0) {}
    LegalizerInput *parse(char argv[]);
};

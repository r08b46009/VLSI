#pragma once

#include "Data.hpp"

class Abacus {
public:
    explicit Abacus(LegalizerInput* input) : input_(input), previousBestCost_(0.0) { AbacusLegalize(); }

    void AbacusLegalize();
    int findRow(const Cell* cell);
    int findSubRow(const Row* row, const Cell* cell);
    int placeRow(int rowIdx, Cell* cell);
    void placeRowFinal(int rowIdx, int subRowIdx, Cell* cell);
    double cost(const Cell* cell);

private:
    LegalizerInput* input_;
    double previousBestCost_;
};

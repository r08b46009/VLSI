#pragma once
#include "Data.hpp"

class abacus
{
    public:
    

    void Abacus();
    LegalizerInput *input;
    int findrow(Cell const *cell);
    int findsubrow(Row const *row, Cell const *cell);

    int placerow(int const &rowIdx, Cell *cell);
    void placeRowFinal(int const &rowIdx, int const &subRowIdx, Cell *cell);
    double cost(Cell const *cell);



    abacus(LegalizerInput *input) : input(input) {

        Abacus();

    }
};




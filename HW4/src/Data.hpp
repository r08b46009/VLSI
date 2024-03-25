#pragma once
#include <string>
#include <vector>
using namespace std;

struct Cell
{
    string n;
    double width, height, weight, x, y, optimalX, optimalY;

    Cell(string const &N, int const &W, int const &H)
    { n = N; 
    width = W;
    height = H;
    weight = width;
    x =0; y = 0 ; optimalX = 0; optimalY =0; }
};

struct Cluster
{
    double x, q, w, wei;
    vector<Cell *> member;
    Cluster *p;

    Cluster(double const &X, Cluster *P, double const &Wei, double const &Q, double const &W)
 {
    x = X;
    p = P;
    wei = Wei;
    q = Q;
    w = W;

 }
};

 

struct SubRow
{
    double minX, maxX, space;
    Cluster *last;

    SubRow(double const &x, double const &X)
         {
            minX = x;
            maxX = X;
            space= maxX - minX; 
            last = nullptr; 
         }
    inline void updateMinMax(double const &low, double const &high)
    {
        this->minX = low;
        this->maxX = high;
        this->space = high - low;
    }
};

struct Row
{
    double width, height, y;
    vector<SubRow *> subRows;

    Row(double const &W, double const &H, double const &Y)
 {
        width = W; 
        height= H; 
        y=Y; 
 }
};

struct LegalizerInput
{
    double maxDisplacement;
    vector<Cell *> cells, terminals;
    vector<Row *> rows;

    LegalizerInput(double const &maxp, vector<Cell *> const &c,
                vector<Cell *> const &t, vector<Row *> const &r)
         {
            maxDisplacement=maxp; cells =c; terminals = t ;rows = r;
         }
};

#pragma once

#include <string>
#include <vector>

struct Cell {
    std::string n;
    double width;
    double height;
    double weight;
    double x;
    double y;
    double optimalX;
    double optimalY;

    Cell(const std::string& name, int w, int h)
        : n(name), width(w), height(h), weight(w), x(0), y(0), optimalX(0), optimalY(0) {}
};

struct Cluster {
    double x;
    double q;
    double w;
    double wei;
    std::vector<Cell*> member;
    Cluster* p;

    Cluster(double x_, Cluster* prev, double weight, double q_, double width)
        : x(x_), q(q_), w(width), wei(weight), p(prev) {}
};

struct SubRow {
    double minX;
    double maxX;
    double space;
    Cluster* last;

    SubRow(double low, double high)
        : minX(low), maxX(high), space(high - low), last(nullptr) {}

    void updateMinMax(double low, double high) {
        minX = low;
        maxX = high;
        space = high - low;
    }
};

struct Row {
    double width;
    double height;
    double y;
    std::vector<SubRow*> subRows;

    Row(double siteWidth, double rowHeight, double rowY)
        : width(siteWidth), height(rowHeight), y(rowY) {}
};

struct LegalizerInput {
    double maxDisplacement;
    std::vector<Cell*> cells;
    std::vector<Cell*> terminals;
    std::vector<Row*> rows;

    LegalizerInput(double maxDisp,
                   const std::vector<Cell*>& cellList,
                   const std::vector<Cell*>& terminalList,
                   const std::vector<Row*>& rowList)
        : maxDisplacement(maxDisp), cells(cellList), terminals(terminalList), rows(rowList) {}
};

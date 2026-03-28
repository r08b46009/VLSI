#include "abacus.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace {
constexpr double kEarlyStopRatio = 0.67;
constexpr double kInvalidPlacementX = 1000000.0;

bool CompareCellX(Cell* a, Cell* b) {
    return a->x < b->x;
}
}  // namespace

double Abacus::cost(const Cell* cell) {
    const double dx = cell->optimalX - cell->x;
    const double dy = cell->optimalY - cell->y;
    return std::sqrt(dx * dx + dy * dy);
}

int Abacus::findRow(const Cell* cell) {
    double minY = 0.0;
    const int rowCount = static_cast<int>(input_->rows.size());
    for (int i = 0; i < rowCount - 1; ++i) {
        const double frontDistance = std::abs(cell->y - input_->rows[i]->y);
        const double backDistance = std::abs(cell->y - input_->rows[rowCount - 1 - i]->y);
        const bool useReverse = frontDistance > backDistance;

        if (!useReverse) {
            if (i == 0) {
                minY = frontDistance;
            } else if (minY > frontDistance) {
                minY = frontDistance;
            } else {
                return i;
            }
        } else {
            if (i == 0) {
                minY = frontDistance;
            } else if (minY > backDistance) {
                minY = backDistance;
            } else {
                return rowCount - i;
            }
        }
    }
    return rowCount - 1;
}

int Abacus::findSubRow(const Row* row, const Cell* cell) {
    const auto& subRows = row->subRows;
    const double width = cell->width;
    if (subRows.empty()) {
        return -1;
    }

    if (cell->x >= subRows.back()->maxX) {
        if (width <= subRows.back()->space) {
            return static_cast<int>(subRows.size()) - 1;
        }
    } else {
        for (int i = 0; i < static_cast<int>(subRows.size()); ++i) {
            const auto* subRow = subRows[i];
            const double freeSpace = subRow->space;
            if (cell->x >= subRow->maxX) {
                continue;
            }

            if (cell->x >= subRow->minX) {
                if (width <= freeSpace) {
                    return i;
                }
            } else {
                if (i == 0) {
                    if (width <= freeSpace) {
                        return 0;
                    }
                } else {
                    const auto* prevSubRow = subRows[i - 1];
                    if (std::abs(cell->x + width - prevSubRow->maxX) < std::abs(cell->x - subRow->minX)) {
                        if (width <= prevSubRow->space) {
                            return i - 1;
                        }
                    } else if (width <= freeSpace) {
                        return i;
                    }
                }
            }
        }
    }
    return -1;
}

int Abacus::placeRow(int rowIdx, Cell* cell) {
    const auto* row = input_->rows.at(rowIdx);
    const int subRowIdx = findSubRow(row, cell);
    if (subRowIdx == -1) {
        cell->optimalX = kInvalidPlacementX;
        cell->optimalY = row->y;
        return -1;
    }

    const auto* subRow = row->subRows[subRowIdx];
    double optimalX = cell->x;
    const double left = subRow->minX;
    const double right = subRow->maxX - cell->width;
    Cluster* cluster = subRow->last;

    if (cell->x < left) {
        optimalX = left;
    } else if (cell->x > right) {
        optimalX = right;
    }

    if (cluster == nullptr || cluster->x + cluster->w <= optimalX) {
        cell->optimalX = optimalX;
    } else {
        double trialWeight = cluster->wei + cell->weight;
        double trialWidth = cluster->w + cell->width;
        double trialQ = cluster->q + cell->weight * (optimalX - cluster->w);
        double trialX = 0.0;

        while (true) {
            trialX = trialQ / trialWeight;
            const double trialLeft = subRow->minX;
            const double trialRight = subRow->maxX - trialWidth;
            if (trialX < trialLeft) {
                trialX = trialLeft;
            }
            if (trialX > trialRight) {
                trialX = trialRight;
            }

            Cluster* prevCluster = cluster->p;
            if (prevCluster != nullptr && prevCluster->x + prevCluster->w > trialX) {
                trialQ = prevCluster->q + trialQ - trialWeight * prevCluster->w;
                trialWeight = prevCluster->wei + trialWeight;
                trialWidth = prevCluster->w + trialWidth;
                cluster = prevCluster;
            } else {
                break;
            }
        }
        cell->optimalX = trialX + trialWidth - cell->width;
    }

    cell->optimalY = row->y;
    return subRowIdx;
}

void Abacus::placeRowFinal(int rowIdx, int subRowIdx, Cell* cell) {
    auto* subRow = input_->rows[rowIdx]->subRows[subRowIdx];
    subRow->space -= cell->width;

    double optimalX = cell->x;
    const double left = subRow->minX;
    const double right = subRow->maxX - cell->width;
    if (cell->x < left) {
        optimalX = left;
    } else if (cell->x > right) {
        optimalX = right;
    }

    Cluster* cluster = subRow->last;
    if (cluster == nullptr || cluster->x + cluster->w <= optimalX) {
        cluster = new Cluster(optimalX, subRow->last, cell->weight, cell->weight * optimalX, cell->width);
        subRow->last = cluster;
        cluster->member.push_back(cell);
    } else {
        cluster->member.push_back(cell);
        cluster->wei += cell->weight;
        cluster->q += cell->weight * (optimalX - cluster->w);
        cluster->w += cell->width;

        while (true) {
            cluster->x = cluster->q / cluster->wei;
            const double clusterLeft = subRow->minX;
            const double clusterRight = subRow->maxX - cluster->w;

            if (cluster->x < clusterLeft) {
                cluster->x = clusterLeft;
            } else if (cluster->x > clusterRight) {
                cluster->x = clusterRight;
            }

            Cluster* prevCluster = cluster->p;
            if (prevCluster != nullptr && prevCluster->x + prevCluster->w > cluster->x) {
                prevCluster->member.insert(prevCluster->member.end(), cluster->member.begin(), cluster->member.end());
                prevCluster->wei += cluster->wei;
                prevCluster->q += cluster->q - cluster->wei * prevCluster->w;
                prevCluster->w += cluster->w;

                delete cluster;
                cluster = prevCluster;
            } else {
                break;
            }
        }
        subRow->last = cluster;
    }
}

void Abacus::AbacusLegalize() {
    std::sort(input_->terminals.begin(), input_->terminals.end(), CompareCellX);
    for (auto* terminal : input_->terminals) {
        const double left = terminal->x;
        const double right = terminal->x + terminal->width;
        for (auto* row : input_->rows) {
            if (row->y < terminal->y || row->y >= terminal->y + terminal->height) {
                continue;
            }

            auto* lastSubRow = row->subRows.back();
            const double l = lastSubRow->minX;
            const double r = lastSubRow->maxX;
            if (l < left) {
                if (r > right) {
                    row->subRows.push_back(new SubRow(right, r));
                }
                lastSubRow->updateMinMax(l, left);
            } else {
                if (r > right) {
                    lastSubRow->updateMinMax(right, r);
                } else {
                    delete lastSubRow;
                    row->subRows.pop_back();
                }
            }
        }
    }

    std::sort(input_->cells.begin(), input_->cells.end(), CompareCellX);
    for (auto* cell : input_->cells) {
        int bestRowIdx = findRow(cell);
        int down = bestRowIdx;
        int up = bestRowIdx;
        int bestSubRowIdx = placeRow(bestRowIdx, cell);
        double bestCost = cost(cell);

        while (down > 0 && std::abs(cell->y - input_->rows[down]->y) < bestCost) {
            down -= 1;
            const int subRowIdx = placeRow(down, cell);
            const double now = cost(cell);
            if (now < bestCost) {
                bestRowIdx = down;
                bestSubRowIdx = subRowIdx;
                bestCost = now;
            }
            if (previousBestCost_ > 0.0 && bestCost < previousBestCost_ * kEarlyStopRatio) {
                break;
            }
        }

        while (up < static_cast<int>(input_->rows.size()) - 1 &&
               std::abs(cell->y - input_->rows[up]->y) < bestCost) {
            up += 1;
            const int subRowIdx = placeRow(up, cell);
            const double now = cost(cell);
            if (now < bestCost) {
                bestRowIdx = up;
                bestSubRowIdx = subRowIdx;
                bestCost = now;
            }
            if (previousBestCost_ > 0.0 && bestCost < previousBestCost_ * kEarlyStopRatio) {
                break;
            }
        }
        previousBestCost_ = bestCost;
        placeRowFinal(bestRowIdx, bestSubRowIdx, cell);
    }

    for (int i = 0; i < static_cast<int>(input_->rows.size()); ++i) {
        const double rowWidth = input_->rows[i]->width;
        for (auto* subRow : input_->rows[i]->subRows) {
            auto* cluster = subRow->last;
            while (cluster != nullptr) {
                const double shiftX = cluster->x - subRow->minX;
                const double ratio = shiftX / rowWidth;
                cluster->x = std::ceil(ratio) * rowWidth + subRow->minX;
                if (shiftX - (std::floor(ratio) + 0.5) * rowWidth <= 0) {
                    cluster->x = std::floor(ratio) * rowWidth + subRow->minX;
                }

                double optimalX = cluster->x;
                for (auto* cell : cluster->member) {
                    cell->optimalX = optimalX;
                    cell->optimalY = input_->rows[i]->y;
                    optimalX += cell->width;
                }
                cluster = cluster->p;
            }
        }
    }

    double totalDisplacement = 0.0;
    double maxDisplacement = 0.0;
    for (int i = 0; i < static_cast<int>(input_->cells.size()); ++i) {
        const double dx = input_->cells[i]->optimalX - input_->cells[i]->x;
        const double dy = input_->cells[i]->optimalY - input_->cells[i]->y;
        const double displacement = std::sqrt(dx * dx + dy * dy);
        totalDisplacement += displacement;
        if (maxDisplacement < displacement) {
            maxDisplacement = displacement;
        }
    }

    std::printf("total: %.3f\n", totalDisplacement);
    std::printf("max:   %.3f\n", maxDisplacement);
}

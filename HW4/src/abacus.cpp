#include "abacus.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>
#include <iostream> 
#include <fstream>
using namespace std;

bool cmp(Cell *A, Cell *B)
{
      return (A->x) < (B->x);
}
double abacus::cost(Cell const *cell)
{

    double x = cell->optimalX - cell->x,y = cell->optimalY - cell->y;
    return sqrt(x * x + y * y);
}


int abacus::findrow(Cell const *cell)
{
    double minY;
    for (int i = 0 ; i < input->rows.size()-1; ++i)
    {
        double Eu = sqrt( (cell->y - input->rows[i]->y) *(cell->y - input->rows[i]->y) ), k = abs(cell->y - input->rows[i]->y);
        double k2 = abs(cell->y - input->rows[input->rows.size()-1-i]->y);
        bool reverse = 0;
        if(k > k2){
            reverse = 1;
        }
        else{
            reverse = 0;
        }

        if(reverse==0){

            if(i == 0){
                minY = k;
            }
            else if(minY > k){
                minY = k;
            }
            else{
                // // cout<<"state2";
                // if(Eu <120)
                return i;
            }
        }
        else{
            if( i == 0){
                minY = k;
            }
            else if(minY > k2){
                minY = k2;
            }
            else{
                // if(Eu <120)
                return input->rows.size()-i;
            }
        }

    
    }
    return input->rows.size()-1;
}

int abacus::findsubrow(Row const *row, Cell const *cell)
{
    auto &subRows = row->subRows;
     auto subRows1 = row->subRows;
     double w = cell->width;
    if (subRows.empty() == true)
        return -1;

    if (cell->x >= row->subRows.back()->maxX)
    {
        if (w <= row->subRows.back()->space)
            return subRows.size() - 1;
    }
    else
    {
        for (int i = 0; i < subRows.size(); ++i)
        {
            auto subRow = row->subRows[i];
            double free = subRow->space;
            if (cell->x >= subRow->maxX)
                continue;

            if (cell->x >= subRow->minX)
            {
                if (w <= free)
                    return i;
            }
            else
            {
                if(i == 0){
                    if (w <= free)
                        return 0;
                }
                else if (i > 0)
                {
                    if (abs(cell->x + w - row->subRows[i - 1]->maxX) < abs(cell->x - subRow->minX) ){
                        if (w <= row->subRows[i - 1]->space)
                            return i - 1;
                    }
                    else{
                        if (w <= free)
                            return i;
                    }
                }

            }
        }
    }
    return -1;
}

int abacus::placerow(int const &rowIdx, Cell *cell)
{
    auto const &row = input->rows.at(rowIdx);
    int subrowId = findsubrow(row, cell);
        auto const &subRow = row->subRows[subrowId];
    if (subrowId == -1)
    {
        cell->optimalX = 1000000;
        cell->optimalY = row->y;
        return -1;
 
    }


    double optimalX = cell->x, left = subRow->minX, right = subRow->maxX - cell->width;
    auto cluster = subRow->last;
    if (cell->x < left)
        optimalX = subRow->minX;
    else if (cell->x > right)
        optimalX = right;
    


    if (cluster == nullptr || cluster->x + cluster->w <= optimalX)
    {
        cell->optimalX = optimalX;
    }
    else
    {
        double trialWeight = cluster->wei + cell->weight, trialWidth = cluster->w + cell->width;
        double trialQ = cluster->q + cell->weight * (optimalX - cluster->w), trialX = 0;
        while (8)
        {
            trialX = trialQ / trialWeight;
            // trialX = (trialX < subRow->minX) ? subRow->minX : subRow->maxX - trialWidth;
            double left = subRow->minX, right = subRow->maxX - trialWidth;
            // trialX = (trialX < left) ? left : trialX;
            // trialX = (trialX < right) ? left : trialX;
            if (trialX < left)
                trialX = left;
            if (trialX > right)
                trialX = right;

            Cluster *t;

            auto const &prevCluster = t;
            if (prevCluster != nullptr && prevCluster->x + prevCluster->w > trialX)
            {
                trialQ = prevCluster->q + trialQ - trialWeight * prevCluster->w;
                trialWeight = prevCluster->wei + trialWeight;
                trialWidth = prevCluster->w + trialWidth;

                cluster = prevCluster;
            }
            else
            {
                break;
            }
        }
        cell->optimalX = trialX + trialWidth - cell->width;
    }
    cell->optimalY = row->y;
    return subrowId;
}

double last = 0;
int last2;
void abacus::placeRowFinal(int const &rowIdx, int const &subRowIdx, Cell *cell)
{
    // cout<<"123"<<endl;
    auto subRow = input->rows[rowIdx]->subRows[subRowIdx];
    subRow->space -= cell->width;

    double optimalX = cell->x, left = subRow->minX, right = subRow->maxX - cell->width;
     if (cell->x < left)
         optimalX = left;
     else if (cell->x > right)
         optimalX = right;
    // optimalX = (cell->x < left) ? left : optimalX;
    // optimalX = (cell->x > right) ? left : optimalX;

    auto cluster = subRow->last;
    if (cluster == nullptr || cluster->x + cluster->w <= optimalX)
    {
        cluster = new Cluster(optimalX,subRow->last,cell->weight,cell->weight * optimalX,cell->width);
        subRow->last = cluster;
        cluster->member.push_back(cell);
    }
    else
    {
        cluster->member.push_back(cell);
        cluster->wei += cell->weight;
        cluster->q += cell->weight * (optimalX - cluster->w);
        cluster->w += cell->width;

        while (true)
        {
            cluster->x = cluster->q / cluster->wei;
            double left = subRow->minX, right = subRow->maxX - cluster->w;

                // cluster->x = (cluster->x < subRow->minX) ? cluster->x = subRow->minX :
            // cluster->x = (cluster->x < left) ? left : cluster->q / cluster->weight;
            // cluster->x = (cluster->x > right) ? right : cluster->q / cluster->weight;

            
            if (cluster->x < left)
                 cluster->x = left;
            else if (cluster->x > right)
                cluster->x = right;

            auto prevCluster = cluster->p;
            if (prevCluster != nullptr && prevCluster->x + prevCluster->w > cluster->x)
            {
                prevCluster->member.insert(prevCluster->member.end(), cluster->member.begin(), cluster->member.end());
                prevCluster->wei += cluster->wei;
                prevCluster->q += cluster->q - cluster->wei * prevCluster->w;
                prevCluster->w += cluster->w;

                delete cluster;
                cluster = prevCluster;
            }
            else
            {
                break;
            }
        }
        subRow->last = cluster;
    }
}







void abacus::Abacus()
{
       
    auto it = input->terminals.begin(), it2 = input->terminals.end();
    sort(it, it2, cmp);
    for (auto &terminal : input->terminals)
    {
        double left = terminal->x, right = terminal->x + terminal->width;
        for (auto &row : input->rows)
        {
            if (row->y < terminal->y || row->y >= terminal->y + terminal->height)
                continue;

            auto last = row->subRows.back();
            double l = last->minX, r = last->maxX;
            if (l < left)
            {
                if (r > right)
                    row->subRows.push_back(new SubRow(right, r));
                last->updateMinMax(l, left);
            }
            else
            {
                if (r > right)
                {
                    last->updateMinMax(right, r);
                }
                else
                {
                    delete last;
                    row->subRows.pop_back();
                }
            }
        }
    }
    it = input->cells.begin();
    it2 = input->cells.end();
    sort( it, it2, cmp);
    for (auto &cell : input->cells)
    {
        int bestRowIdx = findrow(cell), down = bestRowIdx, up = bestRowIdx, bestSubRowIdx = placerow(bestRowIdx, cell);
        double bestCost = cost(cell);


        while (down > 0 && abs(cell->y - input->rows[down]->y) < bestCost)
        {
            down -= 1;
            int subRowIdx = placerow(down, cell);
            double now = cost(cell);
            if (now < bestCost)
            {
                bestRowIdx = down;
                bestSubRowIdx = subRowIdx;
                bestCost = now; 
            }
            if(bestCost < last *0.67){
                // cout<<"short"<<endl;
                break;
            }
            // if(now < last *0.66){
            //     break;
            // }
        }

        while (up < input->rows.size() - 1 && abs(cell->y - input->rows[up]->y) < bestCost)
        {
            up += 1;
            int subRowIdx = placerow(up, cell);
            double now = cost(cell);
            if (now < bestCost)
            {
                bestRowIdx = up;
                bestSubRowIdx = subRowIdx;
                bestCost = now;
                
            }
            if(bestCost < last *0.67){
                // cout<<"short"<<endl;
                break;
            }
        }
        last = bestCost;

        placeRowFinal(bestRowIdx, bestSubRowIdx, cell);
    }
    
        // for (auto &row : input->rows)
    for (int i = 0; i <  input->rows.size(); i++)
    {
        // auto row = input->cells[i];
        double rowWidth = input->rows[i]->width;
        for (auto &subRow : input->rows[i]->subRows)
        {
            auto cluster = subRow->last;
            while (cluster != nullptr)
            {
                double shiftX = cluster->x - subRow->minX, ratio = shiftX / rowWidth;
                cluster->x = ceil(ratio) * rowWidth + subRow->minX;
                if (shiftX - (floor(ratio) +0.5) * rowWidth <= 0){
                    cluster->x = floor(ratio) * rowWidth + subRow->minX;
                 }

                double optimalX = cluster->x;
                for (auto &cell : cluster->member)
                {
                    cell->optimalX = optimalX;
                    cell->optimalY = input->rows[i]->y;
                    optimalX += cell->width;
                }
                cluster = cluster->p;
            }
        }
    }

        double sum = 0, max = 0, dis = 0;
for (int i = 0; i <  input->cells.size(); i++)
    {
        double x = input->cells[i]->optimalX - input->cells[i]->x, y = input->cells[i]->optimalY - input->cells[i]->y;
        dis = sqrt(x * x + y * y);
        sum += dis;
        if (max < dis)
            max = dis;
        // if(sum < last2 -10000){
        //     break;
        // }
    }
    last2 = sum;

    printf("total: %.3f\n", sum);
    printf("max:   %.3f\n", max);
}



#include "SA.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;



unordered_map<string, Pin *> map;
void Parser::readHB(string const &filename)
{
    ifstream fin(filename);
    string temp;
    int cnt, cnt1;
    fin >> temp >> temp >> cnt;
    fin >> temp >> temp >> cnt1;
    while (cnt--)
    {

        //declare a new block;
        int id, num, zero, x, y;
        char c;
        string name;
        
        fin >> name >> temp;  //id from 0, hardrectilinear
        fin >> num;         //4
        fin >> c;           // (
        fin >> x;           // 0
        fin >> c;
        fin >> y;           // 0
        fin >> c; //x, y is the left down location
        
        
        fin >> c;
        fin >> zero;
        fin >> c;
        fin >> y;
        fin >> c;
        
        fin >> c;
        fin >> x;
        fin >> c;
        fin >> y;
        fin >> c;
        
        fin >> c;
        fin >> x;
        fin >> c;
        fin >> zero;
        fin >> c;
        
        hardblocks.push_back(new HB(name, x, y));

        map.emplace(name, hardblocks.back()->pin);

    }
}

void Parser::readPl(string const &filename)
{
    ifstream fin(filename);
    string name;
    int x, y;
    while (fin >> name >> x >> y)
        map.emplace(name, new Pin(name, x, y));
}

void Parser::readNet(string const &filename)
{
    ifstream fin(filename);
    string identifier;
    while (fin >> identifier)
    {
        if (identifier == "NetDegree")
        {
            nets.push_back(new Net());
            string temp;
            size_t pinNum;
            fin >> temp >> pinNum;
            for (size_t i = 0; i < pinNum; ++i)
            {
                string name;
                fin >> name;
                nets.back()->pins.push_back(map.at(name));
            }
        }
    }
}

SAInput *Parser::parse(char *argv[])
{
    readHB(argv[1]);
    readPl(argv[3]);
    readNet(argv[2]);
    return new SAInput(hardblocks, nets, stod(argv[5]));
}



void ResultWriter::write(string const &filename, int WL)
{
    ofstream fout(filename);
    fout << "Wirelength " << WL << endl
         << "Blocks\n";
    for (auto  &hardblock : input->hardblocks)
        fout << hardblock->name << ' '
             << hardblock->x << ' '
             << hardblock->y << ' '
             << hardblock->isRotated << endl;
}


int Net::HPWL()
{
    int cnt = 0, minX = 0 , maxX = 0, minY = 0 , maxY = 0;
    for (auto pin : pins)
    {
        if(cnt == 0){
            minX = maxX = pin->x;
            minY = maxY = pin->y;
        }
        else{
            if (minX > pin->x)
                minX = pin->x;
            if (minY > pin->y)
                minY = pin->y;
            if (maxX < pin->x)
                maxX = pin->x;
            if (maxY < pin->y)
                maxY = pin->y;
        }
        cnt++;
    }
    return (maxX - minX) + (maxY - minY);
}

bool Cmp(vector<int>  &a,vector<int>  &b)
{
    return a[1] > b[1];
}
/*
bool Cmp1( vector<int>  &a,  vector<int>  &b)
{
    return a[1] <= b[1];
}*/

void Node::update()
{
    int WR, WL, HR, HL;
    direc.clear();
    if (type == -2)
    {
        sort(left->direc.begin(), left->direc.end(), Cmp);
        sort(right->direc.begin(), right->direc.end(), Cmp);

        int l = 0, r = 0;
        
        while (l < static_cast<int>(left->direc.size()) &&
               r < static_cast<int>(right->direc.size()))
        {
            WL = left->direc[l][0], WR = right->direc[r][0], HL = left->direc[l][1], HR = right->direc[r][1];
             vector<int> row = {WL + WR,
                                     max(HL, HR),
                                    l, r};
            direc.push_back(row);
            if (HL > HR)
                l += 1;
            else if (HL < HR)
                r += 1;
            else
            {
                l += 1;
                r += 1;
            }
        }
    }
    else
    {
         sort(left->direc.begin(), left->direc.end());
         sort(right->direc.begin(), right->direc.end());

        int l = left->direc.size() - 1, r = right->direc.size() - 1;
        while (l >= 0 && r >= 0)
        {
            WL = left->direc[l][0], WR = right->direc[r][0], HL = left->direc[l][1], HR = right->direc[r][1];
             vector<int> row = { max(WL, WR),
                                    HL + HR,
                                    l, r};
            direc.push_back(row);
            if (WL > WR)
                l -= 1;
            else if (WL < WR)
                r -= 1;
            else
            {
                l -= 1;
                r -= 1;
            }
        }
    }
}




bool SA::check( vector<int>  &post, int  &idx) 
{
    int num = post.size();
    if (post[idx + 1] < 0)
    {
        int op = 0;
        for (int i = 0; i <= idx + 1; ++i)
            if (post[i] < 0)
                op += 1;
        
        if (2 * op >= idx + 1)
            return false;
    }
    
    if (post[idx + 1] < 0 && idx != 0)
    {
        if (post[idx - 1] != post[idx + 1])
            return true;
    }
    else if (post[idx] < 0 && idx + 1 != num - 1)
    {
        if (post[idx] != post[idx + 2])
            return true;
    }
    return false;
}

void SA::perturbinit(vector<int> post, int &type){
    int num = post.size();
    int idx[num], idxCnt = 0, idx2 = 0, violationCnt = 0, idx1;
    if(type == 0){
        for (int i = 0; i < num; ++i){
            if (post[i] >= 0)
                idx[idxCnt++] = i;
        }
    }
    else if(type == 1){
        for (int i = 1; i < num; ++i)
            if (post[i - 1] >=  0 && post[i] <  0)
                idx[idxCnt++] = i;
    }
    else if(type == 2){
        for (int i = 0; i < num - 1; ++i)
            if ((post[i] >=  0 && post[i + 1] <  0) || (post[i] <  0 && post[i + 1] >=  0))
                idx[idxCnt++] = i;
    }
    return;

    

}



 vector<int> SA::perturb( vector<int> post, int &type)
{
    int num = post.size(), idx[num], idxCnt = 0, idx2 = 0, violationCnt = 0, idx1;
    //perturbinit(post, type);
    if(type == 0){
        for (int i = 0; i < num; ++i){
            if (post[i] >= 0)
                idx[idxCnt++] = i;
        }

        idx1 = rand() % idxCnt;
        idx2 = rand() % idxCnt;
        while (idx1 == idx2)
            idx2 = rand() % idxCnt;
        idx1 = idx[idx1];
        idx2 = idx[idx2];
         swap(post[idx1], post[idx2]);
    }
    else if(type == 1){
        for (int i = 1; i < num; ++i)
            if (post[i - 1] >=  0 && post[i] <  0)
                idx[idxCnt++] = i;

        idx1 = rand() % idxCnt;
        idx1 = idx[idx1];
            while (idx1 < num && post[idx1] <  0)
            {
                if (post[idx1] == -2)
                    post[idx1] = -1;
                else
                    post[idx1] = -2;
                idx1 += 1;
            }
    }

    else if(type ==2){
        for (int i = 0; i < num - 1; ++i)
            if ((post[i] >=  0 && post[i + 1] <  0) ||
                (post[i] <  0 && post[i + 1] >=  0))
                idx[idxCnt++] = i;

        do
        {
            idx1 = rand() % idxCnt;
            violationCnt += 1;
        } while ((check(post, idx[idx1]) ) == false && violationCnt < idxCnt);

        if (violationCnt < idxCnt)
        {
            idx1 = idx[idx1];
            idx2 = idx1 + 1;
             swap(post[idx1], post[idx2]);
        }
    }
  
    return post;
}

Node *SA::buildTree( vector<int>  &post)
{
    int cutIdx = 0;
     vector<Node *> idxtOrder;
    for (auto  &type : post)
    {
        if (type >=  0)
        {
            idxtOrder.push_back(hardblocktype[type]);
        }
        else
        {
            auto node = cutidx[cutIdx++];
            node->type = type;
            node->right = idxtOrder.back();
            idxtOrder.pop_back();
            node->left = idxtOrder.back();
            idxtOrder.pop_back();
            idxtOrder.push_back(node);
            node->update();
        }
    }
    return idxtOrder.back();
}

void SA::position(Node *node, int &choice, int const &x, int const &y)
{
    if (node->type ==  0)
    {
        //cout<< "0: "<< node->direc[choice][0]<<endl;
        //cout<< "1: "<< node->direc[choice][1]<<endl;
        int margin_x = node->direc[choice][0] ;
        int margin_y = node->direc[choice][1] ;
        node->hardblock->x = x;
        node->hardblock->y = y;
        if(node->hardblock->width >= margin_x && node->hardblock->height >= margin_y){
            node->hardblock->isRotated = false;
        }
        else{
            node->hardblock->isRotated = true;
        }
        node->hardblock->pin->x = x + margin_x / 2;
        node->hardblock->pin->y = y + margin_y / 2;

    }
    else
    {
        position(node->left, node->direc[choice][2], x, y); 

        int offsetX = 0, offsetY = 0;
        if (node->type == -2){
            offsetX = node->left->direc[node->direc[choice][2]][0];
            //cout<<"2 :"<<node->left->direc[node->direc[choice][2]][0]<<endl;
        }
        else{
            offsetY = node->left->direc[node->direc[choice][2]][1];
            //cout<<"3 :"<<node->left->direc[node->direc[choice][3]][0]<<endl;
        }
        int new_border_x = x + offsetX;
        int new_border_y = y + offsetY;
        //cout<<"3 :"<<node->direc[choice][3]<<endl;
        position(node->right, node->direc[choice][3], new_border_x, new_border_y);
    }
}
int SA::calCost( vector<int>  &post, bool const &focusWirelength)
{
    auto root = buildTree(post);
    int area = 0, minArea = 0, choice = 0, num = root->direc.size();
    for (int i = 0; i < num; ++i)
    {
        auto &direc = root->direc[i];
        int de0 = direc[0] - outline;
        int de1 = direc[1] - outline;
        if (de0 > 0 && de1 > 0)
            area = direc[0] * direc[1] - outline * outline;
        else if (de0 <= 0 && de1 >0)
            area = outline * de1;
        else if (de0 > 0 && de1 <= 0)
            area = de0 * outline;
        else
            area = 0;
        if(i == 0){
            minArea = area;
        }
        else if (minArea > area)
        {
            minArea = area;
            choice = i;
        }
    }

    if (focusWirelength == false)
        return minArea * 10;


    position(root, choice, 0, 0);
    int wirelength = 0;
    for (auto  &net : input->nets){ 
        wirelength += net->HPWL();
    }
    //cout<<"result "<< wirelength<<endl;
    return minArea * 10 + wirelength;
}

 vector<int> SA::algorithm(double const &c, double const &r, int const &k, vector<int> const &post, bool const &WL)
{
    int wirelength = 0;
    for (auto  &net : input->nets){ 
        wirelength += net->HPWL();
    }
     cout << "Initialized\n"
              << "wirelength: " << wirelength << "\n\n";
    int MT = 0, hill = 0, reject = 0, N = k * input->hardblocks.size();
     vector<int> BE, E;
    BE = E = post;
    int bestCost = 0, cost = 0;
    bestCost = cost = calCost(E, WL);

    do
    {
        double T0 = 100;
        do
        {
            MT = hill = reject = 0;
            do
            {
                int type = 0;
                if (WL == false)
                    type = rand() % 3;
                 vector<int> NE = perturb(E, type);

                MT += 1;
                int newCost = calCost(NE, WL);
                if (newCost - cost < 0 || (double)rand() / RAND_MAX < exp(-1 * (newCost - cost) / T0))
                {
                    if (newCost - cost > 0)
                        hill += 1;

                    E = NE;
                    cost = newCost;
                    if (cost < bestCost)
                    {
                        BE = E;
                        bestCost = cost;
                        
                        if (bestCost == 0){
                            calCost(BE, true);
                            return BE;
                        } 
                        
                    }
                }
                else
                {
                    reject += 1;
                }
            }while (hill <= N && MT <= 2 * N);
            T0 = r * T0;
        } while (reject / MT <= 0.95 && T0 >= c);
    } while (WL == false);
    calCost(BE, true);
    wirelength = 0;
    for (auto  &net : input->nets){
        wirelength += net->HPWL();
    }

     cout << "find a possible solution\n"
              << "wirelength: " << wirelength << "\n\n";

    return BE;
}

SA::SA(SAInput *input)
    : input(input)
{
    int num = input->hardblocks.size();
    int Area = 0;
    for (int i = 0; i < num; ++i)
    {
        auto hardblock = input->hardblocks[i];
        hardblocktype.push_back(new Node( 0, hardblock));
        Area += hardblock->width * hardblock->height;
    }
    outline = sqrt(Area * (1 + input->deadSpaceRatio));

    for (int i = 0; i < num - 1; ++i)
        cutidx.push_back(new Node(-2));
}

ResultWriter *SA::solve()
{
    int seed = 0;
    srand(100);

    vector<int> post;
    int rowWidth = 0, xCnt = 0, yCnt = 0, cCnt = 0, num = input->hardblocks.size();
    for (int i = 0; i < num; ++i)
    {
        auto hardblock = input->hardblocks[i];
        rowWidth += hardblock->width;
        if (rowWidth >= outline)
        {
            yCnt += 1;
            if (yCnt >= 2)
            {
                post.push_back(-1);
                cCnt += 1;
                yCnt = 1;
            }
            rowWidth = hardblock->width;
            xCnt = 0;
        }
        post.push_back(i);

        xCnt += 1;
        if (xCnt >= 2)
        {
            post.push_back(-2);
            cCnt += 1;
            xCnt = 1;
        }
    }

    for (int i = cCnt; i < num - 1; ++i)
        post.push_back(-1);
    post = algorithm(0.1, 0.9, 10, post, false);

    int wirelength = 0;
        for (auto  &net : input->nets)
            wirelength += net->HPWL();
     cout << "SA atart\n";
    algorithm(1, 0.95, 5, post, true);
    wirelength = 0;

    return new ResultWriter(input);
}


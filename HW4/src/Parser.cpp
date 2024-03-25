// #include "Parser.hpp"
#include <unordered_map>
#include "Data.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
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


void Parser::read(string const &filename1)
{
    string i, buff, t;
    ifstream fin(filename1);
    unordered_map<string, Cell *> strToCell;

    while (getline(fin, buff))
    {
        if (buff.empty())
            continue;

        stringstream input(buff);
        input >> i;
        if (i == "RowBasedPlacement")
        {
            input >> t >> this->nodeFile >> this->plFile >> this->sclFile;
        }
        else if (i == "MaxDisplacement")
        {
            input >> t >> this->maxDisplacement;
        }
    }


    string filename = filename1.substr(0, filename1.find_last_of('/'));
    ifstream fin2(filename+ '/' + this->nodeFile);
    int nodeNum = 0, terminalNum = 0;
    while (getline(fin2, buff))
    {
        if (buff.empty())
            continue;

        stringstream buffer(buff);
        
        buffer >> i;
        if (i == "NumNodes")
        {
            buffer >> t >> nodeNum;
        }
        else if (i == "NumTerminals")
        {
            buffer >> t >> terminalNum;
            break;
        }
    }
    getline(fin2, buff);
        string n;
        int w = 0, h = 0;
    int const cellNum = nodeNum - terminalNum;
    for (int i = 0; i < nodeNum; ++i)
    {
        getline(fin2, buff);
        stringstream buffStream(buff);

        buffStream >> n >> w >> h;
        auto cell = new Cell(n, w, h);
        if (i < cellNum)
            this->cells.push_back(cell);
        else
            this->terminals.push_back(cell);
        strToCell.emplace(n, cell);
    }

   ifstream fin3(filename  + '/' + this->plFile);
    int size = this->cells.size()+this->terminals.size();

    for (int i = 0; i < size; ++i)
    {
        getline(fin3, buff);
        stringstream buffStream(buff);
        string name;
        double x = 0, y = 0;
        buffStream >> name >> x >> y;
        strToCell.at(name)->x = x;
        strToCell.at(name)->y = y;
    }
       ifstream fin4(filename + '/' + this->sclFile);
    int rowNum = 0;
    // string buff;
    string id;
    while (getline(fin4, buff))
    {
        if (buff.empty())
            continue;

        stringstream buffer(buff);
        
        buffer >> id;
        if (id == "NumRows")
        {
            buffer >> t >> rowNum;
            break;
        }
    }
    for (int i = 0; i < rowNum; ++i)
    {
        int y = 0, h = 0, siteWidth = 0, x = 0, siteNum = 0;
        while (getline(fin4, buff))
        {
            if (buff.empty())
                continue;

            stringstream buffer(buff);
            
            buffer >> id;
            if (id == "Coordinate")
            {
                buffer >> t >> y;
            }
            else if (id == "Height")
            {
                buffer >> t >> h;
            }
            else if (id == "Sitewidth")
            {
                buffer >> t >> siteWidth;
            }
            else if (id == "NumSites")
            {
                buffer >> t >> siteNum;
            }
            else if (id == "SubrowOrigin")
            {
                buffer >> t >> x;
            }

            else if (id == "End")
            {
                auto row = new Row(siteWidth, h, y);
                row->subRows.push_back(new SubRow(x, x + siteWidth * siteNum));
                this->rows.push_back(row);
                break;
            }
        }
    }
}


LegalizerInput *Parser::parse(char argv[])
{
    string auxFilePath(argv);
    read(auxFilePath);

    return new LegalizerInput(this->maxDisplacement, this->cells, this->terminals, this->rows);
}
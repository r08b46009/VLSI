#include "Parser.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

using std::getline;
using std::ifstream;
using std::runtime_error;
using std::stringstream;
using std::string;
using std::unordered_map;

void Parser::read(const string& filename1) {
    ifstream fin(filename1);
    if (!fin) {
        throw runtime_error("Failed to open aux file: " + filename1);
    }

    string token, buff, t;
    unordered_map<string, Cell*> strToCell;

    while (getline(fin, buff)) {
        if (buff.empty()) {
            continue;
        }
        stringstream input(buff);
        input >> token;
        if (token == "RowBasedPlacement") {
            input >> t >> nodeFile >> plFile >> sclFile;
        } else if (token == "MaxDisplacement") {
            input >> t >> maxDisplacement;
        }
    }

    const string baseDir = filename1.substr(0, filename1.find_last_of('/'));

    ifstream fin2(baseDir + '/' + nodeFile);
    if (!fin2) {
        throw runtime_error("Failed to open nodes file: " + baseDir + '/' + nodeFile);
    }

    int nodeNum = 0;
    int terminalNum = 0;
    while (getline(fin2, buff)) {
        if (buff.empty()) {
            continue;
        }
        stringstream buffer(buff);
        buffer >> token;
        if (token == "NumNodes") {
            buffer >> t >> nodeNum;
        } else if (token == "NumTerminals") {
            buffer >> t >> terminalNum;
            break;
        }
    }

    getline(fin2, buff);  // skip one line after header

    const int cellNum = nodeNum - terminalNum;
    for (int i = 0; i < nodeNum; ++i) {
        getline(fin2, buff);
        stringstream buffStream(buff);

        string name;
        int w = 0;
        int h = 0;
        buffStream >> name >> w >> h;
        auto* cell = new Cell(name, w, h);
        if (i < cellNum) {
            cells.push_back(cell);
        } else {
            terminals.push_back(cell);
        }
        strToCell.emplace(name, cell);
    }

    ifstream fin3(baseDir + '/' + plFile);
    if (!fin3) {
        throw runtime_error("Failed to open placement file: " + baseDir + '/' + plFile);
    }

    const int totalObjects = static_cast<int>(cells.size() + terminals.size());
    for (int i = 0; i < totalObjects; ++i) {
        getline(fin3, buff);
        stringstream buffStream(buff);
        string name;
        double x = 0;
        double y = 0;
        buffStream >> name >> x >> y;
        strToCell.at(name)->x = x;
        strToCell.at(name)->y = y;
    }

    ifstream fin4(baseDir + '/' + sclFile);
    if (!fin4) {
        throw runtime_error("Failed to open row file: " + baseDir + '/' + sclFile);
    }

    int rowNum = 0;
    while (getline(fin4, buff)) {
        if (buff.empty()) {
            continue;
        }
        stringstream buffer(buff);
        buffer >> token;
        if (token == "NumRows") {
            buffer >> t >> rowNum;
            break;
        }
    }

    for (int i = 0; i < rowNum; ++i) {
        int y = 0;
        int h = 0;
        int siteWidth = 0;
        int x = 0;
        int siteNum = 0;
        while (getline(fin4, buff)) {
            if (buff.empty()) {
                continue;
            }
            stringstream buffer(buff);
            buffer >> token;
            if (token == "Coordinate") {
                buffer >> t >> y;
            } else if (token == "Height") {
                buffer >> t >> h;
            } else if (token == "Sitewidth") {
                buffer >> t >> siteWidth;
            } else if (token == "NumSites") {
                buffer >> t >> siteNum;
            } else if (token == "SubrowOrigin") {
                buffer >> t >> x;
            } else if (token == "End") {
                auto* row = new Row(siteWidth, h, y);
                row->subRows.push_back(new SubRow(x, x + siteWidth * siteNum));
                rows.push_back(row);
                break;
            }
        }
    }
}

LegalizerInput* Parser::parse(const char* auxPath) {
    read(string(auxPath));
    return new LegalizerInput(maxDisplacement, cells, terminals, rows);
}

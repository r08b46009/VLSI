
#include "abacus.hpp"
#include "Parser.hpp"
#include "Data.hpp"
#include <iostream>
#include <ctime>
#include <fstream>
using namespace std;



int main(int argc, char **argv)
{
    clock_t loadTime1, loadTime2, ecTime1, ecTime2, outputTime1, outputTime2;
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <aux file>\n";
        return 0;
    }
    cout<< "1331"<<endl;
    loadTime1 = clock();
    Parser parser;
    auto input = parser.parse(argv[1]);
    loadTime2 = clock();
    
    ecTime1 = clock();
    abacus abacus(input);

    ecTime2 = clock();
    outputTime1 = clock();



    cout<<"123"<<endl;
    string auxFilePath(argv[1]);
    ofstream fout(argv[2]);
    for (auto const &cell : input->cells)
        fout << cell->n << ' '<< (cell->optimalX) << ' '<< (cell->optimalY) << " : N\n";
    for (auto const &terminal : input->terminals)
        fout << terminal->n << ' '<< (terminal->x) << ' '<< (terminal->y) << " : N /FIXED\n";
    
    // result->write(argv);
    outputTime2 = clock();
    cout<<"loading:" <<double(loadTime2-loadTime1)/CLOCKS_PER_SEC<<endl;
    cout<<"abacus process:" <<double(ecTime2-ecTime1)/CLOCKS_PER_SEC<<endl;
    cout<<"ouput:" <<double(outputTime2-outputTime1)/CLOCKS_PER_SEC<<endl;


    return 0;
}

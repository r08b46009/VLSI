#include "Parser.hpp"
#include "abacus.hpp"

#include <ctime>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    clock_t loadTime1, loadTime2, execTime1, execTime2, outputTime1, outputTime2;
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <aux file> <output file>\n";
        return 1;
    }

    loadTime1 = clock();
    Parser parser;
    auto* input = parser.parse(argv[1]);
    loadTime2 = clock();

    execTime1 = clock();
    Abacus legalizer(input);
    execTime2 = clock();

    outputTime1 = clock();
    std::ofstream fout(argv[2]);
    if (!fout) {
        std::cerr << "Failed to open output file: " << argv[2] << "\n";
        return 1;
    }

    for (const auto* cell : input->cells) {
        fout << cell->n << ' ' << cell->optimalX << ' ' << cell->optimalY << " : N\n";
    }
    for (const auto* terminal : input->terminals) {
        fout << terminal->n << ' ' << terminal->x << ' ' << terminal->y << " : N /FIXED\n";
    }
    outputTime2 = clock();

    std::cout << "loading: " << double(loadTime2 - loadTime1) / CLOCKS_PER_SEC << '\n';
    std::cout << "abacus process: " << double(execTime2 - execTime1) / CLOCKS_PER_SEC << '\n';
    std::cout << "output: " << double(outputTime2 - outputTime1) / CLOCKS_PER_SEC << '\n';

    return 0;
}

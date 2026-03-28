#include "SA.hpp"

#include <ctime>
#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 6) {
        std::cerr << "Usage: ./hw3 <hardblocks_file> <nets_file> <pl_file> "
                  << "<output_floorplan> <dead_space_ratio>\n";
        return 1;
    }

    try {
        const clock_t load_start = clock();
        Parser parser;
        SAInput* input = parser.parse(argv);
        const clock_t load_end = clock();

        const clock_t sa_start = clock();
        SA solver(input);
        ResultWriter* result = solver.solve();
        const clock_t sa_end = clock();

        int wirelength = 0;
        for (std::vector<Net*>::const_iterator it = result->input->nets.begin();
             it != result->input->nets.end();
             ++it) {
            wirelength += (*it)->HPWL();
        }

        const clock_t output_start = clock();
        result->write(argv[4], wirelength);
        const clock_t output_end = clock();

        std::cout << "WL " << wirelength << std::endl;
        std::cout << "loading: "
                  << static_cast<double>(load_end - load_start) / CLOCKS_PER_SEC
                  << std::endl;
        std::cout << "SA process: "
                  << static_cast<double>(sa_end - sa_start) / CLOCKS_PER_SEC
                  << std::endl;
        std::cout << "output: "
                  << static_cast<double>(output_end - output_start) / CLOCKS_PER_SEC
                  << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

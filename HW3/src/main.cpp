
#include "SA.hpp"
#include <iostream>
#include <ctime>
using namespace std;

int main(int argc, char *argv[])
{
    clock_t IOtime, Exectime, loadTime1, loadTime2, ecTime1, ecTime2, outputTime1, outputTime2, StartTime, FinishTime;
    int WL = 0;
    loadTime1 = clock();
    Parser parser;
    auto input = parser.parse(argv);
    loadTime2 = clock();

    ecTime1 = clock();
    SA solver(input);
    WL =0;

    auto result = solver.solve();

    WL = 0;
    for (auto  &net : input->nets)
        WL += net->HPWL();
        cout<<"WL"<<WL<<endl;


    ecTime2 = clock();

    outputTime1 = clock();
    WL = 0;
    for (auto  &net : result->input->nets)
            WL += net->HPWL();
    cout<<WL<<endl;
    result->write(argv[4], WL);
    outputTime2 = clock();


    cout<<"loading:" <<double(loadTime2-loadTime1)/CLOCKS_PER_SEC<<endl;
    cout<<"SA process:" <<double(ecTime2-ecTime1)/CLOCKS_PER_SEC<<endl;
    cout<<"ouput:" <<double(outputTime2-outputTime1)/CLOCKS_PER_SEC<<endl;

    return 0;
}

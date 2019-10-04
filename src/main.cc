#include "Controller.hh"
#include <cstdlib>
#include <ctime>
#include <iostream>

int main (int argc, char* argv[])
{
    unsigned int seed (time(nullptr));
    srandom(seed);

    Controller c(false);

    c.RunSimulation();
    c.RetrieveResults();

    std::cout << "random seed: " << seed;
    return 0;
}
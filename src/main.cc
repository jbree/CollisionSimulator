#include "Controller.hh"
#include <cstdlib>
#include <ctime>
#include <iostream>

int main (int argc, char* argv[])
{
    int vcs(false);
    const char* inputFile = "setup.txt";
    unsigned int seed (time(nullptr));

    for (int i(0); i < argc; i++) {
        if (strcmp(argv[i], "--vcs") == 0) {
            vcs = true;
        }

        if (strcmp(argv[i], "--input") == 0) {
            inputFile = argv[i+1];
        }

        if (strcmp(argv[i], "--seed") == 0) {
            seed = strtoul(argv[i+1], 0, 0);
        }
    }

    srandom(seed);

    Controller c(vcs, inputFile);

    c.RunSimulation();
    c.RetrieveResults();

    std::cout << "random seed: " << seed << std::endl
            << "input file: " << inputFile << std::endl;

    return 0;
}
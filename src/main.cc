#include "Controller.hh"

int main (int argc, char* argv[])
{
    Controller c(false);
    srandom(time(nullptr));

    c.RunSimulation();

    return 0;
}
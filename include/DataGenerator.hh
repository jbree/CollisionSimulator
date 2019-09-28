#pragma once

#include <list>
#include <cstdlib>

static const int MAX_SIMULATION_TICKS{ 1000000 };

class DataGenerator {

public:
	std::list<int> createArrivalTimes(int lambda);
};
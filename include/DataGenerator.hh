#pragma once

#include <list>
#include <cstdlib>

static const int MAX_SIMULATION_TICKS{ 1000000 };

const std::list<std::pair<int, int>> lambdaPairList = {
	std::make_pair(50, 50),
	std::make_pair(100, 100),
	std::make_pair(200, 200),
	std::make_pair(300, 300),
	std::make_pair(500, 500),
	std::make_pair(50, 10),
	std::make_pair(100, 200),
	std::make_pair(200, 400),
	std::make_pair(300, 600),
	std::make_pair(500, 1000),
};

class DataGenerator {

public:
	std::list<int> createArrivalTimes(int lambda);
};
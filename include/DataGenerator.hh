#pragma once

#include <list>

static const int MAX_SIMULATION_TICKS{ 1000000 };

const std::list<std::pair<int, int>> lambdaPairList = {
	std::make_pair(50, 50),
	std::make_pair(100, 100),
	std::make_pair(200, 200),
	std::make_pair(300, 300),
	std::make_pair(500, 500),
	std::make_pair(100, 50),
	std::make_pair(200, 100),
	std::make_pair(400, 200),
	std::make_pair(600, 300),
	std::make_pair(1000, 500),
};

class DataGenerator {

public:
	std::list<int> createArrivalTimes(int lambda);
};
#include "DataGenerator.hh"

#include <chrono>

static const int MAX_SIMULATION_TICKS{ 1000000 }; //number of 10 micro-s ticks for 10 seconds of simulation

std::list<int> DataGenerator::createArrivalTimes(int lambda)
{
	int tickCounter = 0; //number of 10 micro-s ticks
	double uniGenValue = 0; //uniformly generated value
	double expDistNum = 0; //exponentially distributed number
	std::list<int> arrivalTimes;

	srand(time(nullptr));

	//generate a random number between 0 and 1, this equation goes up to 5 significant digits
	uniGenValue = (rand() % 99999 + 1) / 100000.0;

	//calculate exponentially distributed number, convert to 10 micro-s units, 10^5 is to scale to 10 micro-s units
	expDistNum = floor((-1 / (float)lambda) * log(1 - uniGenValue) * pow(10.0, 5.0));

	//check if the tick counter is within the simulation time
	while (tickCounter < MAX_SIMULATION_TICKS)
	{
		//store frame arrival time
		arrivalTimes.push_back(tickCounter);

		uniGenValue = (rand() % 999 + 1) / 1000.0;

		expDistNum = floor((-1 / (float)lambda) * log(1 - uniGenValue) * pow(10, 5));

		//compute next frame arrival time
		tickCounter += expDistNum;
	}

	return arrivalTimes;
}

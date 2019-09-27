#pragma once


#include "Station.hh"

class Controller {
public:
	Controller(bool virtualCarrierSensingEnabled);

	void RunSimulation();

private:
	std::list<Station> stationList;
};
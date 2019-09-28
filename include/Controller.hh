#pragma once

#include "DataGenerator.hh"
#include "Station.hh"
#include "Medium.hh"
#include "Packet.hh"

#include <fstream>
#include <string>
#include <map>

class Controller {
public:
	Controller(bool virtualCarrierSensingEnabled);

	void RunSimulation();

private:
	DataGenerator generator;
	std::list<Station> stationList;
	//std::map<std::string, Station> initStationMap;


	std::map<std::string, std::pair<std::list<int>, std::string>> packetArrivalMap;
	//std::map <std::string, std::pair<Station, std::list<int>>> txStationMap;
};
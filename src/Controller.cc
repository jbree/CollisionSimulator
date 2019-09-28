
#include "Controller.hh"
#include "Medium.hh"
#include "Packet.hh"

#include <fstream>
#include <string>
#include <map>

Controller::Controller(bool virtualCarrierSensingEnabled)
{
	//import set of mediums into station contructor
	std::ifstream inFile("setup.txt");

	std::string stationName;
	std::string mediumName;
	std::string transmitMarker;
	std::string targetStation;
	std::string lambdaValue;

	std::pair<std::list<int>, std::string> packetTargetStationPair;

	std::map<std::string, std::list<std::string>> stationMap;
	std::map<std::string, std::list<std::string>>::iterator itStation = stationMap.begin();

	std::map<std::string, std::shared_ptr<Medium>> mediumMap;
	std::list<std::string>::iterator itMedium;
	std::set<std::shared_ptr<Medium>> mediumSet;

	/*
	File format is expected to be something like:
	A 1 T B 100
	B 1
	C 1 T D 200
	D 1
	*/
	if (inFile.is_open())
	{
		while (!inFile.eof())
		{
			inFile >> stationName >> mediumName >> transmitMarker >> targetStation >> lambdaValue;

			if (stationMap.count(stationName) == 0)
			{
				std::list<std::string> mediumList;
				mediumList.push_back(mediumName);

				stationMap[stationName] = mediumList;
			}
			else
			{
				stationMap[stationName].push_back(mediumName);
			}

			if (mediumMap.count(mediumName) == 0)
			{
				std::shared_ptr<Medium> mediumPtr(new Medium(mediumName));
				mediumMap[mediumName] = mediumPtr;
			}

			if (transmitMarker == "T")
			{
				packetTargetStationPair = std::make_pair(generator.createArrivalTimes(stoi(lambdaValue)), targetStation);
				packetArrivalMap[stationName] = packetTargetStationPair;
			}
		}
	}

	while (itStation != stationMap.end())
	{
		itMedium = itStation->second.begin();
		while (itMedium != itStation->second.end())
		{
			mediumSet.insert(mediumMap[itMedium->front]);
		}

		Station station(itStation->first, mediumSet, virtualCarrierSensingEnabled);
		//initStationMap[itStation->first] = station;
		stationList.push_back(station);

		/*std::pair<Station, std::list<int>> stationPacketPair(station, packetArrivalMap[itStation->first]);

		if (packetArrivalMap.count(itStation->first) != 0)
		{
			txStationMap[itStation->first] = stationPacketPair;
		}*/
	}
}

void Controller::RunSimulation()
{
	int tickCounter = 0; //number of 10 micro-s ticks
	Packet packetToSend;

	std::map<std::string, std::pair<std::list<int>, std::string>>::iterator itPacketArrivals = packetArrivalMap.begin();

	while (tickCounter < MAX_SIMULATION_TICKS)
	{
		while (itPacketArrivals != packetArrivalMap.end())
		{
			if (packetArrivalMap[itPacketArrivals->first].first.front == tickCounter)
			{
				//packet arrives at station

				//do algorithm, if free, send to corresponding medium
				//use packetArrivalMap[itPacketArrivals->first].second to get destination for the packet
				
				packetToSend.src = itPacketArrivals->first;
				packetToSend.dst = packetArrivalMap[itPacketArrivals->first].second;
				//packetToSend.type = ;
				//packetToSend.PACKET_SIZE = ;
				//packetToSend.size = ;

				//remove front of list
				packetArrivalMap[itPacketArrivals->first].first.front.pop_front();
			}
		}

		tickCounter++;
	}
}

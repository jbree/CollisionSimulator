
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

	std::list<std::string>::iterator itMediums;
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
				std::list<std::string> mediumNameList;
				mediumNameList.push_back(mediumName);

				stationMap[stationName] = mediumNameList;
			}
			else
			{
				stationMap[stationName].push_back(mediumName);
			}

			if (mediumMap.count(mediumName) == 0)
			{
				std::shared_ptr<Medium> mediumPtr(new Medium(mediumName));
				mediumMap[mediumName] = mediumPtr;
				//mediumList.push_back(mediumPtr);
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
		itMediums = itStation->second.begin();
		while (itMediums != itStation->second.end())
		{
			mediumSet.insert(mediumMap.at(*itMediums));
		}

		//initStationMap[itStation->first] = station;
		// emplace instead of insert because Station doesn't have valid copy constructor
		stationList.emplace_back(itStation->first, mediumSet, virtualCarrierSensingEnabled);

		/*std::pair<Station, std::list<int>> stationPacketPair(station, packetArrivalMap[itStation->first]);

		if (packetArrivalMap.count(itStation->first) != 0)
		{
			txStationMap[itStation->first] = stationPacketPair;
		}*/

		itStation++;
	}
}

void Controller::RunSimulation()
{
	int tickCounter = 0; //counter for number of 10 micro-s ticks
	Packet packetToSend;

	std::map<std::string, std::pair<std::list<int>, std::string>>::iterator itPacketArrivals = packetArrivalMap.begin();
	std::list<Station>::iterator itStationList;

	std::map<std::string, std::shared_ptr<Medium>>::iterator itMediums;

	while (tickCounter < MAX_SIMULATION_TICKS)
	{
		//ticks, mediums first, then stations
		itMediums = mediumMap.begin();
		while (itMediums != mediumMap.end())
		{
			itMediums->second->tick();
			itMediums++;
		}

		itStationList = stationList.begin();
		while (itStationList != stationList.end())
		{
			itStationList->tick();
			itStationList++;
		}

		while (itPacketArrivals != packetArrivalMap.end())
		{
			if (itPacketArrivals->second.first.front() == tickCounter)
			{
				//packet arrives at station

				//do algorithm, if free, send to corresponding medium
				//use packetArrivalMap[itPacketArrivals->first].second to get destination for the packet
				
				packetToSend.src = itPacketArrivals->first;
				packetToSend.dst = itPacketArrivals->second.second;
				packetToSend.type = PacketType::Data;
				//packetToSend.PACKET_SIZE = ;
				packetToSend.size = packetToSend.PACKET_SIZE.at(packetToSend.type);

				//have data packet arrive at transmit station
				itStationList = stationList.begin();
				while (itStationList != stationList.end())
				{
					if (itStationList->name() == itPacketArrivals->first)
					{
						itStationList->arrive(packetToSend);
					}

					itStationList++;
				}

				//remove front of list
				itPacketArrivals->second.first.pop_front();
			}

			itPacketArrivals++;
		}

		//tocks, mediums first, then stations
		itMediums = mediumMap.begin();
		while (itMediums != mediumMap.end())
		{
			itMediums->second->tock();
			itMediums++;
		}

		itStationList = stationList.begin();
		while (itStationList != stationList.end())
		{
			itStationList->tock();
			itStationList++;
		}

		itPacketArrivals = packetArrivalMap.begin();
		tickCounter++;
	}
}

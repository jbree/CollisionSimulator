
#include "Controller.hh"
#include "Medium.hh"
#include "Packet.hh"

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

Controller::Controller(bool virtualCarrierSensingEnabled)
{
	//import set of mediums into station contructor
	std::ifstream inFile("setup.txt");

	std::pair<std::list<int>, std::string> packetTargetStationPair;

	std::map<std::string, std::list<std::string>> stationMap;

	std::list<std::string>::iterator itMediums;
	std::set<std::shared_ptr<Medium>>::iterator itMediumPtrs;
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
		std::string line;
		while (std::getline(inFile, line))
		{
			std::string stationName;
			std::string mediumName;
			std::string transmitMarker;
			std::string targetStation;
			std::string lambdaValue;
			std::istringstream stream(line);

			stream >> stationName >> mediumName >> transmitMarker >> targetStation >> lambdaValue;

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
			}

			if (transmitMarker == "T")
			{
				packetTargetStationPair = std::make_pair(generator.createArrivalTimes(stoi(lambdaValue)), targetStation);
				packetArrivalMap[stationName] = packetTargetStationPair;
			}
		}
	}

	std::map<std::string, std::list<std::string>>::iterator itStation = stationMap.begin();
	while (itStation != stationMap.end())
	{
		itMediums = itStation->second.begin();
		while (itMediums != itStation->second.end())
		{
			mediumSet.insert(mediumMap.at(*itMediums));

			itMediums++;
		}

		// emplace instead of insert because Station doesn't have valid copy constructor

		itMediumPtrs = mediumSet.begin();
		if (packetArrivalMap.count(itStation->first) != 0)
		{
			std::shared_ptr<SenderStation> stationPtrShared(new SenderStation(itStation->first, mediumSet, virtualCarrierSensingEnabled));

			senderList.emplace_back(stationPtrShared);

			while (itMediumPtrs != mediumSet.end())
			{
				std::weak_ptr<Station> stationPtr = stationPtrShared;
				(*itMediumPtrs)->addStation(stationPtr);

				itMediumPtrs++;
			}
		}
		else
		{
			std::shared_ptr<ReceiverStation> stationPtrShared(new ReceiverStation(itStation->first, mediumSet));

			receiverList.emplace_back(stationPtrShared);

			while (itMediumPtrs != mediumSet.end())
			{
				std::weak_ptr<Station> stationPtr = stationPtrShared;
				(*itMediumPtrs)->addStation(stationPtr);

				itMediumPtrs++;
			}
		}

		itStation++;
	}
}

void Controller::RunSimulation()
{
	int tickCounter = 0; //counter for number of 10 micro-s ticks
	Packet packetToSend;

	for (auto station: senderList) {
		std::cout << "sender: " << station->name() << std::endl;
	}

	for (auto station: receiverList) {
		std::cout << "receiver: " << station->name() << std::endl;
	}

	std::map<std::string, std::pair<std::list<int>, std::string>>::iterator itPacketArrivals = packetArrivalMap.begin();
	std::list<std::shared_ptr<SenderStation>>::iterator itSenderList;
	std::list<std::shared_ptr<ReceiverStation>>::iterator itReceiverList;

	std::map<std::string, std::shared_ptr<Medium>>::iterator itMediums;

	//should do for each lambda combination
	// const uint32_t MAX_SIMULATION_TICKS(5000);
	while (tickCounter < MAX_SIMULATION_TICKS)
	{
		// std::cout << "tick " << tickCounter << std::endl;
		//ticks, mediums first, then stations
		itMediums = mediumMap.begin();
		while (itMediums != mediumMap.end())
		{
			itMediums->second->tick();
			itMediums++;
		}

		itSenderList = senderList.begin();
		while (itSenderList != senderList.end())
		{
			(*itSenderList)->tick();
			itSenderList++;
		}

		itReceiverList = receiverList.begin();
		while (itReceiverList != receiverList.end())
		{
			(*itReceiverList)->tick();
			itReceiverList++;
		}

		while (itPacketArrivals != packetArrivalMap.end())
		{
			while (itPacketArrivals->second.first.front() == tickCounter)
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
				itSenderList = senderList.begin();
				while (itSenderList != senderList.end())
				{
					if ((*itSenderList)->name() == itPacketArrivals->first)
					{
						(*itSenderList)->arrive(packetToSend);
					}

					itSenderList++;
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

		itSenderList = senderList.begin();
		while (itSenderList != senderList.end())
		{
			(*itSenderList)->tock();
			itSenderList++;
		}

		itReceiverList = receiverList.begin();
		while (itReceiverList != receiverList.end())
		{
			(*itReceiverList)->tock();
			itReceiverList++;
		}

		itPacketArrivals = packetArrivalMap.begin();
		tickCounter++;
	}

	for (auto packetList: packetArrivalMap) {
		std::cout << packetList.first << " has "
				<< packetList.second.first.size() << " remaining "
				<< std::endl;
	}

	//call retrieve results to report findings, list lambda, throughput, collisions for all nodes
}

void Controller::RunSimulationAllLambdas(bool virtualCarrierSensingEnabled)
{
	int tickCounter = 0; //counter for number of 10 micro-s ticks
	Packet packetToSend;

	std::map<std::string, std::pair<std::list<int>, std::string>>::iterator itPacketArrivals = packetArrivalMap.begin();
	std::list<SenderStation>::iterator itSenderList;
	std::list<ReceiverStation>::iterator itReceiverList;

	std::map<std::string, std::shared_ptr<Medium>>::iterator itMediums;
	std::list<std::pair<int, int>>::const_iterator itLambdas = lambdaPairList.begin();

	//should do for each lambda combination

	/*while (itLambdas != lambdaPairList.end())
	{
		Controller((bool)virtualCarrierSensingEnabled);
		//change up stations A, C's packetArrivalMap with lambdaPairList
		packetArrivalMap["A"] = std::make_pair(generator.createArrivalTimes(itLambdas->first), packetArrivalMap["A"].second);
		packetArrivalMap["C"] = std::make_pair(generator.createArrivalTimes(itLambdas->first), packetArrivalMap["C"].second);
		
		while (tickCounter < MAX_SIMULATION_TICKS)
		{
			//ticks, mediums first, then stations
			itMediums = mediumMap.begin();
			while (itMediums != mediumMap.end())
			{
				itMediums->second->tick();
				itMediums++;
			}

			itSenderList = senderList.begin();
			while (itSenderList != senderList.end())
			{
				itSenderList->tick();
				itSenderList++;
			}

			itReceiverList = receiverList.begin();
			while (itReceiverList != receiverList.end())
			{
				itReceiverList->tick();
				itReceiverList++;
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
					itSenderList = senderList.begin();
					while (itSenderList != senderList.end())
					{
						if (itSenderList->name() == itPacketArrivals->first)
						{
							itSenderList->arrive(packetToSend);
						}

						itSenderList++;
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

			itSenderList = senderList.begin();
			while (itSenderList != senderList.end())
			{
				itSenderList->tock();
				itSenderList++;
			}

			itReceiverList = receiverList.begin();
			while (itReceiverList != receiverList.end())
			{
				itReceiverList->tock();
				itReceiverList++;
			}

			itPacketArrivals = packetArrivalMap.begin();
			tickCounter++;
		}

		itLambdas++;
	}*/

	//call retrieve results to report findings, list lambda, throughput, collisions for all nodes

}

void Controller::RetrieveResults()
{
	std::list<SenderStation>::iterator itSenderList;
	std::list<ReceiverStation>::iterator itReceiverList;

	for (auto receiver: receiverList) {
		std::cout << receiver->name() <<" saw "
				<< receiver->collisions() << " collisions and "
				<< receiver->receivedPackets() << " successes" << std::endl; 
	}

	/*while (itSenderList != senderList.end())
	{
		//record byte arrival data (for throughput) and collision data
		//itSenderList->arrivedPackets (iterate through to count bytes) or itSenderList->receivedBytes, then count for Kb for throughput unit Kbps

		itSenderList++;
	}

	itReceiverList = receiverList.begin();
	while (itReceiverList != receiverList.end())
	{
		//record byte arrival data (for throughput) and collision data

		itReceiverList++;
	}*/
}

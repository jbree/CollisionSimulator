#pragma once

#include "DataGenerator.hh"
#include "Station.hh"
#include "SenderStation.hh"
#include "ReceiverStation.hh"
#include "Medium.hh"
#include "Packet.hh"

#include <fstream>
#include <string>
#include <map>

class Controller {
public:
	Controller(bool virtualCarrierSensingEnabled);

	void RunSimulation();

	void RunSimulationAllLambdas(bool virtualCarrierSensingEnabled);

	void RetrieveResults();
private:
	DataGenerator generator;

	std::list<SenderStation> senderList;
	std::list<ReceiverStation> receiverList;

	std::map<std::string, std::shared_ptr<Medium>> mediumMap;
	std::map<std::string, std::pair<std::list<int>, std::string>> packetArrivalMap;
};
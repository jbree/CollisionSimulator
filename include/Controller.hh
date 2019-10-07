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
#include <ctime>

class Controller {
public:
	Controller(
			bool virtualCarrierSensingEnabled,
			const std::string& inputFilename = "setup.txt"
			);

	void RunSimulation();

	void RunSimulationAllLambdas(bool virtualCarrierSensingEnabled);

	void RetrieveResults();
	void ResetSimulation(bool virtualCarrierSensingEnabled);
private:
	DataGenerator generator;

	std::list< std::shared_ptr<SenderStation>> senderList;
	std::list< std::shared_ptr<ReceiverStation>> receiverList;

	/*
		std::list<std::shared_ptr<SenderStation>> senderList;
		std::list<std::shared_ptr<ReceiverStation>> receiverList;*/

	std::map<std::string, std::shared_ptr<Medium>> mediumMap;
	std::map<std::string, std::pair<std::list<int>, std::string>> packetArrivalMap;
};
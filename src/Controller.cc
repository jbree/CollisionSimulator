#include "DataGenerator.hh"
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
	std::map<std::string, std::list<std::string>> stationMap;
	std::map<std::string, std::shared_ptr<Medium>> mediumMap;
	std::map<std::string, std::list<std::string>>::iterator itStation = stationMap.begin();

	std::list<std::string>::iterator itMedium;
	std::set<std::shared_ptr<Medium>> mediumSet;

	if (inFile.is_open())
	{
		while (!inFile.eof())
		{
			inFile >> stationName >> mediumName;

			if (mediumMap.count(mediumName) == 0)
			{
				std::shared_ptr<Medium> mediumPtr(new Medium(mediumName));
				mediumMap[mediumName] = mediumPtr;
			}

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
		}
	}

	while (itStation != stationMap.end)
	{
		itMedium = itStation->second.begin();
		while (itMedium != itStation->second.end())
		{
			mediumSet.insert(mediumMap[itMedium->front]);
		}

		Station station(itStation->first, mediumSet, virtualCarrierSensingEnabled);
		stationList.push_back(station);
	}
}

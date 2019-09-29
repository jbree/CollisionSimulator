#include <gmock/gmock.h>
#include "Station.hh"

class MockStation: public Station {
public:
    MockStation (
            const std::string& name,
            const std::set<std::shared_ptr<Medium>>& media)
    : Station(name, media, false) {}
    
    MOCK_METHOD(bool, arrive, (const Packet& packet), (const, override));
    MOCK_METHOD(bool, receive, (const Packet& packet), (const, override));
};
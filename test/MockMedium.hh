#include <gmock/gmock.h>
#include "Medium.hh"

class MockMedium: public Medium {
public:
    MockMedium (const std::string& name)
    : Medium(name) {}
    
    MOCK_METHOD(void, transmit, (const Packet& p), (override));
    MOCK_METHOD(bool, isBusy, (), (const, override));
    MOCK_METHOD(void, addStation, (std::weak_ptr<Station> receiver), (override));
};
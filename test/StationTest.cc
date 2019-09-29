#include <gtest/gtest.h>
#include "Station.hh"
#include "MockMedium.hh"

using namespace ::testing;

TEST(StationTest, NamedByConstructor)
{
    Station s("TestStation", {}, false);
    ASSERT_EQ("TestStation", s.name());
}

TEST(StationTest, NoCallsUponArrival)
{
    auto medium = std::shared_ptr<StrictMock<MockMedium>>(new StrictMock<MockMedium>("MockMedium"));
    Station s("TestStation", {medium}, false);

    Packet p;
    p.dst = "TestStation";
    p.src = "Unknown";
    p.size = 1500;
    p.type = PacketType::Data;


    s.tick();
    s.arrive(p);
    s.tock();

    // for (size_t i(0); i < 4; i++) {
    //     s.tick();
    //     s.tock();
    //     EXPECT_CALL(*medium, isBusy())
    //         .WillOnce(::testing::Return(false));
    // }

}


TEST(StationTest, SenseBeforeTransmit)
{
    auto medium = std::shared_ptr<StrictMock<MockMedium>>(new StrictMock<MockMedium>("MockMedium"));
    Station s("TestStation", {medium}, false);

    Packet p;
    p.dst = "TestStation";
    p.src = "Unknown";
    p.size = 1500;
    p.type = PacketType::Data;

    s.tick();
    s.arrive(p);
    s.tock();

    EXPECT_CALL(*medium, isBusy())
            .Times(AtLeast(4));
    for (size_t i(0); i < 4; i++) {
        s.tick();
        s.tock();
    }

}


TEST(StationTest, AckAfterCollisionFree)
{
    auto medium = std::shared_ptr<StrictMock<MockMedium>>(new StrictMock<MockMedium>("MockMedium"));
    Station s("TestStation", {medium}, false);

    Packet p;
    p.dst = "TestStation";
    p.src = "Unknown";
    p.size = 1500;
    p.type = PacketType::Data;

    Packet ack;
    p.dst = "Unknown";
    p.src = "TestStation";
    p.size = 30;
    p.type = PacketType::Ack;

    for (size_t i(0); i < 100; i++) {
        s.tick();
        s.receive(p);
        s.tock();
    }

    // SIFS before ACK
    s.tick();
    s.tock();

    EXPECT_CALL(*medium, transmit(_))
            .Times(2);

    for (size_t i(0); i < 3; i++) {
        s.tick();
        s.tock();
    }
}
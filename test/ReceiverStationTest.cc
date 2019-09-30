#include <gtest/gtest.h>
#include "ReceiverStation.hh"
#include "MockMedium.hh"

using namespace ::testing;

TEST(ReceiverStationTest, NamedByConstructor)
{
    ReceiverStation s("TestStation", {});
    ASSERT_EQ("TestStation", s.name());
}


TEST(ReceiverStationTest, AckAfterCollisionFreeTransmit)
{
    auto medium = std::shared_ptr<StrictMock<MockMedium>>(new StrictMock<MockMedium>("MockMedium"));
    ReceiverStation s("TestStation", {medium});

    Packet p;
    p.dst = "TestStation";
    p.src = "Unknown";
    p.size = 1500;
    p.type = PacketType::Data;

    Packet ack;
    ack.dst = "Unknown";
    ack.src = "TestStation";
    ack.size = 30;
    ack.type = PacketType::Ack;

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

    EXPECT_EQ(s.receivedPackets(), 1);
}


TEST(ReceiverStationTest, IgnorePacketsDestinedForOtherReceiver)
{
    auto medium = std::shared_ptr<StrictMock<MockMedium>>(new StrictMock<MockMedium>("MockMedium"));
    ReceiverStation s("TestStation", {medium});

    Packet p;
    p.dst = "OtherStation";
    p.src = "Unknown";
    p.size = 1500;
    p.type = PacketType::Data;

    Packet ack;
    ack.dst = "Unknown";
    ack.src = "OtherStation";
    ack.size = 30;
    ack.type = PacketType::Ack;

    for (size_t i(0); i < 100; i++) {
        s.tick();
        s.receive(p);
        s.tock();
    }

    // SIFS before ACK
    s.tick();
    s.tock();

    EXPECT_CALL(*medium, transmit(_))
            .Times(0);

    for (size_t i(0); i < 3; i++) {
        s.tick();
        s.tock();
    }

    EXPECT_EQ(s.receivedPackets(), 0);
}


TEST(ReceiverStationTest, CollisionReported)
{
    auto medium = std::shared_ptr<StrictMock<MockMedium>>(new StrictMock<MockMedium>("MockMedium"));
    ReceiverStation s("TestStation", {medium});

    Packet p1;
    p1.dst = "RxStation1";
    p1.src = "Unknown";
    p1.size = 1500;
    p1.type = PacketType::Data;

    Packet p2;
    p2.dst = "RxStation2";
    p2.src = "Unknown";
    p2.size = 1500;
    p2.type = PacketType::Data;

    Packet ack;
    ack.dst = "Unknown";
    ack.src = "TestStation";
    ack.size = 30;
    ack.type = PacketType::Ack;

    for (size_t i(0); i < 100; i++) {
        s.tick();
        s.receive(p1);
        s.receive(p2);
        s.tock();
    }

    // SIFS before ACK
    s.tick();
    s.tock();

    EXPECT_CALL(*medium, transmit(_))
            .Times(0);

    for (size_t i(0); i < 3; i++) {
        s.tick();
        s.tock();
    }

    EXPECT_EQ(s.collisions(), 1);
    EXPECT_EQ(s.receivedPackets(), 0);
}

#include <gtest/gtest.h>
#include "SenderStation.hh"
#include "MockMedium.hh"

using namespace ::testing;

TEST(SenderStationTest, NamedByConstructor)
{
    SenderStation s("TestStation", {}, false);
    ASSERT_EQ("TestStation", s.name());
}

TEST(SenderStationTest, NoCallsUponArrival)
{
    auto medium = std::shared_ptr<StrictMock<MockMedium>>(new StrictMock<MockMedium>("MockMedium"));
    SenderStation s("TestStation", {medium}, false);

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


TEST(SenderStationTest, SenseBeforeTransmit)
{
    auto medium = std::shared_ptr<StrictMock<MockMedium>>(new StrictMock<MockMedium>("MockMedium"));
    SenderStation s("TestStation", {medium}, false);

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

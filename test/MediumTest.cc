#include <gtest/gtest.h>
#include "Medium.hh"
#include "Packet.hh"

TEST(MediumTest, IsBusyAfterTransmit)
{
    Medium m("TestMedium");
    Packet p;

    m.tick();
    m.transmit(p);
    m.tock();

    ASSERT_TRUE(m.isBusy());
}

TEST(MediumTest, BusyResetsAfterTick)
{
    Medium m("TestMedium");
    Packet p;

    m.tick();
    m.transmit(p);
    m.tock();

    ASSERT_TRUE(m.isBusy());

    m.tick();
    m.tock();

    ASSERT_FALSE(m.isBusy());
}

TEST(MediumTest, ThrowsOutOfPhase)
{
    Medium m("TestMedium");
    Packet p;

    m.tick();

    EXPECT_THROW(m.isBusy(), std::runtime_error);

    m.tock();

    EXPECT_THROW(m.transmit(p), std::runtime_error);

}
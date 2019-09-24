#include "Simulatable.hh"
#include <gtest/gtest.h>

TEST(SimulatableTest, clockStartsAtZero)
{
    Simulatable s;
    ASSERT_EQ(0ULL, s.clock());
}

TEST(SimulatableTest, tickTockIncrements)
{
    Simulatable s;
    s.tick();
    s.tock();
    EXPECT_EQ(1ULL, s.clock());
}

TEST(SimulatableTest, doubleTickThrows)
{
    Simulatable s;
    s.tick();
    EXPECT_ANY_THROW(s.tick());
}

TEST(SimulatableTest, doubleTockThrows)
{
    Simulatable s;
    s.tick();
    s.tock();
    EXPECT_ANY_THROW(s.tock());
}

TEST(SimulatableTest, active)
{
    Simulatable s;
    EXPECT_FALSE(s.active());
    s.tick();
    EXPECT_TRUE(s.active());
    s.tock();
    EXPECT_FALSE(s.active());
}
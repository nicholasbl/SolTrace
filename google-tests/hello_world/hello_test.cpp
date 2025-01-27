#include <gtest/gtest.h>

TEST(HelloTest, BasicAssertions)
{
	// EXPECT_STRNE = Expect String not equal
	EXPECT_STRNE("hello", "world");
	EXPECT_EQ(7 * 6, 42);
}
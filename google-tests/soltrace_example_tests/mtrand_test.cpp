#include <gtest/gtest.h>

#include "mtrand.h"

using namespace std;

TEST(RandomNumberGenerator, SingleNumberMersenneTwister)
{
	MTRand myrng(1);
	double random_number = myrng.rand();

	EXPECT_NEAR(random_number, 0.13387664401253274, 0.0000001);
}

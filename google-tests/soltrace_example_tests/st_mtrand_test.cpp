#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "mtrand.h"

using namespace std;

TEST(st_mtrand_test, BasicAssertions)
{
	MTRand myrng(1);
	double random_number = myrng.rand();

	EXPECT_NEAR(random_number, 0.99718480823026556, 0.0000001);
}

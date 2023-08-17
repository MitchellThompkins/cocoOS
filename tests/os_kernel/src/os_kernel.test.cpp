#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "test_macros.hpp"

#include "CppUTest/TestOutput.h"

#include "platform.h"

#include "os_assert.h"
#include "os_kernel.h"

TEST_GROUP(TestOsKernel)
{
    void setup()
    {
    }

    void teardown()
    {
    }
};

TEST(TestOsKernel, temp)
{
    CHECK_EQUAL(1,1);
}

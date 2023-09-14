#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "test_macros.hpp"

#include "CppUTest/TestOutput.h"

#include "platform.h"

#include "os_defines.h"
#include "os_sem.h"

TEST_GROUP(TestOsSem)
{
    void setup()
    {
    }

    void teardown()
    {
        mock().clear();
    }
};

TEST(TestOsSem, temp)
{
    UT_CATALOG_ID("EVENT-TBD");

    CHECK_TRUE(false);
}

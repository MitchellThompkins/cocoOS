#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "test_macros.hpp"

#include "CppUTest/TestOutput.h"

#include "platform.h"

#include "os_msgqueue.h"

TEST_GROUP(TestOsMsgqueue)
{
    void setup()
    {
    }

    void teardown()
    {
        mock().clear();
    }
};


TEST(TestOsMsgqueue, os_temp)
{
    UT_CATALOG_ID("MSGQUEUE-TBD");

    CHECK_TRUE(true);
}

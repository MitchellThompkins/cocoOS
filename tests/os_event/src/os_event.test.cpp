#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "test_macros.hpp"

#include "CppUTest/TestOutput.h"

#include "platform.h"

#include "os_assert.h"
#include "os_event.h"
#include "os_task.h"

TEST_GROUP(TestOsEvent)
{
    void setup()
    {
    }

    void teardown()
    {
        mock().clear();
    }
};

TEST(TestOsEvent, test0)
{
    CHECK_TRUE(true);
}

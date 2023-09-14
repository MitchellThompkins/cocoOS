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
        os_sem_init();
    }

    void teardown()
    {
        mock().clear();
    }
};

TEST(TestOsSem, os_bin_create)
{
    UT_CATALOG_ID("SEM-1");

    for(int i=0; i<5; i++)
    {
        const auto sem { sem_bin_create(true) };
        CHECK_EQUAL(i, sem);
    }
}

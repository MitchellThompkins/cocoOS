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

        CHECK_TRUE( os_sem_value_get(sem) );
        CHECK_EQUAL( i, sem );
        CHECK_EQUAL( i+1, os_sem_num_sems_get() );
    }

    for(int i=5; i<10; i++)
    {
        const auto sem { sem_bin_create(false) };

        CHECK_FALSE( os_sem_value_get(sem) );
        CHECK_EQUAL( i, sem );
        CHECK_EQUAL( i+1, os_sem_num_sems_get() );
    }
}

TEST(TestOsSem, os_counting_create)
{
    UT_CATALOG_ID("SEM-TBD");

    for(int i=0; i<10; i++)
    {
        const auto sem { sem_counting_create(i+1, i+1) };

        CHECK_EQUAL( i+1, os_sem_value_get(sem) );
        CHECK_EQUAL( i, sem );
        CHECK_EQUAL( i+1, os_sem_num_sems_get() );
    }
}

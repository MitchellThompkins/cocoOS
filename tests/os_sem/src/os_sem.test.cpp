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

    CHECK_EQUAL( 10, os_sem_num_sems_get() );
    os_sem_init();
    CHECK_EQUAL( 0, os_sem_num_sems_get() );
}


TEST(TestOsSem, os_counting_create)
{
    UT_CATALOG_ID("SEM-2");

    for(int i=0; i<10; i++)
    {
        const auto sem { sem_counting_create(i+1, i+1) };

        CHECK_EQUAL( i+1, os_sem_value_get(sem) );
        CHECK_EQUAL( i, sem );
        CHECK_EQUAL( i+1, os_sem_num_sems_get() );
    }

    CHECK_EQUAL( 10, os_sem_num_sems_get() );
    os_sem_init();
    CHECK_EQUAL( 0, os_sem_num_sems_get() );
}


TEST(TestOsSem, os_sem_larger_than_zero)
{
    UT_CATALOG_ID("SEM-5");

    const auto sem_id_0 { sem_counting_create(3, 3) };
    CHECK_TRUE( os_sem_larger_than_zero(sem_id_0) );

    const auto sem_id_1 { sem_counting_create(1, 0) };
    CHECK_FALSE( os_sem_larger_than_zero(sem_id_1) );

    const auto sem_id_2 { sem_bin_create(false) };
    CHECK_FALSE( os_sem_larger_than_zero(sem_id_2) );

    const auto sem_id_3 { sem_bin_create(true) };
    CHECK_TRUE( os_sem_larger_than_zero(sem_id_3) );
}


TEST(TestOsSem, os_sem_increment_decrement)
{
    UT_CATALOG_ID("SEM-3");
    UT_CATALOG_ID("SEM-4");
    UT_CATALOG_ID("SEM-6");

    static constexpr uint8_t max_sem_val {4};

    const auto sem_id { sem_counting_create(max_sem_val, 0) };
    CHECK_EQUAL( 0, os_sem_value_get(sem_id) );

    // Shoudn't be able to count below 0
    os_sem_decrement( sem_id );
    CHECK_EQUAL( 0, os_sem_value_get(sem_id) );

    for(int i=0; i<max_sem_val; i++)
    {
        os_sem_increment( sem_id );
        CHECK_EQUAL( i+1, os_sem_value_get(sem_id) );
    }

    os_sem_increment( sem_id );
    CHECK_EQUAL( max_sem_val, os_sem_value_get(sem_id) );

    for(int i=0; i<max_sem_val; i++)
    {
        os_sem_decrement( sem_id );
        CHECK_EQUAL( max_sem_val-i-1, os_sem_value_get(sem_id) );
    }

    // Still shoudn't be able to count below 0
    os_sem_decrement( sem_id );
    CHECK_EQUAL( 0, os_sem_value_get(sem_id) );
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "test_macros.hpp"

#include "CppUTest/TestOutput.h"

#include "platform.h"

#include "os_assert.h"
#include "os_kernel.h"
#include "os_task.h"

static uint16_t running_count[3] {0};

static void dummy_task0(void)
{
    task_open();

    for(;;)
    {
        running_count[0]++;
        task_wait(10);
    }

    task_close();
}

static void dummy_task1(void)
{
    task_open();

    for(;;)
    {
        running_count[1]++;
        task_wait(20);
    }

    task_close();
}

static void dummy_task2(void)
{
    task_open();

    for(;;)
    {
        running_count[2]++;
        task_wait(30);
    }

    task_close();
}

TEST_GROUP(TestOsKernel)
{
    void setup()
    {
        for(int i{0}; i<sizeof(running_count)/sizeof(running_count[0]); i++)
        {
            running_count[i]=0;
        }

        os_task_init();
    }

    void teardown()
    {
        mock().clear();
    }
};

TEST(TestOsKernel, verify_schedule)
{
    mock().expectOneCall("os_init");
    os_init();

    const auto id0 {task_create( dummy_task0, NULL, 3, NULL, 0, 0 )};
    const auto id1 {task_create( dummy_task1, NULL, 2, NULL, 0, 0 )};
    const auto id2 {task_create( dummy_task2, NULL, 1, NULL, 0, 0 )};

    for(int i{0}; i<10; i++)
    {
        os_tick();
        unit_test_os_schedule();
    }
    CHECK_EQUAL(1 ,running_count[0]);
    CHECK_EQUAL(0 ,running_count[1]);
    CHECK_EQUAL(0 ,running_count[2]);

    for(int i{0}; i<20; i++)
    {
        os_tick();
        unit_test_os_schedule();
    }
    CHECK_EQUAL(1 ,running_count[0]);
    CHECK_EQUAL(1 ,running_count[1]);
    CHECK_EQUAL(0 ,running_count[2]);

    for(int i{0}; i<30; i++)
    {
        os_tick();
        unit_test_os_schedule();
    }
    CHECK_EQUAL(1 ,running_count[0]);
    CHECK_EQUAL(1 ,running_count[1]);
    CHECK_EQUAL(1 ,running_count[2]);

    for(int i{0}; i<80; i++)
    {
        os_tick();
        unit_test_os_schedule();
    }
    CHECK_EQUAL(2 ,running_count[0]);
    CHECK_EQUAL(2 ,running_count[1]);
    CHECK_EQUAL(2 ,running_count[2]);

    for(int i{0}; i<30; i++)
    {
        os_tick();
        unit_test_os_schedule();
    }
    CHECK_EQUAL(3 ,running_count[0]);
    CHECK_EQUAL(3 ,running_count[1]);
    CHECK_EQUAL(2 ,running_count[2]);

    for(int i{0}; i<30; i++)
    {
        os_tick();
        unit_test_os_schedule();
    }
    CHECK_EQUAL(3 ,running_count[0]);
    CHECK_EQUAL(3 ,running_count[1]);
    CHECK_EQUAL(3 ,running_count[2]);
}

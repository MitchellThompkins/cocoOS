#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "test_macros.hpp"

#include "CppUTest/TestOutput.h"

#include "platform.h"

#include "os_assert.h"
#include "os_kernel.h"
#include "os_task.h"

static uint16_t running_count[4] {0};

static void step_os(const uint16_t steps)
{
    for(int i{0}; i<steps; i++)
    {
        os_tick();
        unit_test_os_schedule();
    }
}

static void dummy_task0(void)
{
    task_open();

    for(;;)
    {
        running_count[0]++;
        task_wait(5);
        running_count[0]++;
    }

    task_close();
}

static void dummy_task1(void)
{
    task_open();

    for(;;)
    {
        running_count[1]++;
        task_wait(10);
        running_count[1]++;
    }

    task_close();
}

static void dummy_task2(void)
{
    task_open();

    for(;;)
    {
        running_count[2]++;
        task_wait(20);
        running_count[2]++;
    }

    task_close();
}

static void dummy_task3(void)
{
    task_open();

    running_count[3]++;
    task_wait(10);
    running_count[3]++;

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

TEST(TestOsKernel, simple_verify_schedule)
{
    UT_CATALOG_ID("KERNEL-1");
    UT_CATALOG_ID("KERNEL-2");

    mock().expectOneCall("os_sem_init");
    mock().expectOneCall("os_event_init");
    mock().expectOneCall("os_msgQ_init");
    mock().expectOneCall("os_task_init");
    os_init();

    const auto id0 {task_create( dummy_task0, NULL, 3, NULL, 0, 0 )};
    const auto id1 {task_create( dummy_task1, NULL, 2, NULL, 0, 0 )};
    const auto id2 {task_create( dummy_task2, NULL, 1, NULL, 0, 0 )};

    step_os(3);
    CHECK_EQUAL(1, running_count[0]);
    CHECK_EQUAL(1, running_count[1]);
    CHECK_EQUAL(1, running_count[2]);

    step_os(5);
    CHECK_EQUAL(3, running_count[0]);
    CHECK_EQUAL(1, running_count[1]);
    CHECK_EQUAL(1, running_count[2]);

    step_os(5);
    CHECK_EQUAL(5, running_count[0]);
    CHECK_EQUAL(3, running_count[1]);
    CHECK_EQUAL(1, running_count[2]);

    step_os(10);
    CHECK_EQUAL(9, running_count[0]);
    CHECK_EQUAL(5, running_count[1]);
    CHECK_EQUAL(3, running_count[2]);
}

TEST(TestOsKernel, verify_single_task_execution)
{
    UT_CATALOG_ID("KERNEL-2");

    mock().expectOneCall("os_sem_init");
    mock().expectOneCall("os_event_init");
    mock().expectOneCall("os_msgQ_init");
    mock().expectOneCall("os_task_init");
    os_init();

    const auto id {task_create( dummy_task3, NULL, 1, NULL, 0, 0 )};

    step_os(5);
    CHECK_EQUAL(1, running_count[3]);

    step_os(5);
    CHECK_EQUAL(1, running_count[3]);

    step_os(1);
    CHECK_EQUAL(2, running_count[3]);

    step_os(100);
    CHECK_EQUAL(2, running_count[3]);
}

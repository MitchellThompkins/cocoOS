#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "test_macros.hpp"

#include "CppUTest/TestOutput.h"

#include "platform.h"

#include "os_assert.h"
#include "os_kernel.h"
#include "os_task.h"

static uint16_t running_count[4] {0};
static bool task_ran {false};

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

static void dummy_task_check_id(void)
{
    task_open();

    const int* this_id = (int*)task_get_data();
    task_ran = true;
    CHECK_EQUAL(*this_id, os_get_running_tid());

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

        task_ran = false;

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

    const auto id0 {os_task_create( dummy_task0, NULL, 3, NULL, 0, 0 )};
    const auto id1 {os_task_create( dummy_task1, NULL, 2, NULL, 0, 0 )};
    const auto id2 {os_task_create( dummy_task2, NULL, 1, NULL, 0, 0 )};

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

    const auto id {os_task_create( dummy_task3, NULL, 1, NULL, 0, 0 )};

    step_os(5);
    CHECK_EQUAL(1, running_count[3]);

    step_os(5);
    CHECK_EQUAL(1, running_count[3]);

    step_os(1);
    CHECK_EQUAL(2, running_count[3]);

    step_os(100);
    CHECK_EQUAL(2, running_count[3]);
}

TEST(TestOsKernel, test_os_tick)
{
    UT_CATALOG_ID("KERNEL-3");

    mock().expectOneCall("os_sem_init");
    mock().expectOneCall("os_event_init");
    mock().expectOneCall("os_msgQ_init");
    mock().expectOneCall("os_task_init");
    os_init();

    const auto id {os_task_create( dummy_task2, NULL, 1, NULL, 0, 0 )};

    // step os once to get task into wait state
    step_os(1);
    CHECK_EQUAL(20, os_task_timeout_get(id));

    step_os(5);
    CHECK_EQUAL(15, os_task_timeout_get(id));

    step_os(15);
    CHECK_EQUAL(20, os_task_timeout_get(id));

}

TEST(TestOsKernel, test_os_running)
{
    UT_CATALOG_ID("KERNEL-4");

    mock().expectOneCall("os_sem_init");
    mock().expectOneCall("os_event_init");
    mock().expectOneCall("os_msgQ_init");
    mock().expectOneCall("os_task_init");
    os_init();

    CHECK_EQUAL(0, os_running());

    static constexpr uint16_t interval_ms {100};
    set_tick_limit_before_exit(100);
    platform_setup_timer(interval_ms);
    platform_enable_timer();

    os_start(10);

    CHECK_EQUAL(1, os_running());
}

TEST(TestOsKernel, test_os_running_id)
{
    UT_CATALOG_ID("KERNEL-5");

    mock().expectOneCall("os_sem_init");
    mock().expectOneCall("os_event_init");
    mock().expectOneCall("os_msgQ_init");
    mock().expectOneCall("os_task_init");
    os_init();

    CHECK_EQUAL(NO_TID, os_get_running_tid());

    uint16_t expectedId;
    const auto id {os_task_create( dummy_task_check_id, &expectedId, 1, NULL, 0, 0 )};
    expectedId = id;

    step_os(1);
    CHECK_TRUE(task_ran);
}

TEST(TestOsKernel, test_os_sub_tick)
{
    UT_CATALOG_ID("KERNEL-6");

    mock().expectOneCall("os_sem_init");
    mock().expectOneCall("os_event_init");
    mock().expectOneCall("os_msgQ_init");
    mock().expectOneCall("os_task_init");
    os_init();

    const auto id0 {os_task_create( dummy_task0, NULL, 3, NULL, 0, 0 )};
    const auto id1 {os_task_create( dummy_task1, NULL, 2, NULL, 0, 0 )};
    const auto id2 {os_task_create( dummy_task2, NULL, 1, NULL, 0, 0 )};

    // Have 2 tasks share a clock id
    const uint8_t clock_id0 {3};
    const uint8_t clock_id1 {5};

    os_task_wait_time_set( id0, clock_id0, 20 );
    os_task_wait_time_set( id1, clock_id0, 30 );
    os_task_wait_time_set( id2, clock_id1, 40 );

    CHECK_EQUAL( 20, os_task_timeout_get(id0) );
    CHECK_EQUAL( 30, os_task_timeout_get(id1) );
    CHECK_EQUAL( 40, os_task_timeout_get(id2) );

    const uint8_t clock_step_first {3};
    os_sub_nTick(clock_id0, clock_step_first);

    CHECK_EQUAL( 20-clock_step_first, os_task_timeout_get(id0) );
    CHECK_EQUAL( 30-clock_step_first, os_task_timeout_get(id1) );
    CHECK_EQUAL( 40, os_task_timeout_get(id2) );

    uint8_t clock_step_second = 7;
    os_sub_nTick(clock_id1, clock_step_second);

    CHECK_EQUAL( 20-clock_step_first,  os_task_timeout_get(id0) );
    CHECK_EQUAL( 30-clock_step_first,  os_task_timeout_get(id1) );
    CHECK_EQUAL( 40-clock_step_second, os_task_timeout_get(id2) );

    // Make sure clocks didn't step when master clock ticks
    os_tick();
    CHECK_EQUAL( 20-clock_step_first,  os_task_timeout_get(id0) );
    CHECK_EQUAL( 30-clock_step_first,  os_task_timeout_get(id1) );
    CHECK_EQUAL( 40-clock_step_second, os_task_timeout_get(id2) );

    // Make sure only specified clock decrements by 1
    os_sub_tick(clock_id0);
    CHECK_EQUAL( 20-clock_step_first-1,  os_task_timeout_get(id0) );
    CHECK_EQUAL( 30-clock_step_first-1,  os_task_timeout_get(id1) );
    CHECK_EQUAL( 40-clock_step_second,   os_task_timeout_get(id2) );
}

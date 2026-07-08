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

    const uint8_t* this_id = (uint8_t*)task_get_data();
    task_ran = true;
    CHECK_EQUAL(*this_id, os_get_running_tid());

    task_close();
}

TEST_GROUP(TestOsKernel)
{
    void setup()
    {
        for(size_t i{0}; i<sizeof(running_count)/sizeof(running_count[0]); i++)
        {
            running_count[i]=0;
        }

        task_ran = false;

        os_task_init();
    }

    void teardown()
    {
        mock().checkExpectations();
        mock().clear();
    }
};

TEST(TestOsKernel, simple_verify_schedule)
{
    UT_CATALOG_ID("KERNEL-1");
    UT_CATALOG_ID("KERNEL-2");

    mock().expectOneCall("os_sem_init");
    mock().expectOneCall("os_event_init");
    mock().ignoreOtherCalls();
    os_init();

    (void)os_task_create( dummy_task0, NULL, 3, NULL, 0, 0 );
    (void)os_task_create( dummy_task1, NULL, 2, NULL, 0, 0 );
    (void)os_task_create( dummy_task2, NULL, 1, NULL, 0, 0 );

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
    mock().ignoreOtherCalls();
    os_init();

    (void)os_task_create( dummy_task3, NULL, 1, NULL, 0, 0 );

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
    mock().ignoreOtherCalls();
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
    mock().ignoreOtherCalls();
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
    mock().ignoreOtherCalls();
    os_init();

    CHECK_EQUAL(NO_TID, os_get_running_tid());

    uint8_t expectedId;
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
    mock().ignoreOtherCalls();
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

    CHECK_EQUAL( (uint32_t)(20-clock_step_first), os_task_timeout_get(id0) );
    CHECK_EQUAL( (uint32_t)(30-clock_step_first), os_task_timeout_get(id1) );
    CHECK_EQUAL( (uint32_t)40,                    os_task_timeout_get(id2) );

    uint8_t clock_step_second = 7;
    os_sub_nTick(clock_id1, clock_step_second);

    CHECK_EQUAL( (uint32_t)(20-clock_step_first), os_task_timeout_get(id0) );
    CHECK_EQUAL( (uint32_t)(30-clock_step_first), os_task_timeout_get(id1) );
    CHECK_EQUAL( (uint32_t)(40-clock_step_second), os_task_timeout_get(id2) );

    // Make sure clocks didn't step when master clock ticks
    os_tick();
    CHECK_EQUAL( (uint32_t)(20-clock_step_first), os_task_timeout_get(id0) );
    CHECK_EQUAL( (uint32_t)(30-clock_step_first), os_task_timeout_get(id1) );
    CHECK_EQUAL( (uint32_t)(40-clock_step_second), os_task_timeout_get(id2) );

    // Make sure only specified clock decrements by 1
    os_sub_tick(clock_id0);
    CHECK_EQUAL( (uint32_t)(20-clock_step_first-1), os_task_timeout_get(id0) );
    CHECK_EQUAL( (uint32_t)(30-clock_step_first-1), os_task_timeout_get(id1) );
    CHECK_EQUAL( (uint32_t)(40-clock_step_second),  os_task_timeout_get(id2) );
}

// Strong override of the weak os_cbkSleep, counts calls for KERNEL-14
static uint32_t sleep_cb_count {0};
extern "C" void os_cbkSleep( void )
{
    sleep_cb_count++;
}

TEST(TestOsKernel, os_init_initializes_subsystems)
{
    UT_CATALOG_ID("KERNEL-7");

    // os_task_init and os_msgQ_init are real (resolve from real impls due to
    // transitive link via os_task_impl), so we cannot expect them as mock calls.
    mock().expectOneCall("os_sem_init");
    mock().expectOneCall("os_event_init");
    mock().ignoreOtherCalls();
    os_init();

    // os_running and os_get_running_tid reset to initial values
    CHECK_EQUAL( 0, os_running() );
    CHECK_EQUAL( NO_TID, os_get_running_tid() );
}

TEST(TestOsKernel, os_start_runs_for_tick_limit)
{
    UT_CATALOG_ID("KERNEL-8");

    mock().expectOneCall("os_sem_init");
    mock().expectOneCall("os_event_init");
    mock().ignoreOtherCalls();
    os_init();

    (void)os_task_create( dummy_task0, NULL, 1, NULL, 0, 0 );

    static constexpr uint16_t interval_ms {1};
    set_tick_limit_before_exit(500);
    platform_setup_timer(interval_ms);
    platform_enable_timer();

    const uint32_t limit {10};
    os_start( limit );

    CHECK_EQUAL( 1, os_running() );
    CHECK_TRUE( running_count[0] > 0 );
}

static uint32_t lock_count   {0};
static uint32_t unlock_count {0};
static void test_lock(void)   { lock_count++;   }
static void test_unlock(void) { unlock_count++; }

TEST(TestOsKernel, os_start_locking_calls_lock_unlock)
{
    UT_CATALOG_ID("KERNEL-9");

    mock().expectOneCall("os_sem_init");
    mock().expectOneCall("os_event_init");
    mock().ignoreOtherCalls();
    os_init();

    os_task_create( dummy_task0, NULL, 1, NULL, 0, 0 );

    lock_count   = 0;
    unlock_count = 0;

    set_tick_limit_before_exit(500);
    platform_setup_timer(1);
    platform_enable_timer();

    const uint32_t limit {5};
    os_start_locking( limit, test_lock, test_unlock );

    CHECK_EQUAL( lock_count, unlock_count );
    CHECK_TRUE( lock_count > 0 );
}

TEST(TestOsKernel, scheduler_runs_highest_prio_task)
{
    UT_CATALOG_ID("KERNEL-10");

    mock().expectOneCall("os_sem_init");
    mock().expectOneCall("os_event_init");
    mock().ignoreOtherCalls();
    os_init();

    // id0 has lower priority (higher number), id1 has higher priority (lower number)
    os_task_create( dummy_task0, NULL, 2, NULL, 0, 0 );  // lower prio
    os_task_create( dummy_task1, NULL, 1, NULL, 0, 0 );  // higher prio

    step_os(1);

    // The higher-prio task (id1 / dummy_task1) ran first
    CHECK_EQUAL( 0, running_count[0] );
    CHECK_EQUAL( 1, running_count[1] );
}

TEST(TestOsKernel, running_tid_set_during_task_execution)
{
    UT_CATALOG_ID("KERNEL-12");

    mock().expectOneCall("os_sem_init");
    mock().expectOneCall("os_event_init");
    mock().ignoreOtherCalls();
    os_init();

    uint8_t expectedId;
    const auto id = os_task_create( dummy_task_check_id, &expectedId, 1, NULL, 0, 0 );
    expectedId = id;

    step_os(1);
    CHECK_TRUE( task_ran );
    // Idle: running tid must be NO_TID
    CHECK_EQUAL( NO_TID, os_get_running_tid() );
}

TEST(TestOsKernel, os_free_tid_clears_running_tid)
{
    UT_CATALOG_ID("KERNEL-13");

    mock().expectOneCall("os_sem_init");
    mock().expectOneCall("os_event_init");
    mock().ignoreOtherCalls();
    os_init();

    os_free_tid();
    CHECK_EQUAL( NO_TID, os_get_running_tid() );
}

TEST(TestOsKernel, sleep_callback_invoked_when_no_task_ready)
{
    UT_CATALOG_ID("KERNEL-14");

    mock().expectOneCall("os_sem_init");
    mock().expectOneCall("os_event_init");
    mock().ignoreOtherCalls();
    os_init();

    // A task waiting for time leaves no READY tasks between ticks
    (void)os_task_create( dummy_task2, NULL, 1, NULL, 0, 0 );

    sleep_cb_count = 0;

    // Single step, task runs, then waits 20 ticks
    step_os(1);  // task runs, enters wait
    step_os(1);  // no task ready, os_cbkSleep called

    CHECK_TRUE( sleep_cb_count > 0 );
}

TEST(TestOsKernel, os_sub_tick_increments_sub_clock_tasks)
{
    UT_CATALOG_ID("KERNEL-15");

    mock().expectOneCall("os_sem_init");
    mock().expectOneCall("os_event_init");
    mock().ignoreOtherCalls();
    os_init();

    os_task_create( dummy_task0, NULL, 3, NULL, 0, 0 );
    os_task_create( dummy_task1, NULL, 2, NULL, 0, 0 );
    os_task_create( dummy_task2, NULL, 1, NULL, 0, 0 );

    const uint8_t sub_id {7};
    os_task_wait_time_set( 0, sub_id, 2 );  // task 0: sub-clock 7, time=2

    // Sub-clock tick 1, task 0 time from 2 to 1, not ready yet
    os_sub_tick( sub_id );
    CHECK_EQUAL( 1, os_task_timeout_get(0) );
    CHECK_EQUAL( WAITING_TIME, task_state_get(0) );

    // Sub-clock tick 2, task 0 time from 1 to 0, becomes READY
    os_sub_tick( sub_id );
    CHECK_EQUAL( READY, task_state_get(0) );

    // os_sub_tick(0) (master clock id) does NOT affect tasks on sub-clocks
    os_task_wait_time_set( 1, sub_id, 2 );
    CHECK_EQUAL( 2, os_task_timeout_get(1) );
    os_sub_tick( 0 );  // master clock: should be ignored
    CHECK_EQUAL( 2, os_task_timeout_get(1) );
}

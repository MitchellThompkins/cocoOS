#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include "platform.h"

#include "os_assert.h"
#include "os_kernel.h"
#include "os_task.h"

static void dummy_task(void)
{
    task_open();

    for(;;)
    {
        task_wait( 20 );
    }

    task_close();
}

TEST_GROUP(TestOsTask)
{
    static constexpr uint16_t interval_ms {250};

    void setup()
    {
        os_task_init();

        set_tick_limit_before_exit(10);

        platform_setup_timer(interval_ms);
        platform_enable_timer();
    }

    void teardown()
    {
        mock().clear();
    }
};

// TASK-1
TEST(TestOsTask, fail_create_task_null_taskproc)
{
    mock().expectOneCall("os_init");
    os_init();

    // Verify that calling task with the same prio level invokes assert
    mock().expectOneCall("os_on_assert");
    task_create( NULL, NULL, 1, NULL, 0, 0 );
    mock().checkExpectations();
    mock().clear();
}

// TASK-1
TEST(TestOsTask, fail_create_task_with_same_prio)
{
    mock().expectOneCall("os_init");
    os_init();

    // Verify that calling task with the same prio level invokes assert
    mock().expectOneCall("os_on_assert");
    task_create( dummy_task, NULL, 1, NULL, 0, 0 );
    task_create( dummy_task, NULL, 1, NULL, 0, 0 );

    mock().checkExpectations();
    mock().clear();
}

// TASK-1
TEST(TestOsTask, fail_create_task_with_too_many_tasks)
{
    mock().expectOneCall("os_init");
    os_init();

    // Verify that calling task with the same prio level invokes assert
    mock().expectOneCall("os_on_assert");
    for(int i{0}; i<=N_TASKS; i++)
    {
        task_create( dummy_task, NULL, i, NULL, 0, 0 );
    }

    mock().checkExpectations();
    mock().clear();
}

// TASK-2
TEST(TestOsTask, fail_create_task_when_os_is_running)
{
    mock().expectOneCall("os_init");
    os_init();

    mock().expectOneCall("os_start");
    os_start();

    // Verify that you cannot create a task while os is running
    mock().expectOneCall("os_on_assert").andReturnValue(false);
    task_create( dummy_task, NULL, 1, NULL, 0, 0 );

    mock().checkExpectations();
    mock().clear();

}

// TASK-3
TEST(TestOsTask, successful_task_initialization)
{
    mock().expectOneCall("os_init");
    os_init();

    // Verify that you cannot create a task while os is running
    const auto id1 {task_create( dummy_task, NULL, 1, NULL, 0, 0 )};
    CHECK_EQUAL(0, id1);

    const auto id2 {task_create( dummy_task, NULL, 2, NULL, 0, 0 )};
    CHECK_EQUAL(1, id2);

    const auto id3 {task_create( dummy_task, NULL, 3, NULL, 0, 0 )};
    CHECK_EQUAL(2, id3);
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include "platform.h"

#define N_TASKS  1
#define N_QUEUES 0
#define N_SEMAPHORES 0
#define N_EVENTS 0
#define ROUND_ROBIN 0
#define Mem_t uint32_t

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
        set_tick_limit_before_exit(10);

        platform_setup_timer(interval_ms);
        platform_enable_timer();
    }

    void teardown()
    {
        mock().clear();
    }
};

TEST(TestOsTask, fail_create_task_with_same_prio)
{
    // Verify that calling task with the same prio level invokes assert
    mock().expectOneCall("os_on_assert").andReturnValue(false);
    task_create( dummy_task, NULL, 1, NULL, 0, 0 );
    task_create( dummy_task, NULL, 1, NULL, 0, 0 );
    mock().checkExpectations();
    mock().clear();
}

TEST(TestOsTask, fail_create_task_when_os_is_running)
{
    os_init();
    os_start();


    // Verify that you cannot create a task while os is running
    mock().expectOneCall("os_on_assert").andReturnValue(false);
    task_create( dummy_task, NULL, 1, NULL, 0, 0 );
    mock().checkExpectations();
    mock().clear();

}

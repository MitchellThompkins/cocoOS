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
{}

TEST_GROUP(TestOsTask)
{
    static constexpr uint16_t interval_ms {250};

    void setup()
    {
        platform_setup_timer(interval_ms);
        platform_enable_timer();
    }

    void teardown()
    {
        mock().clear();
    }
};

TEST(TestOsTask, task_create_fail)
{
    set_tick_limit_before_exit(10);

    mock().expectOneCall("os_on_assert").andReturnValue(false);

    task_create( dummy_task, NULL, 1, NULL, 0, 0 );
    task_create( dummy_task, NULL, 1, NULL, 0, 0 );

    mock().checkExpectations();
    mock().clear();

}

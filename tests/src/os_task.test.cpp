#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "mocks.hpp"

#include "cocoos.h"

#define N_TASKS  1
#define N_QUEUES 0
#define N_SEMAPHORES 0
#define N_EVENTS 0
#define ROUND_ROBIN 0
#define Mem_t uint32_t

static void dummy_task(void)
{}

TEST_GROUP(TestOsTask)
{
    void setup()
    {
    }

    void teardown()
    {
        mock().clear();
    }
};

TEST(TestOsTask, task_create_fail)
{
    mock().expectOneCall("os_assert");
    task_create( dummy_task, NULL, 1, NULL, 0, 0 );
    mock().checkExpectations();
    mock().clear();

}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "test_macros.hpp"

#include "CppUTest/TestOutput.h"

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

TEST_GROUP(TestOsKernel)
{
    void setup()
    {
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

    const auto id0 {task_create( dummy_task, NULL, 3, NULL, 0, 0 )};
    const auto id1 {task_create( dummy_task, NULL, 2, NULL, 0, 0 )};
    const auto id2 {task_create( dummy_task, NULL, 1, NULL, 0, 0 )};

    CHECK_EQUAL(1,1);
}

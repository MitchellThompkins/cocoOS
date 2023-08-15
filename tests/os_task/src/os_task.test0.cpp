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

TEST(TestOsTask, fail_create_task_null_taskproc)
{
    UT_CATALOG_ID("TASK-1");
    mock().expectOneCall("os_init");
    os_init();

    // Verify that calling task with the same prio level invokes assert
    mock().expectOneCall("os_on_assert");
    task_create( NULL, NULL, 1, NULL, 0, 0 );
    mock().checkExpectations();
    mock().clear();
}

TEST(TestOsTask, fail_create_task_with_same_prio)
{
    UT_CATALOG_ID("TASK-1");

    mock().expectOneCall("os_init");
    os_init();

    // Verify that calling task with the same prio level invokes assert
    mock().expectOneCall("os_on_assert");
    task_create( dummy_task, NULL, 1, NULL, 0, 0 );
    task_create( dummy_task, NULL, 1, NULL, 0, 0 );

    mock().checkExpectations();
    mock().clear();
}

TEST(TestOsTask, fail_create_task_with_too_many_tasks)
{
    UT_CATALOG_ID("TASK-1");

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

TEST(TestOsTask, fail_create_task_when_os_is_running)
{
    UT_CATALOG_ID("TASK-2");

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

TEST(TestOsTask, successful_task_initialization)
{
    UT_CATALOG_ID("TASK-3");
    UT_CATALOG_ID("TASK-18");
    mock().expectOneCall("os_init");
    os_init();

    // Verify that you cannot create a task while os is running
    const auto id1 {task_create( dummy_task, NULL, 1, NULL, 0, 0 )};
    CHECK_EQUAL(0, id1);
    CHECK_EQUAL(1, os_task_prio_get(id1) );

    const auto id2 {task_create( dummy_task, NULL, 2, NULL, 0, 0 )};
    CHECK_EQUAL(1, id2);
    CHECK_EQUAL(2, os_task_prio_get(id2) );

    const auto id3 {task_create( dummy_task, NULL, 3, NULL, 0, 0 )};
    CHECK_EQUAL(2, id3);
    CHECK_EQUAL(3, os_task_prio_get(id3) );
}

TEST(TestOsTask, next_highest_prio_task)
{
    UT_CATALOG_ID("TASK-9");

    mock().expectOneCall("os_init");
    os_init();

    const auto id1 {task_create( dummy_task, NULL, 3, NULL, 0, 0 )};
    const auto id2 {task_create( dummy_task, NULL, 2, NULL, 0, 0 )};
    const auto id3 {task_create( dummy_task, NULL, 1, NULL, 0, 0 )};

    auto task_to_run {os_task_highest_prio_ready_task()};

    CHECK_EQUAL(id3, task_to_run);

    os_task_suspend(id3);
    task_to_run = os_task_highest_prio_ready_task();
    CHECK_EQUAL(id2, task_to_run);

}

TEST(TestOsTask, release_task_prio_waiting_on_semaphore)
{
    UT_CATALOG_ID("TASK-11");
    UT_CATALOG_ID("TASK-14");

    mock().expectOneCall("os_init");
    os_init();

    const auto id0 {task_create( dummy_task, NULL, 3, NULL, 0, 0 )};
    const auto id1 {task_create( dummy_task, NULL, 2, NULL, 0, 0 )};
    const auto id2 {task_create( dummy_task, NULL, 1, NULL, 0, 0 )};

    mock().setData("sem_return_value", 0);
    mock().expectOneCall("sem_counting_create").andReturnValue(0);
    Sem_t sem0 {sem_counting_create(3, 0)};

    mock().setData("sem_return_value", 1);
    mock().expectOneCall("sem_counting_create").andReturnValue(1);
    Sem_t sem1 {sem_counting_create(3, 0)};

    task_wait_sem_set(id0, sem0);
    task_wait_sem_set(id1, sem1);

    os_task_release_waiting_task(sem0);

    const auto id0_state { task_state_get(id0) };
    const auto id1_state { task_state_get(id1) };
    const auto id2_state { task_state_get(id2) };

    CHECK_EQUAL(READY, id0_state);
    CHECK_EQUAL(WAITING_SEM, id1_state);
    CHECK_EQUAL(READY, id2_state);
}

TEST(TestOsTask, task_waiting_semaphore)
{
    UT_CATALOG_ID("TASK-12");
    UT_CATALOG_ID("TASK-13");
    UT_CATALOG_ID("TASK-14");

    mock().expectOneCall("os_init");
    os_init();

    const auto id0 {task_create( dummy_task, NULL, 3, NULL, 0, 0 )};
    const auto id1 {task_create( dummy_task, NULL, 2, NULL, 0, 0 )};
    const auto id2 {task_create( dummy_task, NULL, 1, NULL, 0, 0 )};

    mock().setData("sem_return_value", 0);
    mock().expectOneCall("sem_counting_create");
    Sem_t sem0 { sem_counting_create(3, 0) };

    mock().setData("sem_return_value", 1);
    mock().expectOneCall("sem_counting_create");
    Sem_t sem1 { sem_counting_create(3, 0) };

    mock().setData("sem_return_value", 2);
    mock().expectOneCall("sem_counting_create");
    Sem_t sem2 { sem_counting_create(3, 0) };

    task_wait_sem_set(id1, sem0);
    task_wait_sem_set(id0, sem1);

    const auto t0 = os_task_waiting_this_semaphore(sem0);
    const auto t1 = os_task_waiting_this_semaphore(sem1);
    const auto t2 = os_task_waiting_this_semaphore(sem2);

    CHECK_TRUE(t0);
    CHECK_TRUE(t1);
    CHECK_FALSE(t2);

    //TODO(@mthompkins): Consider returning from
    //os_task_waiting_this_semaphore _which_ task is waiting on this
    //semaphore, make the return type signed and return -1 for none
    //CHECK_EQUAL(id1, t0);
    //CHECK_EQUAL(id0, t1);
    //CHECK_EQUAL(-1, t2);

    const auto waiting_state_id0 { task_state_get(id0) };
    const auto waiting_state_id1 { task_state_get(id1) };
    const auto waiting_state_id2 { task_state_get(id2) };

    CHECK_EQUAL(WAITING_SEM, waiting_state_id0);
    CHECK_EQUAL(WAITING_SEM, waiting_state_id1);
    CHECK_EQUAL(READY, waiting_state_id2);
}

TEST(TestOsTask, tick_time_for_tasks)
{
    UT_CATALOG_ID("TASK-22");

    mock().expectOneCall("os_init");
    os_init();

    const auto id0 {task_create( dummy_task, NULL, 3, NULL, 0, 0 )};
    const auto id1 {task_create( dummy_task, NULL, 2, NULL, 0, 0 )};
    const auto id2 {task_create( dummy_task, NULL, 1, NULL, 0, 0 )};

    mock().setData("sem_return_value", 0);
    mock().expectOneCall("sem_counting_create");
    Sem_t sem0 { sem_counting_create(3, 0) };

    mock().setData("sem_return_value", 1);
    mock().expectOneCall("sem_counting_create");
    Sem_t sem1 { sem_counting_create(3, 0) };

    task_wait_sem_set(id1, sem0);
    task_wait_sem_set(id0, sem1);

    const auto t0 = os_task_waiting_this_semaphore(sem0);
    const auto t1 = os_task_waiting_this_semaphore(sem1);

    const auto waiting_state_id0 { task_state_get(id0) };
    const auto waiting_state_id1 { task_state_get(id1) };
    const auto waiting_state_id2 { task_state_get(id2) };

    auto timeout0 { os_task_timeout_get(id0) };
    auto timeout1 { os_task_timeout_get(id1) };
    auto timeout2 { os_task_timeout_get(id2) };
    CHECK_EQUAL(0, timeout0);
    CHECK_EQUAL(0, timeout1);
    CHECK_EQUAL(0, timeout2);

    os_task_tick(0, 0);
    timeout0 = os_task_timeout_get(id0);
    timeout1 = os_task_timeout_get(id1);
    timeout2 = os_task_timeout_get(id2);
    CHECK_EQUAL(1, timeout0);
    CHECK_EQUAL(1, timeout1);
    CHECK_EQUAL(0, timeout2);

    os_task_tick(0, 0);
    timeout0 = os_task_timeout_get(id0);
    timeout1 = os_task_timeout_get(id1);
    timeout2 = os_task_timeout_get(id2);
    CHECK_EQUAL(2, timeout0);
    CHECK_EQUAL(2, timeout1);
    CHECK_EQUAL(0, timeout2);
}

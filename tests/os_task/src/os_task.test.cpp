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
    void setup()
    {
        os_task_init();
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
    mock().expectOneCall("os_running");
    os_task_create( NULL, NULL, 1, NULL, 0, 0 );
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
    mock().expectNCalls(2, "os_running");
    os_task_create( dummy_task, NULL, 1, NULL, 0, 0 );
    os_task_create( dummy_task, NULL, 1, NULL, 0, 0 );

    mock().checkExpectations();
    mock().clear();
}

TEST(TestOsTask, fail_create_task_with_too_many_tasks)
{
    UT_CATALOG_ID("TASK-1");

    mock().expectOneCall("os_init");
    os_init();

    // Verify that trying to create too many tasks invokes assert
    mock().expectOneCall("os_on_assert");
    mock().expectNCalls(N_TASKS+1, "os_running");
    for(int i{0}; i<=N_TASKS; i++)
    {
        os_task_create( dummy_task, NULL, i, NULL, 0, 0 );
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
    os_start(0);

    // Verify that you cannot create a task while os is running
    mock().expectOneCall("os_on_assert").andReturnValue(false);
    mock().expectOneCall("os_running");
    os_task_create( dummy_task, NULL, 1, NULL, 0, 0 );

    mock().checkExpectations();
    mock().clear();

}

TEST(TestOsTask, successful_task_initialization)
{
    UT_CATALOG_ID("TASK-3");
    UT_CATALOG_ID("TASK-18");
    mock().expectOneCall("os_init");
    os_init();

    mock().expectNCalls(3, "os_running");
    // Verify that you cannot create a task while os is running
    const auto id1 {os_task_create( dummy_task, NULL, 1, NULL, 0, 0 )};
    CHECK_EQUAL(0, id1);
    CHECK_EQUAL(1, os_task_prio_get(id1) );

    const auto id2 {os_task_create( dummy_task, NULL, 2, NULL, 0, 0 )};
    CHECK_EQUAL(1, id2);
    CHECK_EQUAL(2, os_task_prio_get(id2) );

    const auto id3 {os_task_create( dummy_task, NULL, 3, NULL, 0, 0 )};
    CHECK_EQUAL(2, id3);
    CHECK_EQUAL(3, os_task_prio_get(id3) );
}

TEST(TestOsTask, next_highest_prio_task)
{
    UT_CATALOG_ID("TASK-9");

    mock().expectOneCall("os_init");
    os_init();

    mock().expectNCalls(3, "os_running");
    const auto id1 {os_task_create( dummy_task, NULL, 3, NULL, 0, 0 )};
    const auto id2 {os_task_create( dummy_task, NULL, 2, NULL, 0, 0 )};
    const auto id3 {os_task_create( dummy_task, NULL, 1, NULL, 0, 0 )};

    auto task_to_run {highest_prio_ready_task()};

    CHECK_EQUAL(id3, task_to_run);

    os_task_suspend(id3);
    task_to_run = highest_prio_ready_task();
    CHECK_EQUAL(id2, task_to_run);

}

TEST(TestOsTask, release_task_prio_waiting_on_semaphore)
{
    UT_CATALOG_ID("TASK-11");
    UT_CATALOG_ID("TASK-14");

    mock().expectOneCall("os_init");
    os_init();

    mock().expectNCalls(3, "os_running");
    const auto id0 {os_task_create( dummy_task, NULL, 3, NULL, 0, 0 )};
    const auto id1 {os_task_create( dummy_task, NULL, 2, NULL, 0, 0 )};
    const auto id2 {os_task_create( dummy_task, NULL, 1, NULL, 0, 0 )};

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

    mock().expectNCalls(3, "os_running");
    const auto id0 {os_task_create( dummy_task, NULL, 3, NULL, 0, 0 )};
    const auto id1 {os_task_create( dummy_task, NULL, 2, NULL, 0, 0 )};
    const auto id2 {os_task_create( dummy_task, NULL, 1, NULL, 0, 0 )};

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

    mock().expectNCalls(3, "os_running");
    const auto id0 {os_task_create( dummy_task, NULL, 3, NULL, 0, 0 )};
    const auto id1 {os_task_create( dummy_task, NULL, 2, NULL, 0, 0 )};
    const auto id2 {os_task_create( dummy_task, NULL, 1, NULL, 0, 0 )};

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

    task_tick(0, 0);
    timeout0 = os_task_timeout_get(id0);
    timeout1 = os_task_timeout_get(id1);
    timeout2 = os_task_timeout_get(id2);
    CHECK_EQUAL(1, timeout0);
    CHECK_EQUAL(1, timeout1);
    CHECK_EQUAL(0, timeout2);

    task_tick(0, 0);
    timeout0 = os_task_timeout_get(id0);
    timeout1 = os_task_timeout_get(id1);
    timeout2 = os_task_timeout_get(id2);
    CHECK_EQUAL(2, timeout0);
    CHECK_EQUAL(2, timeout1);
    CHECK_EQUAL(0, timeout2);
}

TEST(TestOsTask, test_os_task_wait_event)
{
    UT_CATALOG_ID("TASK-21");

    mock().expectOneCall("os_init");
    os_init();

    mock().expectOneCall("os_running");
    const auto id0 {os_task_create( dummy_task, NULL, 1, NULL, 0, 0 )};

    mock().expectOneCall("event_create");
    mock().setData("event_create_return", 0);
    const auto event_id {event_create()};

    //TODO(@mthompkins): Figure out how to use waitSingleEvent
    const int timeout {5};
    os_task_wait_event(id0, event_id, false, timeout);

    // With zero timeout the task should be WAITING_EVENT_TIMEOUT
    CHECK_EQUAL( WAITING_EVENT_TIMEOUT, task_state_get(id0) );
    CHECK_EQUAL( timeout, os_task_timeout_get(id0) );

    os_task_wait_event(id0, event_id, false, 0);

    // With zero timeout the task should be WAITING_EVENT
    CHECK_EQUAL( WAITING_EVENT, task_state_get(id0) );
}

TEST(TestOsTask, test_os_task_signal_event)
{
    UT_CATALOG_ID("TASK-23");

    mock().expectOneCall("os_init");
    os_init();

    mock().expectOneCall("event_create");
    mock().setData("event_create_return", 0);
    const auto event_id {event_create()};

    // Test that when a task is set with os_task_wait_event with a timeout of
    // 0, it is set to the WAITING_EVENT and when signaled is set to READY
    mock().expectOneCall("os_running");
    const auto id0 {os_task_create( dummy_task, NULL, 1, NULL, 0, 0 )};

    const auto id0_timeout {0};
    os_task_wait_event(id0, event_id, false, id0_timeout);
    CHECK_EQUAL( WAITING_EVENT, task_state_get(id0) );

    os_task_signal_event(event_id);

    CHECK_EQUAL( id0_timeout, os_task_timeout_get(id0) );
    CHECK_EQUAL( READY, task_state_get(id0) );

    // Test that when a task is instructed to waitSingleEvent, that when
    // os_signal event is called it is set to the ready state
    mock().expectOneCall("os_running");
    const auto id1 {os_task_create( dummy_task, NULL, 2, NULL, 0, 0 )};

    const auto id1_timeout {40};
    os_task_wait_event(id1, event_id, true, id1_timeout);
    os_task_signal_event(event_id);

    CHECK_EQUAL( READY, task_state_get(id1) );
    CHECK_EQUAL( id1_timeout, os_task_timeout_get(id1) );


    // Test that when a task times out after having been instructed to wait
    // with os_task_wait_event, that it gets put into the ready state
    mock().expectOneCall("os_running");
    const auto id2 {os_task_create( dummy_task, NULL, 3, NULL, 0, 0 )};

    const auto id2_timeout {9};
    os_task_wait_event(id2, event_id, false, id2_timeout);

    CHECK_EQUAL( id2_timeout, os_task_timeout_get(id2) );
    CHECK_EQUAL( WAITING_EVENT_TIMEOUT, task_state_get(id2) );

    task_tick(0, id2_timeout);

    CHECK_EQUAL( READY, task_state_get(id2) );
}

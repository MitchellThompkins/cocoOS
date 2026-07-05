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
        mock().checkExpectations();
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

    os_task_wait_sem_set(id0, sem0);
    os_task_wait_sem_set(id1, sem1);

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

    os_task_wait_sem_set(id1, sem0);
    os_task_wait_sem_set(id0, sem1);

    const auto t0 = os_task_waiting_this_semaphore(sem0);
    const auto t1 = os_task_waiting_this_semaphore(sem1);
    const auto t2 = os_task_waiting_this_semaphore(sem2);

    CHECK_EQUAL(id1, t0);
    CHECK_EQUAL(id0, t1);
    CHECK_EQUAL(-1, t2);

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

    os_task_wait_sem_set(id1, sem0);
    os_task_wait_sem_set(id0, sem1);

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
    const auto event_id0 {event_create()};

    const int timeout {5};
    os_task_wait_event(id0, event_id0, false, timeout);

    CHECK_EQUAL( WAITING_EVENT_TIMEOUT, task_state_get(id0) );
    CHECK_EQUAL( timeout, os_task_timeout_get(id0) );

    // waitSingleEvent does not change the initial wait state
    os_task_wait_event(id0, event_id0, true, timeout);
    CHECK_EQUAL( WAITING_EVENT_TIMEOUT, task_state_get(id0) );

    os_task_wait_event(id0, event_id0, false, 0);

    CHECK_EQUAL( WAITING_EVENT, task_state_get(id0) );

    os_task_wait_event(id0, event_id0, true, 0);
    CHECK_EQUAL( WAITING_EVENT, task_state_get(id0) );
}

TEST(TestOsTask, test_os_task_signal_event)
{
    UT_CATALOG_ID("TASK-23");

    mock().expectOneCall("os_init");
    os_init();

    mock().expectOneCall("event_create");
    mock().setData("event_create_return", 0);
    const auto event_id0 {event_create()};


    // Test that when a task is set with os_task_wait_event with a timeout of
    // 0, it is set to the WAITING_EVENT and when signaled is set to READY
    mock().expectOneCall("os_running");
    const auto task_id0 {os_task_create( dummy_task, NULL, 1, NULL, 0, 0 )};

    const auto id0_timeout {0};
    os_task_wait_event(task_id0, event_id0, false, id0_timeout);
    CHECK_EQUAL( WAITING_EVENT, task_state_get(task_id0) );

    os_task_signal_event(event_id0);

    CHECK_EQUAL( id0_timeout, os_task_timeout_get(task_id0) );
    CHECK_EQUAL( READY, task_state_get(task_id0) );


    // Test that when a task is instructed to waitSingleEvent, that when
    // os_signal event is called it is set to the ready state
    mock().expectOneCall("os_running");
    const auto task_id1 {os_task_create( dummy_task, NULL, 2, NULL, 0, 0 )};

    const auto id1_timeout {40};
    os_task_wait_event(task_id1, event_id0, true, id1_timeout);
    os_task_signal_event(event_id0);

    CHECK_EQUAL( READY, task_state_get(task_id1) );
    CHECK_EQUAL( id1_timeout, os_task_timeout_get(task_id1) );


    // Test that when a task times out after having been instructed to wait
    // with os_task_wait_event, that it gets put into the ready state
    mock().expectOneCall("os_running");
    const auto task_id2 {os_task_create( dummy_task, NULL, 3, NULL, 0, 0 )};

    const auto id2_timeout {9};
    os_task_wait_event(task_id2, event_id0, false, id2_timeout);

    CHECK_EQUAL( id2_timeout, os_task_timeout_get(task_id2) );
    CHECK_EQUAL( WAITING_EVENT_TIMEOUT, task_state_get(task_id2) );

    task_tick(0, id2_timeout);

    CHECK_EQUAL( READY, task_state_get(task_id2) );


    // Test that when waiting for multiple tasks, it takes all of them to clear
    // to set the task to ready
    mock().expectOneCall("os_running");
    const auto task_id3 {os_task_create( dummy_task, NULL, 4, NULL, 0, 0 )};

    mock().expectOneCall("event_create");
    mock().setData("event_create_return", 1);
    const auto event_id1 {event_create()};

    mock().expectOneCall("event_create");
    mock().setData("event_create_return", 2);
    const auto event_id2 {event_create()};

    os_task_wait_event(task_id3, event_id1, false, 0);
    CHECK_EQUAL( WAITING_EVENT, task_state_get(task_id3) );

    os_task_wait_event(task_id3, event_id2, false, 0);
    CHECK_EQUAL( WAITING_EVENT, task_state_get(task_id3) );

    os_task_signal_event(event_id1);
    CHECK_EQUAL( WAITING_EVENT, task_state_get(task_id3) );

    os_task_signal_event(event_id2);
    CHECK_EQUAL( READY, task_state_get(task_id3) );
}

TEST(TestOsTask, task_create_with_message_pool)
{
    UT_CATALOG_ID("TASK-4");

    mock().expectOneCall("os_init");
    os_init();

    // os_msgQ_create mock wins the linker race over the real impl (test.o
    // directly references os_msg_post/receive which pulls in the mock archive
    // before os_task_impl introduces its os_msgQ_create dependency).
    mock().expectOneCall("os_running");
    mock().expectOneCall("os_msgQ_create");
    mock().ignoreOtherCalls();

    static Msg_t pool[4];
    const auto tid = os_task_create( dummy_task, NULL, 1, pool, 4, sizeof(Msg_t) );

    CHECK_TRUE( os_task_msgQ_get(tid) != NO_QUEUE );
}

TEST(TestOsTask, task_create_clears_wait_queue)
{
    UT_CATALOG_ID("TASK-5");

    mock().expectOneCall("os_init");
    os_init();

    mock().expectNCalls(2, "os_running");

    // Create task and put it in event-wait state
    const auto tid = os_task_create( dummy_task, NULL, 1, NULL, 0, 0 );
    os_task_wait_event( tid, 0, false, 0 );
    CHECK_EQUAL( WAITING_EVENT, task_state_get(tid) );

    // Re-initialize the task list (simulates fresh boot)
    os_task_init();

    // Re-create a task, it gets tid 0 again, wait queue must be clean
    const auto tid2 = os_task_create( dummy_task, NULL, 1, NULL, 0, 0 );
    CHECK_EQUAL( 0, tid2 );
    CHECK_EQUAL( READY, task_state_get(tid2) );

    // Signal event 0: task2 must stay READY (not wake into a wait state)
    os_task_signal_event( 0 );
    CHECK_EQUAL( READY, task_state_get(tid2) );
}

TEST(TestOsTask, task_init_reinitializes_task_list)
{
    UT_CATALOG_ID("TASK-6");

    mock().expectOneCall("os_init");
    os_init();

    const uint8_t prio1 {5};
    mock().expectNCalls(2, "os_running");
    os_task_create( dummy_task, NULL, prio1,     NULL, 0, 0 );
    os_task_create( dummy_task, NULL, prio1 + 1, NULL, 0, 0 );

    // Re-initialize: old prio registrations must be gone
    os_task_init();

    // Re-creating with prio1 must not trigger an assert (no duplicate)
    mock().expectOneCall("os_running");
    const auto tid = os_task_create( dummy_task, NULL, prio1, NULL, 0, 0 );
    CHECK_EQUAL( 0, tid );
}

TEST(TestOsTask, task_kill_sets_state)
{
    UT_CATALOG_ID("TASK-7");

    mock().expectOneCall("os_init");
    os_init();

    mock().expectOneCall("os_running");
    const auto tid = os_task_create( dummy_task, NULL, 1, NULL, 0, 0 );
    CHECK_EQUAL( READY, task_state_get(tid) );

    os_task_kill( tid );
    CHECK_EQUAL( KILLED, task_state_get(tid) );
    CHECK_TRUE( task_is_killed(tid) );
}

TEST(TestOsTask, task_get_data_returns_running_task_data)
{
    UT_CATALOG_ID("TASK-8");

    mock().expectOneCall("os_init");
    os_init();

    static uint32_t data_value {0xDEADBEEF};
    mock().expectOneCall("os_running");
    const auto tid = os_task_create( dummy_task, &data_value, 1, NULL, 0, 0 );

    mock().setData("running_tid_from_get", tid);
    mock().expectOneCall("os_get_running_tid");

    CHECK_EQUAL( &data_value, task_get_data() );
}

TEST(TestOsTask, next_ready_task_round_robin)
{
    UT_CATALOG_ID("TASK-10");
    UT_CATALOG_ID("KERNEL-11");

    mock().expectOneCall("os_init");
    os_init();

    mock().expectNCalls(3, "os_running");
    const auto id0 = os_task_create( dummy_task, NULL, 3, NULL, 0, 0 );
    const auto id1 = os_task_create( dummy_task, NULL, 2, NULL, 0, 0 );
    const auto id2 = os_task_create( dummy_task, NULL, 1, NULL, 0, 0 );

    // Round-robin cycles through all READY tasks in creation order
    CHECK_EQUAL( id0, next_ready_task() );
    CHECK_EQUAL( id1, next_ready_task() );
    CHECK_EQUAL( id2, next_ready_task() );
    CHECK_EQUAL( id0, next_ready_task() );  // wraps

    // Suspending id1 means it is skipped
    os_task_suspend( id1 );
    CHECK_EQUAL( id2, next_ready_task() );
    CHECK_EQUAL( id0, next_ready_task() );
    CHECK_EQUAL( id2, next_ready_task() );
}

TEST(TestOsTask, task_suspend_and_resume)
{
    UT_CATALOG_ID("TASK-15");
    UT_CATALOG_ID("TASK-16");

    mock().expectOneCall("os_init");
    os_init();

    mock().expectNCalls(2, "os_running");
    const auto id0 = os_task_create( dummy_task, NULL, 1, NULL, 0, 0 );
    const auto id1 = os_task_create( dummy_task, NULL, 2, NULL, 0, 0 );

    // (a) READY task becomes SUSPENDED, savedState is READY
    os_task_suspend( id0 );
    CHECK_EQUAL( SUSPENDED, task_state_get(id0) );

    // Resume restores READY
    os_task_resume( id0 );
    CHECK_EQUAL( READY, task_state_get(id0) );

    // (b) Task in WAITING_SEM, suspend sets savedState READY and internal_state 0
    os_task_wait_sem_set( id0, 0 );
    CHECK_EQUAL( WAITING_SEM, task_state_get(id0) );

    os_task_suspend( id0 );
    CHECK_EQUAL( SUSPENDED, task_state_get(id0) );
    CHECK_EQUAL( 0, task_internal_state_get(id0) );

    os_task_resume( id0 );
    CHECK_EQUAL( READY, task_state_get(id0) );

    // (c) KILLED task, suspend is a no-op
    os_task_kill( id1 );
    CHECK_EQUAL( KILLED, task_state_get(id1) );
    os_task_suspend( id1 );
    CHECK_EQUAL( KILLED, task_state_get(id1) );

    // Resume on a non-suspended task is a no-op
    CHECK_EQUAL( READY, task_state_get(id0) );
    os_task_resume( id0 );
    CHECK_EQUAL( READY, task_state_get(id0) );
}

TEST(TestOsTask, task_kill_traced_as_task17)
{
    UT_CATALOG_ID("TASK-17");

    mock().expectOneCall("os_init");
    os_init();

    mock().expectOneCall("os_running");
    const auto tid = os_task_create( dummy_task, NULL, 1, NULL, 0, 0 );

    os_task_kill( tid );
    CHECK_EQUAL( KILLED, task_state_get(tid) );
    CHECK_TRUE( task_is_killed(tid) );
}

TEST(TestOsTask, task_clear_wait_queue)
{
    UT_CATALOG_ID("TASK-19");

    mock().expectOneCall("os_init");
    os_init();

    mock().expectOneCall("os_running");
    const auto tid = os_task_create( dummy_task, NULL, 1, NULL, 0, 0 );

    // Put task into event-wait state
    os_task_wait_event( tid, 0, false, 0 );
    CHECK_EQUAL( WAITING_EVENT, task_state_get(tid) );

    // Clear the wait queue
    os_task_clear_wait_queue( tid );

    // Signaling event 0 must NOT make the task READY (queue was cleared)
    os_task_signal_event( 0 );
    CHECK_EQUAL( WAITING_EVENT, task_state_get(tid) );
}

TEST(TestOsTask, task_wait_time_set)
{
    UT_CATALOG_ID("TASK-20");

    mock().expectOneCall("os_init");
    os_init();

    mock().expectOneCall("os_running");
    const auto tid = os_task_create( dummy_task, NULL, 1, NULL, 0, 0 );

    const uint8_t clockId {3};
    const uint32_t time {10};
    os_task_wait_time_set( tid, clockId, time );

    CHECK_EQUAL( WAITING_TIME, task_state_get(tid) );
    CHECK_EQUAL( time, os_task_timeout_get(tid) );

    // Ticking master clock (id 0) does not affect a sub-clock task
    task_tick( 0, 1 );
    CHECK_EQUAL( time, os_task_timeout_get(tid) );

    // Ticking the matching sub-clock decrements the timer
    task_tick( clockId, 1 );
    CHECK_EQUAL( time - 1, os_task_timeout_get(tid) );

    // Tick until expiry, task becomes READY
    task_tick( clockId, time - 1 );
    CHECK_EQUAL( READY, task_state_get(tid) );
}

TEST(TestOsTask, task_tick_event_timeout_expiry)
{
    UT_CATALOG_ID("TASK-24");

    mock().expectOneCall("os_init");
    os_init();

    mock().expectOneCall("os_running");
    const auto tid = os_task_create( dummy_task, NULL, 1, NULL, 0, 0 );

    mock().expectOneCall("event_create");
    mock().setData("event_create_return", 0);
    const auto evt = event_create();

    const uint32_t timeout {5};
    os_task_wait_event( tid, evt, false, timeout );
    CHECK_EQUAL( WAITING_EVENT_TIMEOUT, task_state_get(tid) );

    // Expire the timeout
    task_tick( 0, timeout );
    CHECK_EQUAL( READY, task_state_get(tid) );

    // After expiry the wait queue is cleared; signaling the event keeps task READY
    os_task_signal_event( evt );
    CHECK_EQUAL( READY, task_state_get(tid) );
}

TEST(TestOsTask, task_tick_ticks_message_queue)
{
    UT_CATALOG_ID("TASK-25");

    mock().expectOneCall("os_init");
    os_init();

    // os_msg_post reference in this test anchors the msgqueue mock archive in
    // the link, so mock os_msgQ_create and os_msgQ_tick are used throughout
    // this translation unit (see TASK-4 comment for details).
    mock().expectOneCall("os_running");
    mock().expectOneCall("os_msgQ_create");
    mock().ignoreOtherCalls();

    static Msg_t pool[4];
    const auto tid = os_task_create( dummy_task, NULL, 1, pool, 4, sizeof(Msg_t) );
    const auto qid = os_task_msgQ_get( tid );
    CHECK_TRUE( qid != NO_QUEUE );

    // Anchor: direct os_msg_post reference causes linker to extract the
    // msgqueue mock archive so os_msgQ_tick resolves to the mock below.
    Msg_t dummy_msg {};
    os_msg_post( &dummy_msg, qid, 0, 0 );

    // Master-clock tick (clockId=0): task_tick must call os_msgQ_tick for each
    // task whose msgQ != NO_QUEUE
    mock().expectOneCall("os_msgQ_tick");
    task_tick( 0, 1 );

    // Sub-clock tick (clockId=1): task_tick must NOT call os_msgQ_tick
    task_tick( 1, 1 );
}

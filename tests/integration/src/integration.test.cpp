#include "CppUTest/TestHarness.h"
#include "test_macros.hpp"

#include "os_kernel.h"
#include "os_task.h"
#include "os_event.h"
#include "os_sem.h"
#include "os_assert.h"

#define SIG_SYNC     ((uint8_t)1)
#define SIG_DELAYED  ((uint8_t)2)
#define SIG_PERIODIC ((uint8_t)3)

// Shared state, static. Coroutine locals do not survive a yield.

static uint8_t recv_tid;
static uint8_t send_tid;
static uint8_t ctrl_tid;

static Msg_t recv_pool[8];

static int received_total;
static int received_sync;
static int received_delayed;
static int received_periodic;
static int async_poll_hit;

static Sem_t sem;
static Evt_t evt;

static bool g_assert_fired;

extern "C" void integration_assert_cb(const char* /*file*/,
                                       uint16_t    /*line*/,
                                       const char* /*expr*/)
{
    g_assert_fired = true;
}

// Task procedures, every user-facing macro exercised at least once

// recv_task, prio 1, blocking receive and async poll
static void recv_task(void)
{
    static Msg_t m;
    task_open();

    for (;;)
    {
        msg_receive(recv_tid, &m);     // exercises msg_receive, blocks until msg
        received_total++;
        switch (m.signal)
        {
            case SIG_SYNC:     received_sync++;     break;
            case SIG_DELAYED:  received_delayed++;  break;
            case SIG_PERIODIC: received_periodic++; break;
            default: break;
        }

        // non-blocking poll exercises msg_receive_async
        msg_receive_async(recv_tid, &m);
        if (m.signal != NO_MSG_ID)
        {
            async_poll_hit++;
            received_total++;
            switch (m.signal)
            {
                case SIG_SYNC:     received_sync++;     break;
                case SIG_DELAYED:  received_delayed++;  break;
                case SIG_PERIODIC: received_periodic++; break;
                default: break;
            }
        }
    }

    task_close();
}

// send_task, prio 2, posts all message variants then waits on sem and event
static void send_task(void)
{
    static Msg_t m;
    task_open();

    // msg_post, synchronous, no delay
    m.signal = SIG_SYNC;
    msg_post(recv_tid, m);

    // msg_post_in, arrives after 3 ticks
    m.signal = SIG_DELAYED;
    msg_post_in(recv_tid, m, 3);

    // msg_post_every, first arrives after 5 ticks then every 5 ticks
    m.signal = SIG_PERIODIC;
    msg_post_every(recv_tid, m, 5);

    // msg_post_async, non-blocking, returns MSG_QUEUE_UNDEF if queue full
    m.signal = SIG_SYNC;
    msg_post_async(recv_tid, m);

    // sem_wait, blocks until ctrl_task calls sem_signal
    sem_wait(sem);

    // event_wait, blocks until ctrl_task calls event_signal
    event_wait(evt);

    task_close();
}

// ctrl_task, prio 3, drives semaphore and event, tests task_suspend and task_resume
static void ctrl_task(void)
{
    task_open();

    // Let messages flow before touching sync primitives
    task_wait(4);

    // Suspend send_task briefly to exercise task_suspend and task_resume.
    // send_task is currently blocked on sem_wait so suspending it is safe.
    task_suspend(send_tid);
    task_wait(2);
    task_resume(send_tid);

    // Release send_task from sem_wait
    sem_signal(sem);

    task_wait(2);

    // Release send_task from event_wait
    event_signal(evt);

    task_close();
}

// Test driver helper

static void step(uint16_t n)
{
    for (uint16_t i = 0; i < n; i++)
    {
        os_tick();
        unit_test_os_schedule();
    }
}

// Test group

TEST_GROUP(Integration)
{
    void setup()
    {
        received_total    = 0;
        received_sync     = 0;
        received_delayed  = 0;
        received_periodic = 0;
        async_poll_hit    = 0;
        g_assert_fired    = false;

        os_on_assert_attach_callback(integration_assert_cb);

        os_init();

        sem = sem_bin_create(0);    // starts locked, send_task blocks until signaled
        evt = event_create();

        recv_tid = os_task_create(recv_task, NULL, 1,
                                  recv_pool, 8, sizeof(Msg_t));
        send_tid = os_task_create(send_task, NULL, 2, NULL, 0, 0);
        ctrl_tid = os_task_create(ctrl_task, NULL, 3, NULL, 0, 0);
    }

    void teardown()
    {
        // If the OS assert fired the test data is unreliable
        CHECK_FALSE(g_assert_fired);
    }
};

// Tests

TEST(Integration, public_api_smoke_test)
{
    UT_CATALOG_ID("KERNEL-1");
    UT_CATALOG_ID("KERNEL-2");

    // 30 ticks, enough for sync at 0, delayed at 3, periodic at 5 and 10, sem and event around tick 8
    step(30);

    // sync message posted at tick 0, received by tick 1
    CHECK(received_sync >= 1);

    // delayed message posted at tick 0 with delay 3, received by tick 4
    CHECK(received_delayed >= 1);

    // periodic message first arrives at tick 5
    CHECK(received_periodic >= 1);

    // total sanity, at minimum sync and delayed and periodic
    CHECK(received_total >= 3);
}

// ===== Phase 10: macro coverage extension =====

// sub-clock: task_wait_id / os_sub_tick / os_sub_nTick

static int g_subclock_count;

static void subclock_task(void)
{
    task_open();
    for (;;)
    {
        task_wait_id(2, 3);
        g_subclock_count++;
    }
    task_close();
}

TEST(Integration, task_wait_id_driven_by_sub_clock)
{
    UT_CATALOG_ID("KERNEL-6");
    UT_CATALOG_ID("KERNEL-15");

    os_init();
    g_subclock_count = 0;
    os_task_create(subclock_task, NULL, 1, NULL, 0, 0);

    // Task starts and blocks on task_wait_id(clock=2, ticks=3)
    unit_test_os_schedule();

    // Master-clock ticks must not advance sub-clock 2
    for (int i = 0; i < 3; i++)
    {
        os_tick();
        unit_test_os_schedule();
    }
    CHECK_EQUAL(0, g_subclock_count);

    // 3 individual sub-clock-2 ticks wake the task once
    for (int i = 0; i < 3; i++)
    {
        os_sub_tick(2);
        unit_test_os_schedule();
    }
    CHECK_EQUAL(1, g_subclock_count);

    // os_sub_nTick(2, 3) delivers the full wait in one call
    g_subclock_count = 0;
    os_sub_nTick(2, 3);
    unit_test_os_schedule();
    CHECK_EQUAL(1, g_subclock_count);
}

// event_wait_timeout / event_get_timeout (timeout path)

static uint32_t g_etimeout_val;
static bool     g_etimeout_fired;
static Evt_t    g_timeout_evt;

static void timeout_task(void)
{
    task_open();
    event_wait_timeout(g_timeout_evt, 5);
    g_etimeout_val   = event_get_timeout();
    g_etimeout_fired = true;
    task_close();
}

TEST(Integration, event_wait_timeout_expires)
{
    UT_CATALOG_ID("TASK-21");
    UT_CATALOG_ID("KERNEL-3");

    os_init();
    g_etimeout_val   = 0xDEADu;
    g_etimeout_fired = false;
    g_timeout_evt    = event_create();

    os_task_create(timeout_task, NULL, 1, NULL, 0, 0);

    // Task starts and blocks on event_wait_timeout(5)
    unit_test_os_schedule();

    // 5 master-clock ticks expire the timeout; the 5th schedule runs the task
    for (int i = 0; i < 5; i++)
    {
        os_tick();
        unit_test_os_schedule();
    }

    CHECK_TRUE(g_etimeout_fired);
    CHECK_EQUAL(0u, g_etimeout_val);   // timed out: remaining ticks == 0
}

// event_wait_timeout signaled before the deadline

static uint32_t g_early_timeout_val;
static bool     g_early_woke;
static Evt_t    g_early_evt;

static void ewt_waiter(void)
{
    task_open();
    event_wait_timeout(g_early_evt, 20);
    g_early_timeout_val = event_get_timeout();
    g_early_woke        = true;
    task_close();
}

static void ewt_signaler(void)
{
    task_open();
    task_wait(3);
    event_signal(g_early_evt);
    task_close();
}

TEST(Integration, event_wait_timeout_early_signal)
{
    UT_CATALOG_ID("TASK-21");

    os_init();
    g_early_timeout_val = 0;
    g_early_woke        = false;
    g_early_evt         = event_create();

    os_task_create(ewt_waiter,   NULL, 1, NULL, 0, 0);
    os_task_create(ewt_signaler, NULL, 2, NULL, 0, 0);

    for (int i = 0; i < 8; i++)
    {
        os_tick();
        unit_test_os_schedule();
    }

    CHECK_TRUE(g_early_woke);
    CHECK(g_early_timeout_val > 0u);   // non-zero: signaled before deadline
}

// event_wait_multiple: wake on ANY of the provided events

static bool  g_multi_any_woke;
static Evt_t g_multi_last_any;
static Evt_t g_multi_e1;
static Evt_t g_multi_e2;

static void multi_any_waiter(void)
{
    task_open();
    event_wait_multiple(0, g_multi_e1, g_multi_e2);
    g_multi_last_any = event_last_signaled_get();
    g_multi_any_woke = true;
    task_close();
}

static void multi_any_signaler(void)
{
    task_open();
    task_wait(2);
    event_signal(g_multi_e2);
    task_close();
}

TEST(Integration, event_wait_multiple_any)
{
    UT_CATALOG_ID("EVENT-6");

    os_init();
    g_multi_any_woke = false;
    g_multi_last_any = (Evt_t)NO_EVENT;
    g_multi_e1       = event_create();
    g_multi_e2       = event_create();

    os_task_create(multi_any_waiter,   NULL, 1, NULL, 0, 0);
    os_task_create(multi_any_signaler, NULL, 2, NULL, 0, 0);

    for (int i = 0; i < 6; i++)
    {
        os_tick();
        unit_test_os_schedule();
    }

    CHECK_TRUE(g_multi_any_woke);
    CHECK_EQUAL(g_multi_e2, g_multi_last_any);
}

// event_wait_multiple: wake only when ALL events have been signaled

static bool  g_multi_all_woke;
static Evt_t g_multi_all_e1;
static Evt_t g_multi_all_e2;

static void multi_all_waiter(void)
{
    task_open();
    event_wait_multiple(1, g_multi_all_e1, g_multi_all_e2);
    g_multi_all_woke = true;
    task_close();
}

static void multi_all_signal_e1(void)
{
    task_open();
    task_wait(2);
    event_signal(g_multi_all_e1);
    task_close();
}

static void multi_all_signal_e2(void)
{
    task_open();
    task_wait(5);
    event_signal(g_multi_all_e2);
    task_close();
}

TEST(Integration, event_wait_multiple_all)
{
    UT_CATALOG_ID("EVENT-6");

    os_init();
    g_multi_all_woke = false;
    g_multi_all_e1   = event_create();
    g_multi_all_e2   = event_create();

    os_task_create(multi_all_waiter,    NULL, 1, NULL, 0, 0);
    os_task_create(multi_all_signal_e1, NULL, 2, NULL, 0, 0);
    os_task_create(multi_all_signal_e2, NULL, 3, NULL, 0, 0);

    // After e1 fires the waiter still needs e2
    for (int i = 0; i < 4; i++)
    {
        os_tick();
        unit_test_os_schedule();
    }
    CHECK_FALSE(g_multi_all_woke);

    // After e2 fires the waiter wakes
    for (int i = 0; i < 8; i++)
    {
        os_tick();
        unit_test_os_schedule();
    }
    CHECK_TRUE(g_multi_all_woke);
}

// event_ISR_signal: wakes a waiting task and records ISR_TID as the source

static bool  g_isr_evt_woke;
static Evt_t g_isr_evt;

static void isr_evt_waiter(void)
{
    task_open();
    event_wait(g_isr_evt);
    g_isr_evt_woke = true;
    task_close();
}

TEST(Integration, event_ISR_signal_wakes_task)
{
    UT_CATALOG_ID("EVENT-7");
    UT_CATALOG_ID("EVENT-8");

    os_init();
    g_isr_evt_woke = false;
    g_isr_evt      = event_create();

    os_task_create(isr_evt_waiter, NULL, 1, NULL, 0, 0);

    // Task starts and blocks on event_wait
    unit_test_os_schedule();

    // Signal from ISR context: no OS_YIELD, task becomes READY immediately
    event_ISR_signal(g_isr_evt);

    unit_test_os_schedule();

    CHECK_TRUE(g_isr_evt_woke);
    CHECK_EQUAL((uint8_t)ISR_TID, event_signaling_taskId_get(g_isr_evt));
}

// sem_ISR_signal: releases a sem-waiting task without scheduling

static bool  g_isr_sem_woke;
static Sem_t g_isr_sem;

static void isr_sem_waiter(void)
{
    task_open();
    sem_wait(g_isr_sem);
    g_isr_sem_woke = true;
    task_close();
}

TEST(Integration, sem_ISR_signal_wakes_task)
{
    UT_CATALOG_ID("TASK-11");

    os_init();
    g_isr_sem_woke = false;
    g_isr_sem      = sem_bin_create(0);   // starts locked

    os_task_create(isr_sem_waiter, NULL, 1, NULL, 0, 0);

    // Task starts and blocks on sem_wait
    unit_test_os_schedule();

    // Signal from ISR context (no schedule)
    sem_ISR_signal(g_isr_sem);

    unit_test_os_schedule();

    CHECK_TRUE(g_isr_sem_woke);
}

// event_wait_ex: callback fires inside os_wait_event before the task yields

static bool  g_event_ex_cb_fired;
static Evt_t g_ex_evt;

static void event_ex_cb(void) { g_event_ex_cb_fired = true; }

static void event_ex_task(void)
{
    task_open();
    event_wait_ex(g_ex_evt, event_ex_cb);
    task_close();
}

TEST(Integration, event_wait_ex_callback_fires_before_block)
{
    UT_CATALOG_ID("EVENT-5");

    os_init();
    g_event_ex_cb_fired = false;
    g_ex_evt            = event_create();

    os_task_create(event_ex_task, NULL, 1, NULL, 0, 0);

    // One schedule step: task runs to event_wait_ex; callback fires, task blocks
    unit_test_os_schedule();
    CHECK_TRUE(g_event_ex_cb_fired);

    // Unblock the task so it can close cleanly
    event_ISR_signal(g_ex_evt);
    unit_test_os_schedule();
}

// event_wait_timeout_ex: timeout path + callback together

static bool     g_timeout_ex_cb_fired;
static bool     g_timeout_ex_woke;
static uint32_t g_timeout_ex_remaining;
static Evt_t    g_timeout_ex_evt;

static void timeout_ex_cb(void) { g_timeout_ex_cb_fired = true; }

static void timeout_ex_task(void)
{
    task_open();
    event_wait_timeout_ex(g_timeout_ex_evt, 10, timeout_ex_cb);
    g_timeout_ex_remaining = event_get_timeout();
    g_timeout_ex_woke      = true;
    task_close();
}

static void timeout_ex_signaler(void)
{
    task_open();
    task_wait(4);
    event_signal(g_timeout_ex_evt);
    task_close();
}

TEST(Integration, event_wait_timeout_ex_callback_and_early_signal)
{
    UT_CATALOG_ID("EVENT-5");
    UT_CATALOG_ID("TASK-21");

    os_init();
    g_timeout_ex_cb_fired  = false;
    g_timeout_ex_woke      = false;
    g_timeout_ex_remaining = 0;
    g_timeout_ex_evt       = event_create();

    os_task_create(timeout_ex_task,     NULL, 1, NULL, 0, 0);
    os_task_create(timeout_ex_signaler, NULL, 2, NULL, 0, 0);

    // First step: task runs to event_wait_timeout_ex; callback fires, task blocks
    unit_test_os_schedule();
    CHECK_TRUE(g_timeout_ex_cb_fired);

    // Signaler fires after 4 ticks; task resumes with remaining ticks > 0
    for (int i = 0; i < 8; i++)
    {
        os_tick();
        unit_test_os_schedule();
    }

    CHECK_TRUE(g_timeout_ex_woke);
    CHECK(g_timeout_ex_remaining > 0u);
}

// msg_receive_ex: callback fires when the queue is empty before the task yields

static bool    g_msg_ex_cb_fired;
static Msg_t   g_msg_ex_pool[4];
static uint8_t g_msg_ex_tid;

static void msg_ex_cb(void) { g_msg_ex_cb_fired = true; }

static void msg_ex_task(void)
{
    static Msg_t m;
    task_open();
    msg_receive_ex(g_msg_ex_tid, &m, msg_ex_cb);
    task_close();
}

TEST(Integration, msg_receive_ex_callback_fires_before_block)
{
    UT_CATALOG_ID("EVENT-5");

    os_init();
    g_msg_ex_cb_fired = false;

    g_msg_ex_tid = os_task_create(msg_ex_task, NULL, 1,
                                  g_msg_ex_pool, 4, sizeof(Msg_t));

    // One schedule step: task tries to receive from an empty queue,
    // callback fires, then task blocks on the queue change event
    unit_test_os_schedule();
    CHECK_TRUE(g_msg_ex_cb_fired);
}

#include "CppUTest/TestHarness.h"
#include "test_macros.hpp"

#include "os_kernel.h"
#include "os_task.h"
#include "os_event.h"
#include "os_sem.h"
#include "os_assert.h"

// ---------------------------------------------------------------------------
// Message signal IDs
// ---------------------------------------------------------------------------

#define SIG_SYNC     ((uint8_t)1)
#define SIG_DELAYED  ((uint8_t)2)
#define SIG_PERIODIC ((uint8_t)3)

// ---------------------------------------------------------------------------
// Shared state (static — coroutine locals don't survive a yield)
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Task procedures — every user-facing macro exercised at least once
// ---------------------------------------------------------------------------

// recv_task (prio 1, highest): blocking receive + async poll
static void recv_task(void)
{
    static Msg_t m;
    task_open();

    for (;;)
    {
        msg_receive(recv_tid, &m);     // exercises msg_receive (blocks until msg)
        received_total++;
        switch (m.signal)
        {
            case SIG_SYNC:     received_sync++;     break;
            case SIG_DELAYED:  received_delayed++;  break;
            case SIG_PERIODIC: received_periodic++; break;
            default: break;
        }

        // non-blocking poll right after — exercises msg_receive_async
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

// send_task (prio 2): posts all message variants, then waits on sem and event
static void send_task(void)
{
    static Msg_t m;
    task_open();

    // msg_post — synchronous, no delay
    m.signal = SIG_SYNC;
    msg_post(recv_tid, m);

    // msg_post_in — arrives after 3 ticks
    m.signal = SIG_DELAYED;
    msg_post_in(recv_tid, m, 3);

    // msg_post_every — first arrives after 5 ticks, then every 5 ticks
    m.signal = SIG_PERIODIC;
    msg_post_every(recv_tid, m, 5);

    // msg_post_async — non-blocking; returns MSG_QUEUE_UNDEF if queue full
    m.signal = SIG_SYNC;
    msg_post_async(recv_tid, m);

    // sem_wait — blocks until ctrl_task calls sem_signal
    sem_wait(sem);

    // event_wait — blocks until ctrl_task calls event_signal
    event_wait(evt);

    task_close();
}

// ctrl_task (prio 3, lowest): drives semaphore/event; tests task_suspend/resume
static void ctrl_task(void)
{
    task_open();

    // Let messages flow before touching sync primitives
    task_wait(4);

    // Suspend send_task briefly to exercise task_suspend/task_resume macros.
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

// ---------------------------------------------------------------------------
// Test driver helper
// ---------------------------------------------------------------------------

static void step(uint16_t n)
{
    for (uint16_t i = 0; i < n; i++)
    {
        os_tick();
        unit_test_os_schedule();
    }
}

// ---------------------------------------------------------------------------
// Test group
// ---------------------------------------------------------------------------

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

        sem = sem_bin_create(0);    // starts locked — send_task blocks until signaled
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

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

TEST(Integration, all_macros_compile_and_run)
{
    UT_CATALOG_ID("KERNEL-1");
    UT_CATALOG_ID("KERNEL-2");

    // 30 ticks: enough for sync(0), delayed(3), periodic(5,10,…), sem/event(~8)
    step(30);

    // sync message posted at tick 0, received by tick 1
    CHECK(received_sync >= 1);

    // delayed message posted at tick 0 with delay 3, received by tick 4
    CHECK(received_delayed >= 1);

    // periodic message first arrives at tick 5
    CHECK(received_periodic >= 1);

    // total sanity: at minimum sync + delayed + periodic
    CHECK(received_total >= 3);
}

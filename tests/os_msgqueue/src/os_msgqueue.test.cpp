#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "test_macros.hpp"

#include "CppUTest/TestOutput.h"

#include "platform.h"

#include "os_msgqueue.h"

// A message type that extends Msg_t with a payload byte
struct TestMsg_t
{
    Msg_t base;
    uint8_t payload;
};

static TestMsg_t make_msg(uint8_t signal, uint8_t payload, uint32_t delay = 0, uint32_t reload = 0)
{
    TestMsg_t m {};
    m.base.signal  = signal;
    m.base.delay   = delay;
    m.base.reload  = reload;
    m.payload      = payload;
    return m;
}

TEST_GROUP(TestOsMsgqueue)
{
    void setup()
    {
        os_msgQ_init();
    }

    void teardown()
    {
        mock().checkExpectations();
        mock().clear();
    }
};


TEST(TestOsMsgqueue, test_os_create)
{
    UT_CATALOG_ID("MSGQUEUE-2");
    UT_CATALOG_ID("MSGQUEUE-4");

    // Buffer large enough for nMessages entries of msgSize bytes each
    static uint8_t buf[10 * sizeof(TestMsg_t)];
    const uint8_t  nMessages = 10;
    const uint16_t msgSize   = sizeof(TestMsg_t);
    const uint8_t  task_id   = 12;

    mock().expectOneCall("event_create");
    mock().setData("event_create_return", 7);

    const auto queue_id = os_msgQ_create( (Msg_t*)buf, nMessages, msgSize, task_id );

    // First queue created should get ID 0
    CHECK_EQUAL(0, queue_id);

    // os_msgQ_find should locate the queue by its owning task
    CHECK_EQUAL(queue_id, os_msgQ_find(task_id));

    // The change event should be the one returned by event_create
    CHECK_EQUAL(7, os_msgQ_event_get(queue_id));
}


TEST(TestOsMsgqueue, test_os_post)
{
    UT_CATALOG_ID("MSGQUEUE-7");
    UT_CATALOG_ID("MSGQUEUE-8");

    static TestMsg_t buf[4];
    const uint8_t  nMessages = 4;
    const uint16_t msgSize   = sizeof(TestMsg_t);
    const uint8_t  task_id   = 1;

    mock().expectOneCall("event_create");
    mock().setData("event_create_return", 0);

    const MsgQ_t queue = os_msgQ_create( (Msg_t*)buf, nMessages, msgSize, task_id );

    // Post a message and check that it is accepted
    TestMsg_t msg_a = make_msg(0x01, 0xAA);
    uint8_t result = os_msg_post( (Msg_t*)&msg_a, queue, 0, 0 );
    CHECK_EQUAL(MSG_QUEUE_POSTED, result);

    // Post until the queue is full (capacity = nMessages - 1 because of
    // the head/tail sentinel slot)
    TestMsg_t msg_b = make_msg(0x02, 0xBB);
    for (int i = 1; i < (int)nMessages - 1; i++)
    {
        result = os_msg_post( (Msg_t*)&msg_b, queue, 0, 0 );
        CHECK_EQUAL(MSG_QUEUE_POSTED, result);
    }

    // One more push should return FULL
    TestMsg_t msg_c = make_msg(0x03, 0xCC);
    result = os_msg_post( (Msg_t*)&msg_c, queue, 0, 0 );
    CHECK_EQUAL(MSG_QUEUE_FULL, result);

    // Posting to an invalid queue should return UNDEF
    result = os_msg_post( (Msg_t*)&msg_a, (MsgQ_t)NO_QUEUE, 0, 0 );
    CHECK_EQUAL(MSG_QUEUE_UNDEF, result);
}


TEST(TestOsMsgqueue, test_os_rcv)
{
    UT_CATALOG_ID("MSGQUEUE-9");
    UT_CATALOG_ID("MSGQUEUE-10");
    UT_CATALOG_ID("MSGQUEUE-11");
    UT_CATALOG_ID("MSGQUEUE-12");

    static TestMsg_t buf[4];
    const uint8_t  nMessages = 4;
    const uint16_t msgSize   = sizeof(TestMsg_t);
    const uint8_t  task_id   = 2;

    mock().expectOneCall("event_create");
    mock().setData("event_create_return", 0);

    const MsgQ_t queue = os_msgQ_create( (Msg_t*)buf, nMessages, msgSize, task_id );

    // Empty queue should return EMPTY
    TestMsg_t rx {};
    uint8_t result = os_msg_receive( (Msg_t*)&rx, queue );
    CHECK_EQUAL(MSG_QUEUE_EMPTY, result);

    // Post a delayed message; queue should still appear empty until tick fires
    TestMsg_t delayed = make_msg(0xDD, 0x11, /*delay=*/3, /*reload=*/0);
    os_msg_post( (Msg_t*)&delayed, queue, 3, 0 );

    result = os_msg_receive( (Msg_t*)&rx, queue );
    CHECK_EQUAL(MSG_QUEUE_EMPTY, result);

    // Tick three times to expire the delay
    os_msgQ_tick(queue);
    os_msgQ_tick(queue);
    mock().expectOneCall("os_signal_event");
    mock().expectOneCall("os_event_set_signaling_tid");
    os_msgQ_tick(queue);

    result = os_msg_receive( (Msg_t*)&rx, queue );
    CHECK_EQUAL(MSG_QUEUE_RECEIVED, result);
    CHECK_EQUAL(0xDD, rx.base.signal);

    // Post a non-delayed message and receive it immediately
    TestMsg_t immediate = make_msg(0xAA, 0x22);
    os_msg_post( (Msg_t*)&immediate, queue, 0, 0 );
    result = os_msg_receive( (Msg_t*)&rx, queue );
    CHECK_EQUAL(MSG_QUEUE_RECEIVED, result);
    CHECK_EQUAL(0xAA, rx.base.signal);

    // Post a periodic message; after receipt it should be re-queued
    TestMsg_t periodic = make_msg(0xBB, 0x33, /*delay=*/2, /*reload=*/2);
    os_msg_post( (Msg_t*)&periodic, queue, 2, 2 );

    // Tick twice so the periodic message becomes ready
    os_msgQ_tick(queue);
    mock().expectOneCall("os_signal_event");
    mock().expectOneCall("os_event_set_signaling_tid");
    os_msgQ_tick(queue);

    result = os_msg_receive( (Msg_t*)&rx, queue );
    CHECK_EQUAL(MSG_QUEUE_RECEIVED, result);
    CHECK_EQUAL(0xBB, rx.base.signal);

    // The periodic message should have been re-enqueued; receive returns EMPTY
    // until it ticks down again
    result = os_msg_receive( (Msg_t*)&rx, queue );
    CHECK_EQUAL(MSG_QUEUE_EMPTY, result);
}


TEST(TestOsMsgqueue, test_os_init)
{
    UT_CATALOG_ID("MSGQUEUE-1");

    // After setup() (which calls os_msgQ_init), no queues exist
    CHECK_EQUAL(NO_QUEUE, os_msgQ_find(42));
    CHECK_EQUAL(NO_EVENT, os_msgQ_event_get(0));

    // Create a queue, then re-init; verify state is fully reset
    static TestMsg_t buf[4];
    mock().expectOneCall("event_create");
    mock().setData("event_create_return", 5);
    const auto q = os_msgQ_create((Msg_t*)buf, 4, sizeof(TestMsg_t), 42);
    CHECK_EQUAL(0, q);
    CHECK_EQUAL(0, os_msgQ_find(42));

    os_msgQ_init();

    CHECK_EQUAL(NO_QUEUE, os_msgQ_find(42));
    CHECK_EQUAL(NO_EVENT, os_msgQ_event_get(0));
}


TEST(TestOsMsgqueue, test_os_create_limit)
{
    UT_CATALOG_ID("MSGQUEUE-3");

    static TestMsg_t bufs[N_QUEUES][2];
    mock().setData("event_create_return", 0);
    mock().expectNCalls(N_QUEUES, "event_create");
    for (int i = 0; i < N_QUEUES; i++)
    {
        os_msgQ_create((Msg_t*)bufs[i], 2, sizeof(TestMsg_t), (uint8_t)i);
    }

    // One more queue exceeds N_QUEUES, assert fires and returns 1
    mock().expectOneCall("os_on_assert");
    const auto excess = os_msgQ_create((Msg_t*)bufs[0], 2, sizeof(TestMsg_t), 99);
    CHECK_EQUAL(1, excess);
}


TEST(TestOsMsgqueue, test_os_find)
{
    UT_CATALOG_ID("MSGQUEUE-5");

    // No queues yet, unknown task returns NO_QUEUE
    CHECK_EQUAL(NO_QUEUE, os_msgQ_find(7));

    static TestMsg_t buf_a[4], buf_b[4];

    mock().expectOneCall("event_create");
    mock().setData("event_create_return", 0);
    const MsgQ_t q_a = os_msgQ_create((Msg_t*)buf_a, 4, sizeof(TestMsg_t), 10);

    mock().expectOneCall("event_create");
    mock().setData("event_create_return", 1);
    const MsgQ_t q_b = os_msgQ_create((Msg_t*)buf_b, 4, sizeof(TestMsg_t), 20);

    CHECK_EQUAL(q_a, os_msgQ_find(10));
    CHECK_EQUAL(q_b, os_msgQ_find(20));
    CHECK_EQUAL(NO_QUEUE, os_msgQ_find(99));
}


TEST(TestOsMsgqueue, test_os_event_get)
{
    UT_CATALOG_ID("MSGQUEUE-6");

    // No queues, any index returns NO_EVENT
    CHECK_EQUAL(NO_EVENT, os_msgQ_event_get(0));

    static TestMsg_t buf[4];
    mock().expectOneCall("event_create");
    mock().setData("event_create_return", 3);
    const MsgQ_t q = os_msgQ_create((Msg_t*)buf, 4, sizeof(TestMsg_t), 5);

    CHECK_EQUAL(3, os_msgQ_event_get(q));

    // Queue id beyond nQueues returns NO_EVENT
    CHECK_EQUAL(NO_EVENT, os_msgQ_event_get((MsgQ_t)(q + 1)));
}


TEST(TestOsMsgqueue, test_os_tick)
{
    UT_CATALOG_ID("MSGQUEUE-13");

    static TestMsg_t buf[4];
    mock().expectOneCall("event_create");
    mock().setData("event_create_return", 0);
    const MsgQ_t queue = os_msgQ_create((Msg_t*)buf, 4, sizeof(TestMsg_t), 3);

    // Post a message with delay=2
    TestMsg_t msg = make_msg(0xAB, 0x00, /*delay=*/2, /*reload=*/0);
    os_msg_post((Msg_t*)&msg, queue, 2, 0);

    // Tick 1, delay becomes 1, message not yet deliverable
    os_msgQ_tick(queue);
    TestMsg_t rx {};
    CHECK_EQUAL(MSG_QUEUE_EMPTY, os_msg_receive((Msg_t*)&rx, queue));

    // Tick 2, delay becomes 0, change event is signaled
    mock().expectOneCall("os_signal_event");
    mock().expectOneCall("os_event_set_signaling_tid");
    os_msgQ_tick(queue);

    CHECK_EQUAL(MSG_QUEUE_RECEIVED, os_msg_receive((Msg_t*)&rx, queue));
    CHECK_EQUAL(0xAB, rx.base.signal);
}


TEST(TestOsMsgqueue, test_os_rcv_undef)
{
    UT_CATALOG_ID("MSGQUEUE-14");

    TestMsg_t rx {};

    // With nQueues=0 any queue id is out of range
    CHECK_EQUAL(MSG_QUEUE_UNDEF, os_msg_receive((Msg_t*)&rx, 0));
    CHECK_EQUAL(MSG_QUEUE_UNDEF, os_msg_receive((Msg_t*)&rx, (MsgQ_t)NO_QUEUE));

    // Create one queue at id 0, nQueues is 1, id 1 is still out of range
    static TestMsg_t buf[4];
    mock().expectOneCall("event_create");
    mock().setData("event_create_return", 0);
    os_msgQ_create((Msg_t*)buf, 4, sizeof(TestMsg_t), 1);

    CHECK_EQUAL(MSG_QUEUE_UNDEF, os_msg_receive((Msg_t*)&rx, 1));
}


TEST(TestOsMsgqueue, test_msg_t_structure)
{
    UT_CATALOG_ID("MSGQUEUE-15");

    static_assert(sizeof(Msg_t::signal) == 1, "Msg_t.signal must be 1 byte");

    Msg_t m {};
    m.signal = 0xAB;
    m.delay  = 42;
    m.reload = 10;

    CHECK_EQUAL(0xAB, m.signal);
    CHECK_EQUAL(42u,  m.delay);
    CHECK_EQUAL(10u,  m.reload);
}

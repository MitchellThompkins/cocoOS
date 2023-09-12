#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "test_macros.hpp"

#include "CppUTest/TestOutput.h"

#include "platform.h"

#include "os_defines.h"
#include "os_event.h"

TEST_GROUP(TestOsEvent)
{
    void setup()
    {
        // reset num events to 0
        os_event_init();
    }

    void teardown()
    {
        mock().clear();
    }
};

TEST(TestOsEvent, successful_create_event)
{
    UT_CATALOG_ID("EVENT-1");

    for(int i{0}; i<N_TOTAL_EVENTS; i++)
    {
        CHECK_EQUAL(i, event_create());
    }

    os_event_init();
    CHECK_EQUAL(0, event_create());
}

TEST(TestOsEvent, create_too_many_events)
{
    UT_CATALOG_ID("EVENT-2");

    for(int i{0}; i<N_TOTAL_EVENTS; i++)
    {
        CHECK_EQUAL(i, event_create());
    }

    mock().expectOneCall("os_on_assert");
    event_create();
}

static bool called = false;
void foo()
{
    called = true;
}

TEST(TestOsEvent, test_os_wait_event)
{
    UT_CATALOG_ID("EVENT-5");

    const auto event_id0 {event_create()};

    struct FakeWaitEventData
    {
        uint8_t tid;
        bool wait_single_event;
        uint32_t timeout;
    };

    const FakeWaitEventData fake{99, true, 12345};

    // Call with no callback
    mock().expectOneCall("os_task_wait_event")
        .withParameter("tid", fake.tid)
        .withParameter("eventId", event_id0)
        .withParameter("waitSingleEvent", fake.wait_single_event)
        .withParameter("timeout", fake.timeout);

    os_wait_event( fake.tid,
                   event_id0,
                   fake.wait_single_event,
                   fake.timeout,
                   nullptr );

    mock().checkExpectations();
    CHECK_EQUAL( NO_TID, event_signaling_taskId_get(event_id0) );

    // Call with event_id that is too big and with callback
    CHECK_FALSE(called);

    os_wait_event( fake.tid,
                   100,
                   fake.wait_single_event,
                   fake.timeout,
                   &foo );

    // Call with callback
    CHECK_FALSE(called);

    mock().expectOneCall("os_task_wait_event")
        .withParameter("tid", fake.tid)
        .withParameter("eventId", event_id0)
        .withParameter("waitSingleEvent", fake.wait_single_event)
        .withParameter("timeout", fake.timeout);

    os_wait_event( fake.tid,
                   event_id0,
                   fake.wait_single_event,
                   fake.timeout,
                   &foo );

    mock().checkExpectations();
    CHECK_TRUE(called);
    CHECK_EQUAL(NO_TID, event_signaling_taskId_get(event_id0) );
}

TEST(TestOsEvent, test_os_wait_event_multiple)
{
    UT_CATALOG_ID("EVENT-TBD");

    struct FakeWaitEventData
    {
        uint8_t tid;
        bool wait_single_event;
        uint32_t timeout;
    };

    static constexpr int kNumEvents {5};
    decltype(event_create()) event_id[kNumEvents];

    for(int i{0}; i<kNumEvents-1; i++)
    {
        event_id[i] = event_create();
    }

    event_id[kNumEvents-1] = NO_TID;

    static constexpr int fake_tid {99};

    mock().setData("running_tid_from_get", fake_tid);
    mock().expectNCalls(kNumEvents+1, "os_get_running_tid");

    mock().expectOneCall("os_task_clear_wait_queue")
        .withParameter("tid", fake_tid);

    for(int i{0}; i<kNumEvents-1; i++)
    {
        mock().expectOneCall("os_task_wait_event")
            .withParameter("tid", fake_tid)
            .withParameter("eventId", event_id[i])
            .withParameter("waitSingleEvent", true)
            .withParameter("timeout", 0);
    }

    os_wait_multiple( false,
                   event_id[0], event_id[1], event_id[2], event_id[3],
                   event_id[4] );


}

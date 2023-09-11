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
}

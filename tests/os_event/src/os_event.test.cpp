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

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "test_macros.hpp"

#include "CppUTest/TestOutput.h"

#include "platform.h"

#include "os_msgqueue.h"

TEST_GROUP(TestOsMsgqueue)
{
    void setup()
    {
    }

    void teardown()
    {
        mock().clear();
    }
};


TEST(TestOsMsgqueue, test_os_create)
{
    UT_CATALOG_ID("MSGQUEUE-TBD");

    struct FakeBuff
    {
        Msg_t msg_queue;
        char data[10];
        bool info;
        int16_t foo;
    };

    FakeBuff fb
    {
        {},
        {10,9,8,7,6,5,4,3,2,1},
        true,
        123
    };

    uint8_t nMessages = 10;
    uint16_t msgSize = 11;
    uint16_t task_id = 12;

    //FakeBuff fb[5];

    auto msg_queue_id = os_msgQ_create( &fb, nMessages, msgSize, task_id );

    CHECK_EQUAL(1, msg_queue_id);
}


//TEST(TestOsMsgqueue, test_os_post)
//{
//}
//
//
//TEST(TestOsMsgqueue, test_os_rcv)
//{
//}

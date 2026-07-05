#include "CppUTestExt/MockSupport_c.h"

#include "os_msgqueue.h"

void os_msgQ_init( void )
{
    mock_c()->actualCall("os_msgQ_init");
}

MsgQ_t os_msgQ_create( const Msg_t *const buffer,
                       const uint8_t nMessages,
                       const uint16_t msgSize,
                       const uint8_t task_id )
{
    return (MsgQ_t)mock_c()->actualCall("os_msgQ_create")
                            ->returnIntValueOrDefault(0);
}

MsgQ_t os_msgQ_find( const uint8_t task_id )
{
    return (MsgQ_t)mock_c()->actualCall("os_msgQ_find")
                            ->returnIntValueOrDefault(NO_QUEUE);
}

Evt_t os_msgQ_event_get( const MsgQ_t queue )
{
    return (Evt_t)mock_c()->actualCall("os_msgQ_event_get")
                           ->returnIntValueOrDefault(NO_EVENT);
}

void os_msgQ_tick( const MsgQ_t queue )
{
    mock_c()->actualCall("os_msgQ_tick");
}

MsgQResult_t os_msg_post( Msg_t *msg,
                           const MsgQ_t queue,
                           const uint32_t delay,
                           const uint32_t period )
{
    return (MsgQResult_t)mock_c()->actualCall("os_msg_post")
                                  ->returnIntValueOrDefault(MSG_QUEUE_POSTED);
}

MsgQResult_t os_msg_receive( Msg_t *msg,
                              const MsgQ_t queue )
{
    return (MsgQResult_t)mock_c()->actualCall("os_msg_receive")
                                  ->returnIntValueOrDefault(MSG_QUEUE_EMPTY);
}

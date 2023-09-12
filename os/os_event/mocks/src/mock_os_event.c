#include "CppUTestExt/MockSupport_c.h"

#include "os_event.h"

void os_event_init( void )
{
    mock_c()->actualCall("os_event_init");
}

void os_wait_event( uint8_t tid,
                    Evt_t ev,
                    bool waitSingleEvent,
                    uint32_t timeout,
                    void (*cb)(void) )
{
    mock_c()->actualCall("os_wait_event");
}

void os_wait_multiple( bool waitAll, ...)
{
    mock_c()->actualCall("os_wait_multiple");
}

void os_signal_event( Evt_t ev )
{
    mock_c()->actualCall("os_signal_event");
}

void os_event_set_signaling_tid( Evt_t ev, uint8_t tid )
{
    mock_c()->actualCall("os_event_set_signaling_tid");
}

Evt_t event_last_signaled_get(void)
{
    mock_c()->actualCall("event_last_signaled_get");
}

Evt_t event_create( void )
{
    int v = (int)mock_c()->getData("event_create_return").value.intValue;
    mock_c()->actualCall("event_create")->returnIntValueOrDefault(v);
}

#include "os_assert.h"
#include "os_defines.h"
#include "os_event.h"
#include "os_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

// Event type
typedef struct
{
    uint8_t id;
    uint8_t signaledByTid;
} Event_t;


// Event list
#if( N_TOTAL_EVENTS > 0 )
static Event_t eventList[ N_TOTAL_EVENTS ];

// Keeping track of number of created events
static Evt_t nEvents;
static Evt_t lastSignaledEvent = NO_EVENT;
#endif

Evt_t event_create( void )
{
#if( N_TOTAL_EVENTS > 0 )
    os_assert_with_return( nEvents < N_TOTAL_EVENTS, 1 );

    eventList[ nEvents ].id = nEvents;
    eventList[ nEvents ].signaledByTid = NO_TID;

    ++nEvents;

    return nEvents - 1;
#else
    return 0;
#endif
}


void os_event_init(void)
{
#if( N_TOTAL_EVENTS > 0 )
    nEvents = 0;
#endif
}


uint8_t event_signaling_taskId_get( Evt_t event_id )
{
#if( N_TOTAL_EVENTS > 0 )
    return eventList[ event_id ].signaledByTid;
#else
    return 0;
#endif
}


Evt_t event_last_signaled_get(void)
{
#if( N_TOTAL_EVENTS > 0 )
    return lastSignaledEvent;
#else
    return NO_EVENT;
#endif
}


void os_wait_event( uint8_t tid,
                    Evt_t event_id,
                    bool waitSingleEvent,
                    uint32_t timeout,
                    void (*call_back)(void) )
{
#if( N_TOTAL_EVENTS > 0 )
    if( event_id < nEvents )
    {
        eventList[ event_id ].signaledByTid = NO_TID;
        os_task_wait_event( tid, event_id, waitSingleEvent, timeout );

        if( call_back )
        {
            call_back();
        }
    }
#endif
}


void os_signal_event( Evt_t event_id )
{
#if( N_TOTAL_EVENTS > 0 )
    lastSignaledEvent = event_id;
    os_task_signal_event( event_id );
#endif
}


void os_event_set_signaling_tid( Evt_t event_id, uint8_t tid )
{
#if( N_TOTAL_EVENTS > 0 )
    eventList[ event_id ].signaledByTid = tid;
#endif
}


void os_wait_multiple( bool waitForAll, ... )
{
#if( N_TOTAL_EVENTS > 0 )
    va_list args;

    // First verify the list of args, if NO_EVENT is not the last event, then
    // this loop will run for up to N_TOTAL_EVENTS. This is to prevent an
    // infinite loop where NO_EVENT is _never_ found so va_args keeps scanning
    // forever. If NO_EVENT isn't the last event, assert.
    va_start( args, waitForAll );

    int event = va_arg( args, int );
    int count = 0;

    do
    {
        event = va_arg( args, int );
        count++;
    } while ( event != NO_EVENT && count <= N_TOTAL_EVENTS );

    va_end(args);

    os_assert(event == NO_EVENT);

    // Set the wait states for this task on provided events.
    const uint16_t running_tid = os_get_running_tid();

    va_start( args, waitForAll );

    os_task_clear_wait_queue( running_tid );
    event = va_arg( args, int );

    do
    {
        os_task_wait_event( running_tid, (Evt_t)event, !waitForAll, 0 );
        event = va_arg( args, int );
    } while ( event != NO_EVENT );

    va_end(args);
#endif
}

#ifdef __cplusplus
}
#endif

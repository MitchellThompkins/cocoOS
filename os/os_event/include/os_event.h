#ifndef OS_EVENT_H
#define OS_EVENT_H

/** @file os_event.h Event header file*/

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#define OS_WAIT_SINGLE_EVENT(x,timeout, cb) \
\
do {\
    os_wait_event(os_get_running_tid(),x,1,timeout, cb);\
    OS_YIELD;\
} while (0)


#define OS_WAIT_MULTIPLE_EVENTS( waitAll, args...)\
\
do {\
 os_wait_multiple(waitAll, args, NO_EVENT);\
 OS_YIELD;\
} while (0)


#define OS_SIGNAL_EVENT(event)  do {\
                                os_signal_event(event);\
                                os_event_set_signaling_tid(\
                                        event,\
                                        os_get_running_tid() );\
                                OS_YIELD;\
                                } while (0)


#define OS_INT_SIGNAL_EVENT(event)  do {\
                                    os_signal_event(event);\
                                    os_event_set_signaling_tid( event, ISR_TID );\
                                    } while (0)


#define OS_GET_TASK_TIMEOUT_VALUE()  os_task_timeout_get(os_get_running_tid())


#ifdef N_TOTAL_EVENTS
    #define EVENT_QUEUE_SIZE    ((N_TOTAL_EVENTS/9)+1)
#else
    #define EVENT_QUEUE_SIZE    1
#endif


typedef uint8_t Evt_t;

typedef struct
{
    uint8_t eventList[ EVENT_QUEUE_SIZE ];
} EventQueue_t;


void os_event_init( void );

void os_wait_event( uint8_t tid,
                    Evt_t ev,
                    bool waitSingleEvent,
                    uint32_t timeout,
                    void (*call_back)(void) );

void os_wait_multiple( uint8_t waitAll, ... );

void os_signal_event( Evt_t ev );

void os_event_set_signaling_tid( Evt_t ev, uint8_t tid );

Evt_t event_last_signaled_get( void );

Evt_t event_create( void );

uint8_t event_signaling_taskId_get( Evt_t ev );

#ifdef __cplusplus
}
#endif

#endif

#ifndef OS_EVENT_H
#define OS_EVENT_H

/** @file os_event.h Event header file*/

#include <stdarg.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#define EVENT_OFS1   10000
#define EVENT_OFS2   11000
#define EVENT_OFS3   12000

#define OS_WAIT_SINGLE_EVENT(x,timeout, cb) do {\
                                os_wait_event(os_get_running_tid(),x,1,timeout, cb);\
                                OS_SCHEDULE(EVENT_OFS1);\
                               } while (0)


#define OS_WAIT_MULTIPLE_EVENTS( waitAll, args...)  do {\
                                os_wait_multiple(waitAll, args, NO_EVENT);\
                                OS_SCHEDULE(EVENT_OFS2);\
                               } while (0)


#define OS_SIGNAL_EVENT(event)  do {\
                                os_signal_event(event);\
                                os_event_set_signaling_tid( event, os_get_running_tid() );\
                                OS_SCHEDULE(EVENT_OFS3);\
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

typedef struct {
    uint8_t eventList[ EVENT_QUEUE_SIZE ];
} EventQueue_t;


void os_event_init( void );
void os_wait_event( uint8_t tid, Evt_t ev, uint8_t waitSingleEvent, uint32_t timeout, void (*cb)(void) );
void os_wait_multiple( uint8_t waitAll, ...);
void os_signal_event( Evt_t ev );
void os_event_set_signaling_tid( Evt_t ev, uint8_t tid );
Evt_t event_last_signaled_get(void);

#ifdef __cplusplus
}
#endif

#endif

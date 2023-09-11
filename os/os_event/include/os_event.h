#ifndef OS_EVENT_H
#define OS_EVENT_H

/** @file os_event.h Event header file*/

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#define OS_WAIT_SINGLE_EVENT(x, timeout, cb) do {\
                                             os_wait_event(\
                                                     os_get_running_tid(),\
                                                     x, 1, timeout, cb);\
                                             OS_YIELD;\
                                             } while (0)


#define OS_WAIT_MULTIPLE_EVENTS( waitAll, args...) do {\
                                                   os_wait_multiple(\
                                                     waitAll, args, NO_EVENT);\
                                                   OS_YIELD;\
                                                   } while (0)


#define OS_SIGNAL_EVENT(event) do {\
                               os_signal_event(event);\
                               os_event_set_signaling_tid(\
                                       event,\
                                       os_get_running_tid() );\
                               OS_YIELD;\
                               } while (0)


#define OS_INT_SIGNAL_EVENT(event) do {\
                                   os_signal_event(event);\
                                   os_event_set_signaling_tid(\
                                        event, ISR_TID );\
                                   } while (0)


#define OS_GET_TASK_TIMEOUT_VALUE()  os_task_timeout_get(os_get_running_tid())


#ifdef N_TOTAL_EVENTS
    #define EVENT_QUEUE_SIZE  ((N_TOTAL_EVENTS/9)+1)
#else
    #define EVENT_QUEUE_SIZE  1
#endif


typedef uint8_t Evt_t;

typedef struct
{
    uint8_t eventList[ EVENT_QUEUE_SIZE ];
} EventQueue_t;


/*********************************************************************************/
/*
  @brief Creates an event.

  @return Returns the id associated with an event

  @remarks \b Usage: @n An event is created by declaring a variable of type
  Evt_t and then assigning the event_create() return value to that variable.

  @code
  Evt_t myEvent;
  myEvent = event_create();
  @endcode

*/
/*********************************************************************************/
Evt_t event_create( void );


/*********************************************************************************/
/*
   @brief initializes the event component

   @post nEvents will be set to 0
*/
/*********************************************************************************/
void os_event_init( void );


/*****************************************************************************/
/*  @brief Gets the Task Id of the task that signaled the event.

    @param event_id event

    @return Id of task that signaled the event.

    @return NO_TID if a timeout occurred before the event was signaled.

    @remarks \b Usage: @n A task can make a call to this function when it has
    resumed execution after waiting for an event to find out which other task
    signaled the event.


    @code
    event_wait(event);
    signalingTask = event_signaling_taskId_get(event);
    if ( signalingTask == Task2_id ) {
    ...
    }
    @endcode

*/
/*****************************************************************************/
uint8_t event_signaling_taskId_get( Evt_t event_id );


/*****************************************************************************/
/*   @brief Gets the last signaled event

     @return Last signaled event

     @remarks \b Usage: @n Used when waiting for multiple events, to find out
     which event was signaled.


     @code
     event_wait_multiple(0, event1, event2);
     Evt_t lastEvt = event_last_signaled_get();
     if ( lastEvt == event1 ) {
     ...
     }
     @endcode

*/
/*****************************************************************************/
Evt_t event_last_signaled_get( void );


/*****************************************************************************/
/*
   @brief Sets the specified task to wait for the specified event

   @param tid the specified task with which to associate the event

   @param event_id the specified event

   @param waitSingleEvent whether or not the event should put the task into the
   ready state

   @param timeout the timeout for the task to wait before the event can be
   signaled

   @param a function pointer to a call back function that should be called
   before returning. The reason for this is b/c this function is _generally_
   only called by one of the macros that will release control of the function.
   Therefore any behavior that is required to occur _before_ release occurs can
   still be achieved by passing function pointer to the call_back

   @return void
*/
/*****************************************************************************/
void os_wait_event( uint8_t tid,
                    Evt_t event_id,
                    bool waitSingleEvent,
                    uint32_t timeout,
                    void (*call_back)(void) );

void os_signal_event( Evt_t event_id );

void os_wait_multiple( uint8_t waitAll, ... );

void os_event_set_signaling_tid( Evt_t event_id, uint8_t tid );


#ifdef __cplusplus
}
#endif

#endif

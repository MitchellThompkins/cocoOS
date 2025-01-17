#ifndef OS_TASK_H__
#define OS_TASK_H__

/** @file os_task.h Task header file*/

#include "stdint.h"
#include "stdbool.h"

#include "os_defines.h"
#include "os_event.h"
#include "os_msgqueue.h"
#include "os_sem.h"
#include "os_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tcb tcb;

typedef enum
{
    SUSPENDED = 0,
    WAITING_SEM,
    WAITING_TIME,
    WAITING_EVENT,
    WAITING_EVENT_TIMEOUT,
    READY,
    RUNNING,
    KILLED
} TaskState_t;


#define TASK_OFS1    30000
#define TASK_OFS2    31000

#define OS_SUSPEND_TASK( id ) do {\
                                   os_task_suspend( id );\
                                   if ( id == os_get_running_tid() ) {\
                                       OS_YIELD;\
                                   }\
                                } while (0)




#define OS_RESUME_TASK( id ) do {\
                                 os_task_resume( id );\
                                 if ( id == os_get_running_tid() ) {\
                                     OS_YIELD;\
                                 }\
                             } while (0)


void os_task_init(void);

uint8_t highest_prio_ready_task( void );

uint8_t next_ready_task( void );

void os_task_ready_set( uint8_t tid );

void task_wait_sem_set( uint8_t tid,
                        Sem_t sem );

void os_task_suspend( uint8_t tid );

void os_task_resume( uint8_t tid );

void os_task_kill( uint8_t tid );

uint8_t os_task_prio_get( uint8_t tid );

void os_task_clear_wait_queue( uint8_t tid );

void os_task_wait_time_set( const uint8_t tid,
                            const uint8_t id,
                            const uint32_t time );

/*****************************************************************************/
/*
   @brief Put the specified task into the WAITING_EVENT_TIMEOUT state if the
   provided timeout is greater than 0 or the WAITING_EVENT state if the timeout
   is 0. If waitSingleEvent is true, then when the event is signaled, the
   associated task will be put into the READY state.

   @param tid the specified task with which to associate the event

   @param eventId the specified event

   @param waitSingleEvent whether or not the event should put the task into the
   ready state

   @param timeout the timeout for the task to wait before the event can be
   signaled

   @return void
*/
/*****************************************************************************/
void os_task_wait_event( uint8_t tid,
                         Evt_t eventId,
                         bool waitSingleEvent,
                         uint32_t timeout );

void task_tick( const uint8_t clockId,
                const uint32_t tickSize );

void os_task_signal_event( const Evt_t eventId );

void task_run( void );

void os_task_run_test( const uint8_t id );

uint16_t task_internal_state_get( uint8_t tid );

void os_task_internal_state_set( const uint8_t tid, const uint16_t state );

void os_task_release_waiting_task( Sem_t sem );

uint8_t os_task_waiting_this_semaphore( Sem_t sem );

MsgQ_t os_task_msgQ_get( const uint8_t tid );

void os_task_set_wait_queue(uint8_t tid, MsgQ_t queue);

MsgQ_t os_task_get_wait_queue(uint8_t tid);

void os_task_set_change_event(uint8_t tid, Evt_t event);

Evt_t os_task_get_change_event(uint8_t tid);

void os_task_set_msg_result(uint8_t tid, uint8_t result);

uint8_t os_task_get_msg_result(uint8_t tid);

uint32_t os_task_timeout_get(uint8_t tid);

uint8_t os_task_create( taskproctype taskproc, void *data, uint8_t prio, Msg_t* msgPool, uint8_t poolSize, uint16_t msgSize );

void *task_get_data( void );

TaskState_t task_state_get( uint8_t tid );

bool task_should_run_test(const uint16_t id);

bool task_is_killed(const uint16_t id);

#ifdef __cplusplus
}
#endif


#endif

#include "CppUTestExt/MockSupport_c.h"

#include "os_task.h"

void os_task_init(void)
{
    mock_c()->actualCall("os_task_init");
}

uint8_t highest_prio_ready_task( void )
{
    return (uint8_t)mock_c()->actualCall("highest_prio_ready_task")
                             ->returnIntValueOrDefault(0);
}


uint8_t next_ready_task( void )
{
    return (uint8_t)mock_c()->actualCall("next_ready_task")
                             ->returnIntValueOrDefault(0);
}


void os_task_ready_set( uint8_t tid )
{
    mock_c()->actualCall("os_task_ready_set");
}


void os_task_wait_sem_set( uint8_t tid, Sem_t sem )
{
    mock_c()->actualCall("os_task_wait_sem_set");
}


void os_task_suspend( uint8_t tid )
{
    mock_c()->actualCall("os_task_suspend");
}


void os_task_resume( uint8_t tid )
{
    mock_c()->actualCall("os_task_resume");
}


void os_task_kill( uint8_t tid )
{
    mock_c()->actualCall("os_task_kill")
        ->withIntParameters("tid", tid);
}


uint8_t os_task_prio_get( uint8_t tid )
{
    return (uint8_t)mock_c()->actualCall("os_task_prio_get")
        ->withIntParameters("tid", tid)
        ->returnIntValueOrDefault(0);
}


void os_task_clear_wait_queue( uint8_t tid )
{
    mock_c()->actualCall("os_task_clear_wait_queue")
        ->withIntParameters("tid", tid);
}


void os_task_wait_time_set( const uint8_t tid,
                            const uint8_t id,
                            const uint32_t time )
{
    mock_c()->actualCall("os_task_wait_time_set")
        ->withIntParameters("tid", tid)
        ->withIntParameters("id", tid)
        ->withIntParameters("time", time);
}


void os_task_wait_event( uint8_t tid,
                         Evt_t eventId,
                         bool waitSingleEvent,
                         uint32_t timeout )
{
    mock_c()->actualCall("os_task_wait_event")
        ->withIntParameters("tid", tid)
        ->withIntParameters("eventId", eventId)
        ->withBoolParameters("waitSingleEvent", waitSingleEvent)
        ->withIntParameters("timeout", timeout);
}


void task_tick( const uint8_t clockId, const uint32_t tickSize )
{
    mock_c()->actualCall("task_tick")
        ->withIntParameters("clockId", clockId)
        ->withIntParameters("tickSize", tickSize);
}


void os_task_signal_event( Evt_t eventId )
{
    mock_c()->actualCall("os_task_signal_event")
        ->withIntParameters("eventId", eventId);
}


void task_run( void )
{
    mock_c()->actualCall("task_run");
}


void os_task_run_test( const uint8_t id )
{
    mock_c()->actualCall("os_task_run_test");
}


uint16_t task_internal_state_get( uint8_t tid )
{
    return (uint16_t)mock_c()->actualCall("task_internal_state_get")
                              ->returnIntValueOrDefault(0);
}


void os_task_internal_state_set( const uint8_t tid, const uint16_t state )
{
    mock_c()->actualCall("os_task_internal_state_set");
}


void os_task_release_waiting_task( Sem_t sem )
{
    mock_c()->actualCall("os_task_release_waiting_task");
}


int16_t os_task_waiting_this_semaphore( Sem_t sem )
{
    mock_c()->actualCall("os_task_waiting_this_semaphore");
    return (int16_t)mock_c()->returnIntValueOrDefault(-1);
}


MsgQ_t os_task_msgQ_get( const uint8_t tid )
{
    return (MsgQ_t)mock_c()->actualCall("os_task_msgQ_get")
                            ->returnIntValueOrDefault(NO_QUEUE);
}


void os_task_set_wait_queue(uint8_t tid, MsgQ_t queue)
{
    mock_c()->actualCall("os_task_set_wait_queue");
}


MsgQ_t os_task_get_wait_queue(uint8_t tid)
{
    return (MsgQ_t)mock_c()->actualCall("os_task_get_wait_queue")
                            ->returnIntValueOrDefault(NO_QUEUE);
}


void os_task_set_change_event(uint8_t tid, Evt_t event)
{
    mock_c()->actualCall("os_task_set_change_event");
}


Evt_t os_task_get_change_event(uint8_t tid)
{
    return (Evt_t)mock_c()->actualCall("os_task_get_change_event")
                           ->returnIntValueOrDefault(NO_EVENT);
}


void os_task_set_msg_result(uint8_t tid, uint8_t result)
{
    mock_c()->actualCall("os_task_set_msg_result");
}


uint8_t os_task_get_msg_result(uint8_t tid)
{
    return (uint8_t)mock_c()->actualCall("os_task_get_msg_result")
                             ->returnIntValueOrDefault(0);
}


uint32_t os_task_timeout_get(uint8_t tid)
{
    return (uint32_t)mock_c()->actualCall("os_task_timeout_get")
                              ->returnIntValueOrDefault(0);
}


uint8_t os_task_create( taskproctype taskproc, void *data, uint8_t prio, Msg_t* msgPool, uint8_t poolSize, uint16_t msgSize )
{
    return (uint8_t)mock_c()->actualCall("os_task_create")
                             ->returnIntValueOrDefault(0);
}


void *task_get_data( void )
{
    return (void*)mock_c()->actualCall("task_get_data")
                           ->returnPointerValueOrDefault(NULL);
}


TaskState_t task_state_get( uint8_t tid )
{
    return (TaskState_t)mock_c()->actualCall("task_state_get")
                                ->returnIntValueOrDefault(READY);
}


bool task_should_run_test(const uint16_t id)
{
    return (bool)mock_c()->actualCall("task_should_run_test")
                          ->returnIntValueOrDefault(0);
}


bool task_is_killed(const uint16_t id)
{
    return (bool)mock_c()->actualCall("task_is_killed")
                          ->returnIntValueOrDefault(0);
}


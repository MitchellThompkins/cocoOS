#ifndef OS_MSGQUEUE_H__
#define OS_MSGQUEUE_H__

/** @file os_msgqueue.h Message queue header file*/

#include <stdint.h>

#include "os_defines.h"
#include "os_event.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    uint8_t signal;
    uint8_t reserved;   /* Alignment byte */
    uint8_t pad0;
    uint8_t pad1;
    uint32_t delay;     /* Delay of posting in ticks */
    uint32_t reload;    /* Reload value for periodic messages */
} Msg_t;


typedef uint8_t MsgQ_t;


enum {
    MSG_QUEUE_UNDEF,
    MSG_QUEUE_DEF,
    MSG_QUEUE_EMPTY,
    MSG_QUEUE_FULL,
    MSG_QUEUE_RECEIVED,
    MSG_QUEUE_POSTED
};




#define OS_MSG_Q_POST(task_id, msg, delay, period, async )     do {\
                                                                uint8_t os_posted;\
                                                                MsgQ_t queue;\
                                                                queue = os_msgQ_find(task_id);\
                                                                os_task_set_wait_queue(running_tid, queue);\
                                                                Evt_t event;\
                                                                event = os_msgQ_event_get( queue );\
                                                                os_task_set_change_event(running_tid, event);\
                                                                do {\
                                                                    os_posted = os_msg_post( (Msg_t*)&msg, os_msgQ_find(task_id), delay, period );\
                                                                    if ( os_posted == MSG_QUEUE_FULL ){\
                                                                        if ( async == 0 ) {\
                                                                            os_task_set_msg_result(running_tid, os_posted);\
                                                                            event_wait(event);\
                                                                            os_posted = os_task_get_msg_result(running_tid);\
                                                                            event = os_task_get_change_event(running_tid);\
                                                                        }\
                                                                        else {\
                                                                            os_posted = MSG_QUEUE_UNDEF;\
                                                                        }\
                                                                    }\
                                                                } while ( os_posted == MSG_QUEUE_FULL );\
                                                                if ( MSG_QUEUE_POSTED == os_posted ) {\
                                                                	os_signal_event(event);\
                                                                	os_event_set_signaling_tid( event, running_tid );\
                                                                }\
                                                            } while(0)



#define OS_MSG_Q_RECEIVE(task_id, pMsg, async, cb)     do {\
                                                    uint8_t os_received;\
                                                    MsgQ_t queue;\
                                                    queue = os_msgQ_find(task_id);\
                                                    os_task_set_wait_queue(running_tid, queue);\
                                                    Evt_t event;\
                                                    event = os_msgQ_event_get(queue);\
                                                    os_task_set_change_event(running_tid, event);\
                                                    do {\
                                                        os_received = os_msg_receive((Msg_t*)pMsg, os_msgQ_find(task_id));\
                                                        if ( os_received == MSG_QUEUE_EMPTY ){\
                                                            if ( async == 0 ) {\
       	                                                        os_task_set_msg_result(running_tid, os_received);\
                                                                if (cb) {\
                                                                 ((void (*)())cb)();\
                                                                }\
                                                                event_wait(event);\
                                                                os_received = os_task_get_msg_result(running_tid);\
                                                                event = os_task_get_change_event(running_tid);\
                                                            }\
                                                            else {\
                                                                ((Msg_t*)pMsg)->signal = NO_MSG_ID;\
                                                                os_received = MSG_QUEUE_UNDEF;\
                                                            }\
                                                        }\
                                                    } while ( os_received == MSG_QUEUE_EMPTY );\
                                                    if ( MSG_QUEUE_RECEIVED == os_received) {\
                                                    	os_signal_event(event);\
                                                    	os_event_set_signaling_tid(event, running_tid );\
													}\
                                                } while(0)




void os_msgQ_init( void );
MsgQ_t os_msgQ_create( Msg_t *buffer, uint8_t nMessages, uint16_t msgSize,  uint8_t task_id );
MsgQ_t os_msgQ_find( uint8_t task_id );
//Sem_t os_msgQ_sem_get( MsgQ_t queue );
Evt_t os_msgQ_event_get( MsgQ_t queue );
void os_msgQ_tick( MsgQ_t queue );

uint8_t os_msg_post( Msg_t *msg, MsgQ_t queue, uint32_t delay, uint32_t period );
uint8_t os_msg_receive( Msg_t *msg, MsgQ_t queue );


#ifdef __cplusplus
}
#endif

#endif

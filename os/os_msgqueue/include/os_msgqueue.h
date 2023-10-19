#ifndef OS_MSGQUEUE_H__
#define OS_MSGQUEUE_H__

/** @file os_msgqueue.h Message queue header file*/

#include <stdint.h>

#include "os_defines.h"
#include "os_event.h"

#ifdef __cplusplus
extern "C" {
#endif


//TODO(@mthompkins): Pack this?
typedef struct
{
    uint8_t signal;
    uint8_t reserved; // Alignment byte
    uint8_t pad[2];
    uint32_t delay;   // Delay of posting in ticks
    uint32_t reload;  // Reload value for periodic messages
} Msg_t;


typedef uint8_t MsgQ_t;


//TODO(@mthompkins): Give this a name
enum {
    MSG_QUEUE_UNDEF,
    MSG_QUEUE_DEF,
    MSG_QUEUE_EMPTY,
    MSG_QUEUE_FULL,
    MSG_QUEUE_RECEIVED,
    MSG_QUEUE_POSTED
};



// TODO(mtchompkins) I highly suspect alot of this can be wrapped into a
// function that the macro calls
#define OS_MSG_Q_POST(task_id, msg, delay, period, async )     do {\
                                                                uint8_t os_posted;\
                                                                MsgQ_t queue;\
                                                                queue = os_msgQ_find(task_id);\
                                                                os_task_set_wait_queue(os_get_running_tid(), queue);\
                                                                Evt_t event;\
                                                                event = os_msgQ_event_get( queue );\
                                                                os_task_set_change_event(os_get_running_tid(), event);\
                                                                do {\
                                                                    os_posted = os_msg_post( (Msg_t*)&msg, os_msgQ_find(task_id), delay, period );\
                                                                    if ( os_posted == MSG_QUEUE_FULL ){\
                                                                        if ( async == 0 ) {\
                                                                            os_task_set_msg_result(os_get_running_tid(), os_posted);\
                                                                            event_wait(event);\
                                                                            os_posted = os_task_get_msg_result(os_get_running_tid());\
                                                                            event = os_task_get_change_event(os_get_running_tid());\
                                                                        }\
                                                                        else {\
                                                                            os_posted = MSG_QUEUE_UNDEF;\
                                                                        }\
                                                                    }\
                                                                } while ( os_posted == MSG_QUEUE_FULL );\
                                                                if ( MSG_QUEUE_POSTED == os_posted ) {\
                                                                    os_signal_event(event);\
                                                                    os_event_set_signaling_tid( event, os_get_running_tid() );\
                                                                }\
                                                            } while(0)



#define OS_MSG_Q_RECEIVE(task_id, pMsg, async, cb)  do {\
                                                    uint8_t os_received;\
                                                    MsgQ_t queue;\
                                                    queue = os_msgQ_find(task_id);\
                                                    os_task_set_wait_queue(os_get_running_tid(), queue);\
                                                    Evt_t event;\
                                                    event = os_msgQ_event_get(queue);\
                                                    os_task_set_change_event(os_get_running_tid(), event);\
                                                    do {\
                                                        os_received = os_msg_receive((Msg_t*)pMsg, os_msgQ_find(task_id));\
                                                        if ( os_received == MSG_QUEUE_EMPTY ){\
                                                            if ( async == 0 ) {\
                                                                os_task_set_msg_result(os_get_running_tid(), os_received);\
                                                                if (cb) {\
                                                                 ((void (*)())cb)();\
                                                                }\
                                                                event_wait(event);\
                                                                os_received = os_task_get_msg_result(os_get_running_tid());\
                                                                event = os_task_get_change_event(os_get_running_tid());\
                                                            }\
                                                            else {\
                                                                ((Msg_t*)pMsg)->signal = NO_MSG_ID;\
                                                                os_received = MSG_QUEUE_UNDEF;\
                                                            }\
                                                        }\
                                                    } while ( os_received == MSG_QUEUE_EMPTY );\
                                                    if ( MSG_QUEUE_RECEIVED == os_received) {\
                                                        os_signal_event(event);\
                                                        os_event_set_signaling_tid(event, os_get_running_tid() );\
                                                    }\
                                                } while(0)




/*****************************************************************************/
/*
   @brief
*/
/*****************************************************************************/
void os_msgQ_init( void );


/*****************************************************************************/
/*
   @brief creates a new message queue instance and returns a unique ID for it

   @param buffer - 
   @param nMessages - The number of messages in the queue
   @param msgSize - 
   @param task_id - The task ID to which this message queue belongs

   @return A unique id for this message ID
*/
/*****************************************************************************/
MsgQ_t os_msgQ_create( const Msg_t *const buffer,
                       const uint8_t nMessages,
                       const uint16_t msgSize,
                       const uint8_t task_id );


/*****************************************************************************/
/*
   @brief Find the queue belonging to specified task_id
*/
/*****************************************************************************/
MsgQ_t os_msgQ_find( const uint8_t task_id );


/*****************************************************************************/
/*
   @brief
*/
/*****************************************************************************/
//Sem_t os_msgQ_sem_get( MsgQ_t queue );


/*****************************************************************************/
/*
   @brief
*/
/*****************************************************************************/
Evt_t os_msgQ_event_get( const MsgQ_t queue );


/*****************************************************************************/
/*
   @brief
*/
/*****************************************************************************/
void os_msgQ_tick( const MsgQ_t queue );


/*****************************************************************************/
/*
   @brief
*/
/*****************************************************************************/
uint8_t os_msg_post( Msg_t *msg,
                     const MsgQ_t queue,
                     const uint32_t delay,
                     const uint32_t period );


/*****************************************************************************/
/*
   @brief
*/
/*****************************************************************************/
uint8_t os_msg_receive( Msg_t *msg,
                        const MsgQ_t queue );


#ifdef __cplusplus
}
#endif

#endif

#include "os_defines.h"
#include "os_msgqueue.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    Mem_t *list;            ///< Storage buffer for messages
    uint16_t messageSize;
    uint8_t head;           ///< Queue head
    uint8_t tail;           ///< Queue tail
    uint8_t size;           ///< Total size of queue in number of messages
    uint8_t pad[3];
} OSQueue_t;


typedef struct {
    OSQueue_t q;
    uint8_t taskId;         ///< The task owning this queue
    Evt_t change;           ///< Queue change event
} OSMsgQ_t;


#if( N_QUEUES > 0 )

static uint8_t MsgQAllDelayed( OSQueue_t *q );
static uint8_t queue_push(OSQueue_t *queue, Msg_t *msg );

/* List of task message queues */
static OSMsgQ_t msgQList[ N_QUEUES ];
static MsgQ_t nQueues;
#endif



void os_msgQ_init() {
#if( N_QUEUES > 0 )
    uint8_t i;
    nQueues = 0;
    for ( i = 0; i < N_QUEUES; ++i  ) {
        msgQList[ i ].q.list = 0;
        msgQList[ i ].q.head = 0;
        msgQList[ i ].q.tail = 0;
        msgQList[ i ].q.size = 0;
        msgQList[ i ].q.messageSize = 0;
        msgQList[ i ].taskId = 0;
        msgQList[ i ].change = 0;
    }
#endif
}

MsgQ_t os_msgQ_create( Msg_t *buffer, uint8_t nMessages, uint16_t msgSize, uint8_t task_id ) {
#if( N_QUEUES > 0 )
    os_assert_with_return( nQueues < N_QUEUES, 1 );
    msgQList[ nQueues ].q.list = (Mem_t*)buffer;
    msgQList[ nQueues ].q.head = 1;
    msgQList[ nQueues ].q.tail = 0;
    msgQList[ nQueues ].q.size = nMessages;
    msgQList[ nQueues ].q.messageSize = msgSize;
    msgQList[ nQueues ].taskId = task_id;
    msgQList[ nQueues ].change = event_create();

    nQueues++;
    return nQueues - 1;
#else
    return 0;
#endif
}

// Find the queue belonging to this task_id
MsgQ_t os_msgQ_find(uint8_t task_id) {
#if( N_QUEUES > 0 )
    MsgQ_t queue;

    for (queue = 0; queue < nQueues; queue++) {
        if ( msgQList[ queue ].taskId == task_id ) {
            return queue;
        }
    }
#endif

    return NO_QUEUE;
}

Evt_t os_msgQ_event_get( MsgQ_t queue ) {
#if( N_QUEUES > 0 )
    if ( queue >= nQueues ) {
        return NO_EVENT;
    }
    return msgQList[ queue ].change;
#else
    return NO_EVENT;
#endif
}

uint8_t os_msg_post( Msg_t *msg, MsgQ_t queue, uint32_t delay, uint32_t period ) {
    #if( N_QUEUES > 0 )

    if ( queue >= nQueues ) {
        return MSG_QUEUE_UNDEF;
    }

    msg->delay = delay;
    msg->reload = period;
    return queue_push(&msgQList[ queue ].q, msg);

#else
    return 0;
#endif
}

#if( N_QUEUES > 0 )
static uint8_t queue_push(OSQueue_t *queue, Msg_t *msg ) {

	if ( 0 == queue->size ) return MSG_QUEUE_UNDEF;

	uint8_t head = queue->head;

	if (head == queue->tail) return MSG_QUEUE_FULL;

	uint16_t msgSz = queue->messageSize;

	uint8_t *src = (uint8_t*)msg;
	uint8_t *dst = (uint8_t*)((Mem_t)queue->list + head * msgSz);

	while ( msgSz--) {
        *dst++ = *src++;
    }

	queue->head = (++head) % queue->size;

	return MSG_QUEUE_POSTED;

}
#endif

uint8_t os_msg_receive( Msg_t *msg, MsgQ_t queue ) {
    #if( N_QUEUES > 0 )

    OSQueue_t *q;
    uint8_t *src;
    uint8_t found;

    if ( queue >= nQueues ) {
        return MSG_QUEUE_UNDEF;
    }

    q = &msgQList[ queue ].q;
    uint8_t tail = q->tail;

    if ((tail+1) % q->size == q->head) return MSG_QUEUE_EMPTY;

    /* If all messages have a delay > 0 we consider the queue as empty */
    if ( MsgQAllDelayed( q ) == 1 ) {
        return MSG_QUEUE_EMPTY;
    }

    /* At least one message is ready to be delivered. Find it! */
    found = 0;
    uint16_t msgSz = q->messageSize;

    while ( found == 0 ) {
    	tail = (tail+1) % q->size;
        src = (uint8_t*)( (Mem_t)q->list + tail * msgSz );

        uint8_t *dst = (uint8_t*)msg;
        while (msgSz--) {
        	*dst++ = *src++;
        }

        // restore msgSz
        msgSz = q->messageSize;

        uint8_t messagePeriodic = ( msg->reload > 0 );
        uint8_t messageTimedOut = ( msg->delay == 0 );

        if ( messageTimedOut ){
            found = 1;
            if ( messagePeriodic ) {
                msg->delay = msg->reload;
            }
        }

        /* Put the message back at head position if delay > 0, or if it is a periodic message that timed out */
        if (( !messageTimedOut ) || ( messagePeriodic && messageTimedOut )) {
            dst = (uint8_t*)( (Mem_t)q->list + q->head * msgSz );
            src = (uint8_t*)msg;

            while(msgSz--) {
            	*dst++ = *src++;
            }

            // restore msgSz
            msgSz = q->messageSize;

            /* Look for buffer wrap around */
            q->head = (q->head+1) % q->size;

        }
    }
    q->tail = tail;

    return MSG_QUEUE_RECEIVED;
#else
    return 0;
#endif
}

void os_msgQ_tick( MsgQ_t queue ) {
#if( N_QUEUES > 0 )
    uint8_t nextMessage;
    Msg_t *pMsg;
    OSQueue_t *q = &msgQList[ queue ].q;
    nextMessage = (q->tail+1) % q->size;

    uint8_t head = q->head;
    uint16_t msgSz = q->messageSize;

    while ( nextMessage != head ) {
        pMsg = (Msg_t*)( (Mem_t)q->list + nextMessage * msgSz );

        if ( pMsg->delay > 0 ) {
            --(pMsg->delay);
            if ( pMsg->delay == 0 ) {
                event_ISR_signal( msgQList[ queue ].change );
            }
        }
        nextMessage = (nextMessage + 1) % q->size;

    }
#endif    
}


#if( N_QUEUES > 0 )


static uint8_t MsgQAllDelayed(OSQueue_t *q) {
    uint32_t nextMessage;
    Msg_t *pMsg;
    uint8_t result;
    uint8_t head = q->head;

    uint16_t msgSz = q->messageSize;
    result = 1;
    nextMessage = (q->tail+1) % q->size;

    while ( nextMessage != head ) {
        pMsg = (Msg_t*)( (Mem_t)q->list + (nextMessage * msgSz) );
        if ( pMsg->delay == 0 ) {
            result = 0;
            break;
        }

        nextMessage = (nextMessage + 1) % q->size;
    }

    return result;
}


#endif

#ifdef __cplusplus
}
#endif

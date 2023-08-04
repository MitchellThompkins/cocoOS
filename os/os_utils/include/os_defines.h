#ifndef _os_defs
#define _os_defs

/** @file os_defines.h cocoOS user configuration
          Macros can be defined in this file or as compiler flags e.g. -DN_TASKS=2 -DN_EVENTS=3...
*/

#ifdef __cplusplus
extern "C" {
#endif

/** Max number of used tasks
* @remarks Must be defined. @n Allowed range: 0-254. Value must not be exceeded */
#ifndef N_TASKS
#define N_TASKS 1
#endif


/** Max number of used message queues
* @remarks Must be defined. @n Allowed range: 0-254. Value must not be exceeded */
#ifndef N_QUEUES
#define N_QUEUES 0
#endif


/** Max number of used semaphores
* @remarks Must be defined. @n Allowed range: 0-254. Value must not be exceeded */
#ifndef N_SEMAPHORES
#define N_SEMAPHORES 0
#endif


/** Max number of used events
* @remarks Must be defined. @n Allowed range: 0-254. Value must not be exceeded */
#ifndef N_EVENTS
#define N_EVENTS 0
#endif


/** Round Robin scheduling
* @remarks If defined, tasks will be scheduled ignoring the priorities */
#ifndef ROUND_ROBIN
#define ROUND_ROBIN 0
#endif


/** Memory size
 * @remarks Should be set to the size of address pointer */
#ifndef Mem_t
#define Mem_t uint32_t
#endif

#define NO_MSG_ID   0xff
#define ISR_TID     0xfe

/* Total number of semaphores needed */
#define N_TOTAL_SEMAPHORES    ( N_SEMAPHORES + N_QUEUES )


/* Total number of events needed */
#define N_TOTAL_EVENTS        ( N_EVENTS + N_QUEUES )

#ifdef __cplusplus
}
#endif

#endif

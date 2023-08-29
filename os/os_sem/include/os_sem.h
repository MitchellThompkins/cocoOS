#ifndef OS_SEM_H
#define OS_SEM_H

/** @file os_sem.h Semaphore header file*/

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SEM_OFS1 20000
#define SEM_OFS2 21000

#define OS_WAIT_SEM(sem)        do {\
                                    if ( os_sem_larger_than_zero( sem )  ){\
                                        os_sem_decrement( sem );\
                                    }\
                                    else{\
                                        os_task_wait_sem_set( os_get_running_tid(), sem );\
                                        OS_YIELD;\
                                    }\
                                } while (0)


#define OS_SIGNAL_SEM(sem)      do {\
                                    if ( os_task_waiting_this_semaphore( sem ) == 0 ) {\
                                        os_sem_increment( sem );\
                                    }\
                                    else {\
                                        os_task_release_waiting_task( sem );\
                                        OS_YIELD;\
                                    }\
                                } while (0)


#define OS_SIGNAL_SEM_NO_SCHEDULE(sem)  do {\
                                            if ( os_task_waiting_this_semaphore( sem ) == 0 ) {\
                                                os_sem_increment( sem );\
                                            }\
                                            else {\
                                                os_task_release_waiting_task( sem );\
                                            }\
                                        } while (0)




typedef int16_t Sem_t;

void os_sem_init(void);
uint8_t os_sem_larger_than_zero( Sem_t sem );
void os_sem_decrement( Sem_t sem );
void os_sem_increment( Sem_t sem );

Sem_t sem_bin_create( uint8_t initial );
Sem_t sem_counting_create( uint8_t max, uint8_t initial );

#ifdef __cplusplus
}
#endif

#endif

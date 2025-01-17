#ifndef OS_SEM_H
#define OS_SEM_H

/** @file os_sem.h Semaphore header file*/

#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OS_WAIT_SEM(sem)  do {\
                              if ( os_sem_larger_than_zero( sem )  ){\
                                  os_sem_decrement( sem );\
                              }\
                              else{\
                                  os_task_wait_sem_set( os_get_running_tid(), sem );\
                                  OS_YIELD;\
                              }\
                          } while (0)


#define OS_SIGNAL_SEM(sem)  do {\
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


/*****************************************************************************/
/*
   @brief initialize number of semaphores to 0

   @post the internal count of the number of semaphores will be set to 0

   @return void
*/
/*****************************************************************************/
void os_sem_init(void);


/*****************************************************************************/
/*
   @brief Determine whether the specified semahpore has a value larger than
   zero

   @pre sem is a sem which was created with os_sem_create

   @param sem - The id of the desired semaphore

   @return whether the specified semaphore is greater than or equal to zero
*/
/*****************************************************************************/
bool os_sem_larger_than_zero( Sem_t sem );


/*****************************************************************************/
/*
   @brief Get the value of the specified semaphore

   @pre sem is a sem which was created with os_sem_create

   @param sem - The id of the desired semaphore

   @return the value of the desired semaphore
*/
/*****************************************************************************/
Sem_t os_sem_value_get( Sem_t sem );


/*****************************************************************************/
/*
   @brief Get the number of created semaphores

   @return the number of created semaphores
*/
/*****************************************************************************/
uint8_t os_sem_num_sems_get( void );


/*****************************************************************************/
/*
   @brief Decrement the value of the specified semaphore

   @pre sem is a sem which was created with os_sem_create

   @post the specified semaphore is decremented by 1

   @param sem - The id of the desired semaphore
*/
/*****************************************************************************/
void os_sem_decrement( Sem_t sem );


/*****************************************************************************/
/*
   @brief Increment the value of the specified semaphore

   @pre sem is a sem which was created with os_sem_create

   @post the specified semaphore is incremented by 1

   @param sem - The id of the desired semaphore
*/
/*****************************************************************************/
void os_sem_increment( Sem_t sem );


/*****************************************************************************/
/*
   @brief Creates and initializes a new binary semaphore.

   @param initial - value of the semaphore

   @post A semaphore with a unique will be created

   @return Returns the created semaphore.

   @remarks \b Usage: @n A semaphore is created by declaring a variable of
   type Sem_t and then assigning the sem_bin_create(value) return value to
   that variable.

   @code
   Sem_t mySem;
   mySem = sem_bin_create(0);

   @endcode

*/
/*****************************************************************************/
Sem_t sem_bin_create( bool initial );


/*********************************************************************************/
/*
  @brief Creates and initializes a new counting semaphore.

  @param max value of the semaphore

  @param initial value of the semaphore

  @return Returns the created semaphore.

  @remarks \b Usage: @n A semaphore is created by declaring a variable of type Sem_t
  and then assigning the sem_create(max, 0) return value to that variable.

  @code
  Sem_t mySem;
  mySem = sem_counting_create(5,0);

  @endcode

*/
/*********************************************************************************/
Sem_t sem_counting_create( uint8_t max, uint8_t initial );

#ifdef __cplusplus
}
#endif

#endif

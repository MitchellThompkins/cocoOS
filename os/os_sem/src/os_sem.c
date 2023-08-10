#include "os_sem.h"
#include "os_defines.h"

static Sem_t os_sem_create( uint8_t max, uint8_t initial );

#if ( N_TOTAL_SEMAPHORES > 0 )
static SemValue_t semList[ N_TOTAL_SEMAPHORES ];
static Sem_t nSemaphores;
#endif

void os_sem_init(void)
{
#if ( N_TOTAL_SEMAPHORES > 0 )
    nSemaphores = 0;
#endif
}


static Sem_t os_sem_create( uint8_t max, uint8_t initial ) {
#if ( N_TOTAL_SEMAPHORES > 0 )
    os_assert( nSemaphores < N_TOTAL_SEMAPHORES );

    /* Initialize the value and the waiting list */
    semList[ nSemaphores ].maxValue = max;
    semList[ nSemaphores ].value = initial;
    ++nSemaphores;

    return ( nSemaphores - 1 );
#else
    return 0;
#endif
}


/*********************************************************************************/
/*  Sem_t sem_bin_create(uint8_t initial)                                              *//**
*
*   Creates and initializes a new binary semaphore.
*
*   @param initial value of the semaphore
*   @return Returns the created semaphore.
*   @remarks \b Usage: @n A semaphore is created by declaring a variable of type Sem_t
*   and then assigning the sem_bin_create(value) return value to that variable.
*
*   @code
*   Sem_t mySem;
*   mySem = sem_bin_create(0);
*
*   @endcode
*
* */
/*********************************************************************************/
Sem_t sem_bin_create( uint8_t initial ) {
    initial = initial > 0 ? 1 : 0;
    return os_sem_create( 1, initial );
}


/*********************************************************************************/
/*  Sem_t sem_counting_create(uint8_t max, uint8_t initial)                                              *//**
*
*   Creates and initializes a new counting semaphore.
*
*   @param max value of the semaphore
*   @param initial value of the semaphore
*   @return Returns the created semaphore.
*   @remarks \b Usage: @n A semaphore is created by declaring a variable of type Sem_t
*   and then assigning the sem_create(max, 0) return value to that variable.
*
*   @code
*   Sem_t mySem;
*   mySem = sem_counting_create(5,0);
*
*   @endcode
*
* */
/*********************************************************************************/
Sem_t sem_counting_create( uint8_t max, uint8_t initial ) {
    return os_sem_create( max, initial );
}


uint8_t os_sem_larger_than_zero( Sem_t sem) {
#if ( N_TOTAL_SEMAPHORES > 0 )
    return ( semList[ sem ].value > 0 );
#else
    return 0;
#endif
}


void os_sem_decrement( Sem_t sem ) {
#if ( N_TOTAL_SEMAPHORES > 0 )
    if ( semList[ sem ].value > 0 ) {
        --semList[ sem ].value;
    }
#endif
}


void os_sem_increment( Sem_t sem ) {
#if ( N_TOTAL_SEMAPHORES > 0 )
    if ( semList[ sem ].value < semList[ sem ].maxValue ) {
        ++semList[ sem ].value;
    }
#endif
}




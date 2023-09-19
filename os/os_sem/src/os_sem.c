#include "os_assert.h"
#include "os_defines.h"
#include "os_sem.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t maxValue;
    uint8_t value;
} SemValue_t;

#if ( N_TOTAL_SEMAPHORES > 0 )
static SemValue_t semList[ N_TOTAL_SEMAPHORES ];
static uint8_t nSemaphores;
#endif

static Sem_t os_sem_create( uint8_t max, uint8_t initial );


void os_sem_init(void)
{
#if ( N_TOTAL_SEMAPHORES > 0 )
    nSemaphores = 0;
#endif
}


static Sem_t os_sem_create( uint8_t max, uint8_t initial )
{
#if ( N_TOTAL_SEMAPHORES > 0 )
    os_assert_with_return( nSemaphores < N_TOTAL_SEMAPHORES, 1 );

    // Initialize the value and the waiting list
    semList[ nSemaphores ].maxValue = max;
    semList[ nSemaphores ].value = initial;
    ++nSemaphores;

    return (Sem_t) ( nSemaphores - 1 );
#else
    return (Sem_t) -1;
#endif
}


Sem_t sem_bin_create( bool initial )
{
    return os_sem_create( (uint8_t) true, (uint8_t) initial );
}


Sem_t sem_counting_create( uint8_t max, uint8_t initial )
{
    return os_sem_create( max, initial );
}


bool os_sem_larger_than_zero( Sem_t sem )
{
#if ( N_TOTAL_SEMAPHORES > 0 )
    return ( semList[ sem ].value > 0 );
#else
    return 0;
#endif
}


Sem_t os_sem_value_get( Sem_t sem )
{
#if ( N_TOTAL_SEMAPHORES > 0 )
    return ( semList[ sem ].value );
#else
    return 0;
#endif
}


uint8_t os_sem_num_sems_get( void )
{
    return nSemaphores;
}


void os_sem_decrement( Sem_t sem )
{
#if ( N_TOTAL_SEMAPHORES > 0 )
    if ( semList[ sem ].value > 0 )
    {
        --semList[ sem ].value;
    }
#endif
}


void os_sem_increment( Sem_t sem )
{
#if ( N_TOTAL_SEMAPHORES > 0 )
    if ( semList[ sem ].value < semList[ sem ].maxValue )
    {
        ++semList[ sem ].value;
    }
#endif
}

#ifdef __cplusplus
}
#endif

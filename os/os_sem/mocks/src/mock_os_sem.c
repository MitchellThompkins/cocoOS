#include "CppUTestExt/MockSupport_c.h"

#include "os_sem.h"
#include "os_defines.h"

#include "stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t maxValue;
    uint8_t value;
} SemValue_t;

static SemValue_t semList[ N_TOTAL_SEMAPHORES ];
static Sem_t nSemaphores;


void os_sem_init(void)
{
    mock_c()->actualCall("os_sem_init");
}

bool os_sem_larger_than_zero( Sem_t sem )
{
    mock_c()->actualCall("os_sem_larger_than_zero");
}

void os_sem_decrement( Sem_t sem )
{
    mock_c()->actualCall("os_sem_decrement");
}

void os_sem_increment( Sem_t sem )
{
    mock_c()->actualCall("os_sem_increment");
}

Sem_t sem_bin_create( bool initial )
{
    mock_c()->actualCall("sem_bin_create");
}

Sem_t sem_counting_create( uint8_t max, uint8_t initial )
{
    int v = (int)mock_c()->getData("sem_return_value").value.intValue;
    mock_c()->actualCall("sem_counting_create")->returnIntValueOrDefault(v);
}

#ifdef __cplusplus
}
#endif

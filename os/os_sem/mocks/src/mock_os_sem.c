#include "CppUTestExt/MockSupport_c.h"

#include "cocoos.h"
#include "os_kernel.h"
#include "os_event.h"
#include "os_msgqueue.h"
#include "os_port.h"
#include "os_sem.h"
#include "os_task.h"

static SemValue_t semList[ N_TOTAL_SEMAPHORES ];
static Sem_t nSemaphores;


void os_sem_init(void)
{
    mock_c()->actualCall("os_sem_init");
}

uint8_t os_sem_larger_than_zero( Sem_t sem )
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

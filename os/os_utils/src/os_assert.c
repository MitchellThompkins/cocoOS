#include <stddef.h>
#include <stdint.h>

#include "os_port.h"

#ifdef __cplusplus
extern "C" {
#endif

static void (*user_callback)(const char*, uint16_t, const char*) = (void*) NULL;

void os_on_assert(const char* file, uint16_t line, const char* expr)
{
    static volatile uint16_t l;
    os_disable_interrupts();
    l = line;
    l = l;

    if (NULL != user_callback)
    {
        user_callback(file, line, expr);
    }

    while(1);
}

void os_on_assert_attach_callback(void (*callback)(const char*, uint16_t, const char*))
{
    user_callback = callback;
}

#ifdef __cplusplus
}
#endif

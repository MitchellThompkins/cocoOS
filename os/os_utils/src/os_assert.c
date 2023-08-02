#include <stdint.h>

#include "os_port.h"

static void (*user_callback)(const char*, uint16_t, const char*) = (void*)0;

void os_on_assert(const char* file, uint16_t line, const char* expr) {
    static volatile uint16_t l;
    os_disable_interrupts();
    l = line;
    l = l;

    if (0 != user_callback) {
        user_callback(file, line, expr);
    }

    while(1);
}

void os_on_assert_attach_callback(void (*callback)(const char*, uint16_t, const char*)) {
    user_callback = callback;
}


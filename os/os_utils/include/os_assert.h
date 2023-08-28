#ifndef _os_assert_h__
#define _os_assert_h__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void os_on_assert( const char* filename, uint16_t line, const char* expression);
void os_on_assert_attach_callback( void (*callback)(const char*, uint16_t, const char*) );

#ifndef NASSERT
// The `return` statement is never hit, however for unit testing
// it is a conveinent way to break out of the function to avoid
// performing undefined behavior.
#define os_assert( test )\
    if ( !(test) ) {os_on_assert(__FILE__, __LINE__, #test); return;}

#define os_assert_with_return( test , rtn)\
    if ( !(test) ) {os_on_assert(__FILE__, __LINE__, #test); return rtn;}
#else
#define os_assert( test )
#endif

#ifdef __cplusplus
}
#endif
#endif

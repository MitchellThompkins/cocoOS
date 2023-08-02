#ifndef _os_assert_h__
#define _os_assert_h__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void os_on_assert( const char* filename, uint16_t line, const char* expression);
void os_on_assert_attach_callback( void (*callback)(const char*, uint16_t, const char*) );

#ifndef NASSERT
#define os_assert( test )   if ( !(test) ) {os_on_assert(__FILE__, __LINE__, #test);}
#else
#define os_assert( test )
#endif

#ifdef __cplusplus
}
#endif
#endif

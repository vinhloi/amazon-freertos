#include <assert.h>
#include "FreeRTOS.h"

/* Messages by __assert_func() and __assert() are output to stderr 
 * but this functionality is not implemented. */

void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
    (void)file;
    (void)line;
    (void)func;
    (void)failedexpr;

    vAssertCalled();

    for(;;){}
}

void __assert(const char *file, int line, const char *failedexpr)
{
    (void)file;
    (void)line;
    (void)failedexpr;

    vAssertCalled();

    for(;;){}
}

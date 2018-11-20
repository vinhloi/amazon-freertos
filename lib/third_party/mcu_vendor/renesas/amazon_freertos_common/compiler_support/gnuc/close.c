#include <sys/unistd.h>

/* Both sscanf() and fscanf() use __sinit() of newlib/libc/stdio/findfp.c internally 
 * and the __sinit() requires the address of close(). Because of this, close() is 
 * necessary for not only fscanf() but also sscanf(). */

int close(int fd)
{
    (void)fd;

    return -1;
}

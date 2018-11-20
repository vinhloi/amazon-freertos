#include <sys/unistd.h>

/* Both sscanf() and fscanf() use __sinit() of newlib/libc/stdio/findfp.c internally 
 * and the __sinit() requires the address of lseek(). Because of this, lseek() is 
 * necessary for not only fscanf() but also sscanf(). */

_off_t lseek(int fd, _off_t pos, int whence)
{
    (void)fd;
    (void)pos;
    (void)whence;

    return (_off_t) -1;
}

#include <sys/unistd.h>

/* Both sscanf() and fscanf() use __sinit() of newlib/libc/stdio/findfp.c internally 
 * and the __sinit() requires the address of write(). Because of this, write() is 
 * necessary for not only fscanf() but also sscanf(). */

int write(int fd, _CONST _PTR buf, size_t cnt)
{
    (void)fd;
    (void)buf;
    (void)cnt;

    return -1;
}

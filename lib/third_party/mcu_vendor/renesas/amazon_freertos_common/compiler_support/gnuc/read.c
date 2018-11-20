#include <sys/unistd.h>

/* Both sscanf() and fscanf() use __sinit() of newlib/libc/stdio/findfp.c internally 
 * and the __sinit() requires the address of read(). Because of this, read() is 
 * necessary for not only fscanf() but also sscanf(). */

int read(int fd, _PTR buf, size_t cnt)
{
    (void)fd;
    (void)buf;
    (void)cnt;

    return -1;
}

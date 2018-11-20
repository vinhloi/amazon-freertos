#include <stdio.h>

/* Messages by fprintf() are output to stdout, stderr, file, etc 
 * but these functionalities are not implemented. */

int fprintf(FILE *__restrict fp, const char *__restrict fmt, ...)
{
    (void)fp;
    (void)fmt;

    return EOF;
}

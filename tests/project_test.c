#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void
report_error (const char *msg,
              const char *file,
              int         line,
              const char *func_name,
              bool        require)
{
    printf("%s:%d:%s: (FAIL)\n", file, line, func_name);
    if (require)
    {
        exit(-1);
    }
}
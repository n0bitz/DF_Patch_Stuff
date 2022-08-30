#include "bg_lib.h"

int sprintf(char *buf, const char *fmt, ...)
{
    int ret;
    va_list argptr;

    va_start(argptr, fmt);
    ret = vsprintf(buf, fmt, argptr);
    va_end(argptr);
    return ret;
}

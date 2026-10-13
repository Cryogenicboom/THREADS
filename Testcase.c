#include <stdlib.h>
#include "xthreads.h"

int main()
{
    xthread_init();
    xthread_self();
    return 0;
}
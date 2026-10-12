#include "stdio.h"
#include <stdlib.h>
#include <ucontext.h>
#include "xthreads.h"
#include <string.h>


xthread_t* current;
xthread_t* head;
xthread_t* tail;

unsigned id_counter = 1;

void xthread_init()
{
    xthread_t main;
    main.state = RUNNING;
    main.id = id_counter;
    id_counter++;

    // main do not need stack, OS provides it
    main.stack_ptr = NULL;
    main.stack_size = 0;

    current = &main;
    head = &main;   
}

void xthread_self()
{
    printf("Id: %u, Status: %d\n", current->id, current->state);
}
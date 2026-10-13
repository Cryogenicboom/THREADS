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
    static xthread_t main;          // Error 1 note in diary
    main.state = RUNNING;
    main.id = id_counter;
    id_counter++;

    // main do not need stack, OS provides it
    main.stack_ptr = NULL;
    main.stack_size = 0;

    current = &main;
    head = &main;   
}

void xthread_create(xthread_t *thread, void* (*func)(void *), void* arg, int allot_stack_size)
{
    thread->id = id_counter;
    if(allot_stack_size == 0)
    {
        thread->stack_size = 64*1024; // default stack size
    }
    else 
    {
        thread->stack_size = allot_stack_size;
    }

    thread->stack_ptr = (void *)malloc(sizeof(thread->stack_size));
    if(thread->stack_ptr == NULL)
    {
        printf("malloc failed, stack_ptr can't be allot.\n");
        free(thread->stack_ptr);
        return;
    }
    thread->state = READY;
    thread->function = func;    // which function this thread refers

    getcontext(&thread->ctx);
    thread->ctx.uc_link = thread->function;
    thread->ctx.uc_stack.ss_size = allot_stack_size;
    thread->ctx.uc_stack.ss_sp = thread->stack_ptr;

    makecontext(&thread->ctx, thread->function, 0);
}

void xthread_self()
{
    if (current == NULL) {
            printf("xthread has not been initialized.\n");
            return;
        }

    printf("Id: %u, Status: %d\n", current->id, current->state);
}
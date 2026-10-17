#ifndef XTHREADS_H
#define XTHREADS_H

#include <ucontext.h>
#include <stddef.h>

typedef enum xthread_state{
       READY, RUNNING, BLOCKED, FINISHED
}xthread_state;

typedef struct xthread_t
{
       unsigned id;
       xthread_state state;

       void* stack_ptr;
       size_t stack_size;

       ucontext_t ctx;

       void *(*function)(void *);
       void *arg;
       void *retval;

       struct xthread_t* join_strand;
       struct xthread_t* next;
}xthread_t;

// void xthread_self();
void xthread_init();
int xthread_create(xthread_t **thread, void* (*func)(void *), void* arg, size_t stack_size);
#endif
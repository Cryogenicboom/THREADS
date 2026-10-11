#ifndef UTHREADS_H
#define UTHREADS_H

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

       struct xthread_t * join_strand;

}xthread_t;


#endif
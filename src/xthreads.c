#include "stdio.h"
#include <stdlib.h>
#include <ucontext.h>
#include "xthreads.h"
#include <string.h>

xthread_t* current;
xthread_t* head;
xthread_t* tail;


unsigned id_counter = 1;

// RULE : TAIL is only NULL WHEN ---> HEAD is NULL

int add_qback(xthread_t* thread)
{   
    xthread_t* temp;

    if(thread == NULL)
    {
        printf("add_qback fail : Null Thread Argument passed\n");
        return 1;
    }
    else if(tail == NULL && head == NULL)
    {
        head = thread;
        tail = thread;
        tail->next =NULL;
        return 0; 
    }
    else
    {
        // store the new node's address in temp
        temp = thread;
        // join old tail node to new node
        tail->next = temp;
        // set tail to new node
        tail = temp;
        // clear temp and set new_node's next to null, ez
        temp = NULL;
        tail->next = NULL;

        return 0;
    }
}

xthread_t* remove_qfront()
{
    xthread_t* temp;

    if(head != NULL)
    {
        if(head->next == NULL)
        {
            temp = head;
            head = head->next;
            tail = head;
            return temp;
        }
        else
        {
            temp = head;
            head = head->next;
            return temp;
        }
    }
    else
    {
        tail = head;
        return NULL;
    }
}

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
    head = NULL; 
    tail = NULL;
}

int xthread_create(xthread_t **thread, void* (*func)(void *), void* arg, size_t stack_size)
{
    xthread_t* local_thread = malloc(sizeof(xthread_t));
    if(local_thread == NULL)
        return -1;

    if(stack_size == 0)
    {
        local_thread->stack_size = 64*1024; // default stack size
    }
    else 
    {
        local_thread->stack_size = stack_size;
    }
    local_thread->stack_ptr = (void *)malloc(sizeof(local_thread->stack_size));
    if(local_thread->stack_ptr == NULL)
    {
        printf("malloc failed, stack_ptr can't be allot.\n");
        free(local_thread->stack_ptr);
        return;
    }

    local_thread->id = id_counter;

    local_thread->state = READY;
    
    local_thread->function = func;    // which function this thread refers

    local_thread->arg = arg;

    local_thread->next = NULL;

    getcontext(&local_thread->ctx);

    local_thread->ctx.uc_link = NULL;
    local_thread->ctx.uc_stack.ss_size = stack_size;
    local_thread->ctx.uc_stack.ss_sp = local_thread->stack_ptr;

    if(add_qback(local_thread) != 0)
    {
        free(local_thread->stack_ptr);
        free(local_thread);
        return -1;
    }
    
    id_counter++;
    *thread = local_thread;         
    return 0;
}

void xthread_self()
{
    if (current == NULL) {
            printf("xthread has not been initialized.\n");
            return;
        }

    printf("Id: %u, Status: %d\n", current->id, current->state);
}
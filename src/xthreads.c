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
    if(tail == NULL)
    {
        return NULL;
    }
    else if(head == tail && tail != NULL)
    {
        temp = head;
        head = head->next;
        if(head == NULL)
        {
            tail = head;
            return temp;
        }
        return temp;
    }
    else
    {
        temp = head;
        head = head->next;
        return temp;
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

void xthread_create(xthread_t *thread, void* (*func)(void *), void* arg, int allot_stack_size)
{
    thread->id = id_counter;
    id_counter++;

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

    thread->next = NULL;
    getcontext(&thread->ctx);
    thread->ctx.uc_link = NULL;
    thread->ctx.uc_stack.ss_size = allot_stack_size;
    thread->ctx.uc_stack.ss_sp = thread->stack_ptr;

}

void xthread_self()
{
    if (current == NULL) {
            printf("xthread has not been initialized.\n");
            return;
        }

    printf("Id: %u, Status: %d\n", current->id, current->state);
}
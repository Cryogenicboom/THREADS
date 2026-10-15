// tests/t_create.c
#include <stdio.h>
#include <string.h>
#include "xthreads.h"

extern xthread_t *head;
extern xthread_t *tail;

static void *dummy(void *a) 
{ 
    (void)a; 
    return NULL; 
}

static int queue_len(void)
{
    int n = 0;
    for (xthread_t *p = head; p != NULL; p = p->next) n++;
    return n;
}

#define CHECK(cond, msg) printf("%s: %s\n", (cond) ? "PASS" : "FAIL", msg)

int main(void)
{
    xthread_init();

    xthread_t *t1, *t2, *t3;
    CHECK(xthread_create(&t1, dummy, NULL, 0)    == 0, "create t1 (default stack)");
    CHECK(xthread_create(&t2, dummy, NULL, 8192) == 0, "create t2 (8 KB stack)");
    CHECK(xthread_create(&t3, dummy, NULL, 0)    == 0, "create t3 (default stack)");

    CHECK(queue_len() == 3, "queue holds 3 threads");
    CHECK(head == t1 && tail == t3, "order is t1 .. t3 (FIFO)");
    CHECK(tail->next == NULL, "tail->next is NULL");

    // walk the queue and print what is in it
    for (xthread_t *p = head; p != NULL; p = p->next)
        printf("  id=%u state=%d stack_size=%zu\n", p->id, p->state, p->stack_size);

    CHECK(t1->state == READY && t2->state == READY && t3->state == READY, "all READY");
    CHECK(t1->stack_size == 64 * 1024, "default stack is 64 KB");

    // stack-size test: touches every byte. With a wrong malloc this overflows,
    // and AddressSanitizer will name the line
    memset(t1->stack_ptr, 0, t1->stack_size);
    memset(t2->stack_ptr, 0, t2->stack_size);
    puts("PASS: memset over full stacks did not overflow");

    // TODO 1: check that t1, t2, t3 have three different ids
    // TODO 2: check that retval, join_strand are NULL for all three
    return 0;
}
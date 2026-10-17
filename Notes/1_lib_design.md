## xthread Design

- failure returns -1, success returns 0 / 1. 

#### 1. xthread_create()
```C
    int xthread_create
    (
        xthread_t **thread, void* (*func)(void *), void* arg, int stack_size
    );
```
- On successful returns '0' else returns '-1'. 
- if malloc fails to allocate stack, returns -1 with error msg.
- stack size is allocated by size of function, if stack size is passed 0, then default 64KB is allocated. 

#### 2. xthread_yield()
- A running thread calls another thread to run. Thread which calls `yield` goes back in queue. 
- Only a running thread can call this function. Blocked or Ready threads cannot.
```C
    void xthread_yield();
```

#### 3. xthread_exit()
```C
    void xthread_exit(void *retval);
```
- retval is something the exiting thread gives to the library, so the library can store it in the TCB.
- retval is used by other threads calling xthread_join().

#### 4. xthread_join()
```C
    int xthread_join(xthread_t thread_observed, void **retval);
```
- used by another thread to wait for the target thread.
- target thread could be running, blocked, or ready. 
- retval is an output, after return the target's result is written into `*retval`
- join on finished thread immediatly returns, joining the same thread twice is undefined behavior, it is caller's responsibility. 

#### 5. xthread_self()
```C
    xthread_t xthread_self(void); 
```
- returns currently running thread's data.

#### 6. xthread_init()
- adapts a running function as thread. used to assign main() as a thread. 
- Needed to call one time in the program, to let main() be a thread.

<br>

![Thread State Diagram](meda/state_dia.png)

<br>

## Thread Control Block (TCB)

#### Sates : 
1. Running : when the thread is actually being executed. 
2. Ready : when the thread is waiting in queue to run
3. Blocked : waiting for its join target to finish
4. Finished : done running, retval is stored in the TCB until a joiner collects it.

- If Thread-A has called join() on Thread-B then Thread A is called 'joiner' and Thread-B is called 'target'. 
- it is joiner's responsibility to free the stack and TCB of target thread once target thread is finished. 
- Before erasing the target, joiner should first save the retval of target. 

- A  thread cannot free it's stack itself since the active threads needs stack for deallocation too.

## Trampoline Function 
- A trampoline is a small helper function whose only job is to bounce execution into another function, adapting for a mismatch between what the caller can provide and what the target needs.
- makecontext starts a function with no parameters and value to return. Meaning - makecontext only works on functions that have no return value and takes no parameters. 
- Our functions are shaped like this : `void *func(void *arg)`. Don't you want arguments for your function ? Yes we want, but makecontext doesnt want. So trampoline fixes that. 
<br>

| `void *` |	a pointer to some data |
| `void (*func)()`	| a pointer to a function returning void |
| `void *(*func)(void *)` |	a pointer to a function taking a void * and returning a void * |

- A trampoline started by makecontext has no such caller. Nothing on that fresh stack says “go back to the scheduler”. The fallback is uc_link = NULL, so when the trampoline reaches its closing brakcet `}`, the process exits. Every thread dies, including main.
- So the trampoline must never reach its closing brace. Its last act must be to hand control somewhere else, permanently.

## The big picture 
<br>

![Execution_flow](meda/Big_picture.png)

## Difference in `xthreads` and `ucontext`
- when writing the code, i would often get confused on what am i doing exactly? the answer was simple but being a dumbo, it takes me time to understand. 
- `xthreads` is a library while `ucontext` is an API library. 
- thin of it as, ucontext is the lowest api that runs the assembly code and works with the threads on CPU. 
- while our cute xthread library manages the threads. It takes orders from human on which thread to execute, structure of a single thread, order of execution. `Like Engineers doing management roles in corporate`.


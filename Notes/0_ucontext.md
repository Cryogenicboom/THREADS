## what is a thread? 
separate execution units sharing same address space and work on same data, that's logical defination. Practical defination is - thread is a combination of "stack" + "saved set of registers". When switching threads, we save one set of regs and load another set.

## Calling Convention x86-64
[AMD64 ABI (Application Binary Interface)](https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf) is a set of rules that defines how **compiled machine code** communicates, manages memory, and passes data on 64-bit x86-64 systems
<br><br>
Important registers for uthread:<br>
rsp : stack pointer <br>
rip : instruction address / execution location <br>
rbp, rbx, r12-r15 : callee-saved registers <br>

- names of the registers in x86-64 : rax **rbx** rcx rdx rsi rdi **rbp rsp** r8 r9 r10 r11 **r12 r13 r14 r15**.
- Important to the project : rsp rip rbp rbx r12 r13 r14 r15
<br>

**rip** - Instruction pointer
> remembers the address to line of execution for each thread. Tracks the program's line of execution.
<br>

**rsp** - Stack pointer
> points to current stack position. The 'stack' grows toward the lower addresses. Each thread gets its own stack. 
<br>

**Stack Frame** 
> A single thread can have multiple functions, each function gets a space allocated in stack. 
<br>

```C
    main()
    {
        Line 1 of code
        Line 2 of code
        foo()               <-- rip
        Line 3 of code
    }
```
- here `main()` is a caller-function and `foo()` is calle-function. When foo() is called, there need to be some information about main() to be saved, like it's return address in `%rip` register. 

- Some registers are caller-saved: if main() cares about its contents, main() must save them before calling foo().
- Other registers are callee-saved: foo() must preserve them for main(). rbx, rbp, and r12-r15 are callee-saved

- **The Calling Convention** below shows how does a compiled machine code where are `a` and `b`. They know it by a rule, for any function 
```C
    int add(int a, int b)
    {
        return a + b;
    }
```


```text
AMD ABI Convention

first integer argument  → rdi
second integer argument→ rsi
third integer argument → rdx
fourth integer argument → rcx
fifth integer argument  → r8
sixth integer argument  → r9
```

#### Context Switching
Context Switching in threads means saving and restoring the the `rsp` register + Callee's saved registers. 
<br>

#### Kernal Threads 
- scheduler in the kernel handles the kernel stack. They can be easily blocked for I/O by the kernel.
- There can be multiple kernel stack but it could lead to use of significant amount of memory. Also switching between multiple stack again and again could lead to cache misses. 
- Another method could be threads sharing a single kernel stack. the `%rsp` must be reset at every context switch. 

#### User threads
- created at userspace and give less overhead of creating and destroying. 
- User level threads have 3 types of models: 
> 1. 1:n Model - all user level thread are directly mapped to a single kernel thread. 
> 2. 1:1 Model - one user thread to one kernel, directly mapped. 
> 3. m:n Model (m > n) - for m number of user threads there are n number of kernel threads.

- We using 1:n model, since we are assuming our threads to be run on single core system.

## ucontext.h 
- the ucontext.h library provides functions for user level threads and context switching within a single process.
<br>

#### ucontext_t ( User Context Type )
- a struct that helps to store several information about a single thread, it has following parts:  
1. `uc_mcontext` is machine-specific field inside the ucontext_t structure that saves the CPU registers and processor state for a thread.
2. `uc_stack` : which memory region is this context's stack (pointer + size).
3. `uc_link` : which context to resume when this one's function returns.
4. `uc_sigmask` : which signals are blocked.

#### getcontext(&ctx)
- ctx == context.
- stores the context of calling thread into a structure. 
```C
SYNOPSIS         
        #include <ucontext.h>
        int getcontext(ucontext_t *ucp);
```

#### setcontext(&ctx)
- Restores the context stored in the registers.
- it never returns since the CPU carries on the execution from saved `%rip`. 

```C
        int setcontext(const ucontext_t *ucp);
```

#### makecontext(&ctx, function, num_of_int_args)
- makecontext creates a entirely new context for a code that has never ran. 
- it is used to create context for code before code has ran. it's rip is set to starting of the function it is made for. 
- This helps to have a entire new stack for a function, otherwise using `getcontext` will make 2 functions share one stack. 
- Order to use makecontext : 
```md
> getcontext(&ctx) first, just to initialize the struct.
> Allocate a stack yourself and set ctx.uc_stack.ss_sp and ctx.uc_stack.ss_size.
> Set ctx.uc_link to a context to resume when the function returns.
> makecontext(&ctx, func(parameters), 0). The 0 is the number of integer arguments.
> Only integer arguments can be passed.
```
##### uc_link 
- it is a pointer member in `ucontext_t`. It is used to tell CPU "which context to jump next, after finishing current context". 
- works with makecontext()

#### swapcontext(&a, &b)
- saves the curret registers into a, and then loads b. 
- getcontext(&a) + setcontext(&b)

<br>

## Ping Pong program ( src/Test/ping_pong.c )
- three fuctions : main, ping, pong. No arguments to be passed, rather they are made global. Since only integer type arugemtns can be passed through makecontext(). 
- getcontext() initializes the struct, but never called it on maincontext() since it gets saved itself when swapcontext().
- `uc_stack` is a struct under which two members are used to work the stack: ss.size, ss_sp.
- makecontext() rewrites the saved registers so that when context is activated, `rip` is top of function and `rsp` top of `uc_stack` assigned. 
- `uc_link` is used to tell context where to go after it terminates. 


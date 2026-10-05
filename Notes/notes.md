## what is a thread? 
separate execution units sharing same address space and work on same data, that's logical defination. Practical defination is - thread is a combination of "stack" + "saved set of registers". When switching threads, we save one set of regs and load another set.

## Calling Convention x86-64
[AMD64 ABI (Application Binary Interface)](https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf) is a set of rules that defines how **compiled machine code** communicates, manages memory, and passes data on 64-bit x86-64 systems
<br>
Important registers for uthread:<br>
rsp : stack pointer <br>
rip : instruction address / execution location <br>
rbp, rbx, r12-r15 : callee-saved registers <br>

- names of the registers in x86-64 : rax **rbx** rcx rdx rsi rdi **rbp rsp** r8 r9 r10 r11 **r12 r13 r14 r15**.
- Important to the project : rsp rip rbp rbx r12 r13 r14 r15
- **rip** - Instruction pointer
> remembers the address to line of execution for each thread. Tracks the program's line of execution.

- **rsp** - Stack pointer
> points to current stack position. The 'stack' grows toward the lower addresses. Each thread gets its own stack. 

- **Stack Frame** 
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

- **The Calling Convention** : 
```C
    int add(int a, int b)
    {
        return a + b;
    }
```
how does a compiled machine code where are `a` and `b`. They know it by a rule, for any function

```text
AMD ABI Convention

first integer argument  → rdi
second integer argument→ rsi
third integer argument → rdx
fourth integer argument → rcx
fifth integer argument  → r8
sixth integer argument  → r9
```

## Context Switching
Context Switching in threads means saving and restoring the the `rsp` register + Callee's saved registers. 
<br>

## Kernal Threads 
- scheduler in the kernel handles the kernel stack. They can be easily blocked for I/O by the kernel.
- There can be multiple kernel stack but it could lead to use of significant amount of memory. Also switching between multiple stack again and again could lead to cache misses. 
- Another method could be threads sharing a single kernel stack. the `%rsp` must be reset at every context switch. 

## User threads
- created at userspace and give less overhead of creating and destroying. 
- User level threads have 3 types of models: 
> 1. 1:n Model - all user level thread are directly mapped to a single kernel thread. 
> 2. 1:1 Model - one user thread to one kernel, directly mapped. 
> 3. m:n Model (m > n) - for m number of user threads there are n number of kernel threads.

- We using 1:n model, since we are assuming our threads to be run on single core system.

### Process memory layout 
- stack stores variables, values, pointers to be used during a function runtime. A function gets a dedicated space in stack called 'stack frame'. 
```text

Higher Addresses
|--------------|
| Stack        |  (Grows Downwards)
|______________|
|              |
|              |  (free)
|______________|
| Heap         |  (Grows Upwards)
|              |  <-- Program Break (Manipulated by sbrk)
|              |  <-- malloc/free work within this area
|              |
|--------------|
| BSS Segment  |  (Uninitialized Data)
|--------------|
| Data Segment |  (Initialized Data)
|--------------|
| Text Segment |  (Code)
|--------------|
Lower Addresses

```
### what is a thread? 
separate execution units sharing same address space and work on same data, that's logical defination. Practical defination is - thread is a combination of "stack" + "saved set of registers". When switching threads, we save one set of regs and load another set.

### Calling Convention x86-64
[AMD64 ABI (Application Binary Interface)](https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf) is a set of rules that defines how **compiled machine code** communicates, manages memory, and passes data on 64-bit x86-64 systems
<br>
Important registers for uthread:<br>
rsp : stack pointer <br>
rip : instruction address / execution location <br>
rbp, rbx, r12-r15 : callee-saved registers <br>

- names of the registers in x86-64 : rax **rbx** rcx rdx rsi rdi **rbp rsp** r8 r9 r10 r11 **r12 r13 r14 r15**.
- Important to the project : rsp rip rbp rbx r12 r13 r14 r15
- **rip** - Instruction pointer
> remembers the address to line of execution for each thread. it helps to resume the thread after context switching.

- **rsp** - Stack pointer
> points to current stack position. The 'stack' grows toward the lower addresses. Each thread gets its own stack

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
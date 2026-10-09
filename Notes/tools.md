- `-O0` is used to compile without any optimization. 

- [CONTEXT Switching](https://mohitmishra786.github.io/chessman/2024/10/04/Context-Switching-in-Operating-Systems.html)

# GDB ( GNU DEBUGGER)
- compile your files with debug flag `gcc "-g" file.c -o file`
- `gdb ./file` 
- `break main` sets a stopper at main(). 

- There are 4 ways to step through the code: 

> 1. `next` : jumps the line according to C. A single C line is multiple Assembly line
> 2. `nexti` : jumps single assembly line

- `info registers` : tells about registers state at the given instruction
- `ref` : refresh the screen `
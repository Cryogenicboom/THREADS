#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <string.h>

// contexts and flags are initialized global, cannot do it in main() since makecontext() takes only integer parameters

ucontext_t ping_ctx;
ucontext_t pong_ctx;
ucontext_t main_ctx;

int pong_status = 0;
int ping_status = 0;

void ping()
{
    for(int i =0; i < 5; i++)
    {
        printf("Ping%d\n", i);
        swapcontext(&ping_ctx, &main_ctx);
    }
    ping_status = 1;
}  

void pong()
{
    for(int i =0; i < 5; i++)
    {
        printf("Pong%d\n", i);
        swapcontext(&pong_ctx, &main_ctx);
    }
    pong_status = 1;
}

int main()
{
    getcontext(&ping_ctx);
    ping_ctx.uc_link = &main_ctx;
    getcontext(&pong_ctx);
    pong_ctx.uc_link = &main_ctx;

    void* pong_stack = (void *)malloc(64*1024);
    if(pong_stack == NULL)
    {
        printf("No pong allocation found");
        return 0;
    }
    pong_ctx.uc_stack.ss_sp = pong_stack;
    pong_ctx.uc_stack.ss_size = 64*1024;

    void* ping_stack = (void *)malloc(64*1024);
    if(ping_stack == NULL)
    {
        printf("No ping allocation found");
        return 0;
    }
    ping_ctx.uc_stack.ss_sp = ping_stack;
    ping_ctx.uc_stack.ss_size = 64*1024;

    makecontext(&ping_ctx, ping, 0);
    makecontext(&pong_ctx, pong, 0);

    while(!(pong_status && ping_status))
    {
        if(ping_status != 1)
        {
            swapcontext(&main_ctx, &ping_ctx);
        }
        if(pong_status != 1)
        {
            swapcontext(&main_ctx, &pong_ctx);
        }
    }

        free(pong_stack);
        free(ping_stack);
        return 0;
}

// void function()
// {
//     printf("Function run\n");
// }

// int main()
// {
//     ucontext_t main_ctx;
//     ucontext_t f_ctx;

//     getcontext(&f_ctx);
//     f_ctx.uc_link = &main_ctx;
    
//     void* ptr = (void *)malloc((64*1024));
//     if(ptr == NULL)
//     {
//         printf("No allocation found");
//         return 0;
//     }
//     f_ctx.uc_stack.ss_sp = ptr;
//     f_ctx.uc_stack.ss_size = 64*1024;

//     makecontext(&f_ctx, function, 0);

//     printf("main : before\n");
//     swapcontext(&main_ctx, &f_ctx);
//     printf("main : after\n");

//     return 0;
// }

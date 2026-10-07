#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <string.h>

/*

int main()
{
    int count = 0;
    ucontext_t ctx;
    getcontext(&ctx);
    count++;
    printf("\n%d", count);
    if(count < 3)
    {
        setcontext(&ctx);
    }
}

*/


  // 1. declare main_ctx and f_ctx            <- you did this
  // 2. initialize f_ctx with getcontext
  // 3. give f_ctx a stack (malloc 64KB), set ss_sp and ss_size
  // 4. set f_ctx.uc_link to main_ctx
  // 5. makecontext(f_ctx, function, 0)
  // 6. print "main: before"
  // 7. swapcontext(main_ctx, f_ctx)
  // 8. print "main: after"
  
void function()
{
    printf("Function run\n");
}

int main()
{
    ucontext_t main_ctx;
    ucontext_t f_ctx;

    getcontext(&f_ctx);
    // f_ctx.uc_link = &main_ctx;
    
    void* ptr = (void *)malloc((64*1024));
    if(ptr == NULL)
    {
        printf("No allocation found");
        return 0;
    }
    f_ctx.uc_stack.ss_sp = ptr;
    f_ctx.uc_stack.ss_size = 64*1024;

    makecontext(&f_ctx, function, 0);

    printf("main : before\n");
    swapcontext(&main_ctx, &f_ctx);
    printf("main : after\n");

    return 0;
}

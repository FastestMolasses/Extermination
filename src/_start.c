// NEARMISS _start  (vram 0xt, ?) — readable decompilation, NOT byte-identical.
//
// objdiff 0.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef unsigned int u32;

#define BSS_START   0x00275B00u   /* D_00275B00 */
#define BSS_END     0x00AC2D80u   /* D_AC2D80: also the heap base            */
#define GP_BASE     0x0027D370u   /* D_0027D370                              */
#define STACK_TOP   0x01F00000u   /* D_1F00000                               */
#define STACK_SIZE  0x00100000u
#define ARGS_BLOCK  0x00275D00u   /* D_00275D00: [0] = argc, [1..] = argv    */

/* EE kernel syscalls 60 / 61. */
extern void *SetupThread(void *gp, void *stack, int stack_size,
                         void *args, void (*root)(void));
extern void  SetupHeap(void *heap_base, int heap_size);

extern void  func_001000B0(void);          /* thread root function */
extern void  DisableDmacHandler(int channel);
extern int   gs_readback_queue_run(int argc, char **argv);  /* == main() */
extern void  Exit(int status);

void _start(void)
{
    u32 *p;
    int  argc;
    char **argv;

    /* 1. clear BSS, one quadword at a time */
    for (p = (u32 *)BSS_START; p < (u32 *)BSS_END; p += 4) {
        p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 0;   /* sq $zero, 0x0($v0) */
    }

    /* 2-3. install $gp, then let the kernel build the main thread's stack */
    /*      move $gp, $a0 happens before the syscall; move $sp, $v0 after  */
    SetupThread((void *)GP_BASE, (void *)STACK_TOP, (int)STACK_SIZE,
                (void *)ARGS_BLOCK, func_001000B0);

    /* 4. everything above the cleared BSS becomes the heap */
    SetupHeap((void *)BSS_END, -1);

    /* 5. */
    DisableDmacHandler(0);
    /* ei */

    /* 6-7. */
    argc = *(int *)ARGS_BLOCK;
    argv = (char **)(ARGS_BLOCK + 4);
    Exit(gs_readback_queue_run(argc, argv));
}

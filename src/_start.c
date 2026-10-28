// NEARMISS _start  (vram 0x00100008, 0xA0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 26.40% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff needs instructions that plain C cannot express:
// hand-written crt0 assembly (splat flags handwritten instructions): it writes $gp and $sp
// directly, issues the kernel SetupThread/SetupHeap syscalls inline, enables interrupts after
// FlushCache, has no stack frame and pads the BSS loop with two nops. C-only, the syscalls are
// extern calls and the function gets a frame; the BSS loop, argument setup and the
// main/Exit tail line up. Not matchable without inline asm (not allowed).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

typedef unsigned int u128 __attribute__((mode(TI)));

extern u128 D_00275B00[];          /* BSS start                         */
extern u128 D_AC2D80[];            /* BSS end, also the heap base       */
extern char D_0027D370[];          /* $gp value                         */
extern char D_1F00000[];           /* stack top                         */
extern char D_00100000[];          /* stack size (0x100000)             */
extern int  D_00275D00[];          /* [0] = argc, [1..] = argv          */

extern void *SetupThread(void *gp, void *stack, int stack_size,
                         void *args, void (*root)(void));
extern void  SetupHeap(void *heap_base, int heap_size);
extern void  func_001000B0(void);
extern void  FlushCache(int channel);
extern int   gs_readback_queue_run(int argc, char **argv);
extern void  Exit(int status);

void _start(void)
{
    u128 *p = D_00275B00;
    int *args;

    /* 1. clear BSS, one quadword at a time */
    do {
        *p = 0;
    } while (p++ < D_AC2D80);

    /* 2-3. install $gp, then let the kernel build the main thread's stack */
    SetupThread(D_0027D370, D_1F00000, (int)D_00100000, D_00275D00, func_001000B0);

    /* 4. everything above the cleared BSS becomes the heap */
    SetupHeap(D_AC2D80, -1);

    /* 5. */
    FlushCache(0);

    /* 6-7. */
    args = D_00275D00;
    Exit(gs_readback_queue_run(args[0], (char **)(args + 1)));
}

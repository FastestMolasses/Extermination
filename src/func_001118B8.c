// NEARMISS func_001118B8  (vram 0x001118B8, 0x94 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.70% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler wall (sched1/pre-reload). Logic fully recovered: 39 instrs both sides, identical frame 0x50, identical save stride, identical guard-branch structure including gcc's duplicated `lui s0,%hi(D_00241D0C)` + `addiu v0,zero,1` in both arms, and all four global stores / three Create...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// Lazily create the three global semaphores used by this subsystem.
// "EndOfHeap" is the project name for the EE syscall-64 stub, which is
// really CreateSema(ee_sema_t *) -> semaphore id.
typedef struct {
    int count;
    int max_count;
    int init_count;
    int wait_threads;
    int attr;
    int option;
} ee_sema_t;

extern int D_00241D08;
extern int D_00241D0C;
extern int D_00241D10;
extern int D_00241D14;

extern int EndOfHeap(ee_sema_t *param);

void func_001118B8(void) {
    ee_sema_t sema;

    if (D_00241D08 != -1 && D_00241D0C != -1) {
        return;
    }
    sema.option = 0;
    sema.max_count = 1;
    sema.init_count = 1;
    D_00241D08 = EndOfHeap(&sema);
    D_00241D0C = EndOfHeap(&sema);
    sema.init_count = 0;
    D_00241D10 = EndOfHeap(&sema);
    D_00241D14 = 0;
}

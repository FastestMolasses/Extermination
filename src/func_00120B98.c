// NEARMISS func_00120B98  (vram 0x00120B98, 0x150 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 83.60% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// This is an EE-GCC function, NOT mwcc (target uses sd/ld + daddu register saves, not mwcc sq/paddub). Both mwcc builds plateau <=50%; ee-gcc 2.9-991111-01 reaches 83.6% with logic fully recovered. The `int size=0x400` hoist reproduces the target's promotion of 0x400 into callee-saved s2 and the 0x...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// Stream/handle (re)opener on object arg0. If flag 0x2 is set in the u16 at
// arg0+0xC, point the read cursor (arg0+0x10 / arg0+0) at the inline 0x43 byte
// at arg0+0x43 and mark len=1. Otherwise probe via func_00120578 on the handle
// at arg0+0x54 (using the s16 id at arg0+0xE) filling a local stat; classify
// the resulting mode word (sp+4) -> 0xF000 bits, recording a 0x800/0x400 flag
// and optionally 0x4C=0x400 for the dir vtable at 0011FE90+0x28. Then map the
// handle through func_00120F40; on failure fall back to the inline buffer +
// flag 0x2, on success wire arg0+0x10/0/0x14 to the mapped ptr, set the read
// callback func_00120038 at (handle+0x3C), flag 0x80, and if it was a 0x2000
// node call func_0010BF10(id, ptr) and OR in flag 0x1 on success.
//
// This is an EE-GCC function (sd/ld + daddu register saves, not mwcc sq/paddub).
// NEARMISS 83.6% (ee-gcc 2.9-991111-01; both mwcc builds <=50%). Logic fully
// recovered; the local `size = 0x400` hoist reproduces the target's promotion of
// 0x400 into callee-saved s2 and the 0xb0 frame. Sole residuals: the confirmed
// EE-GCC forward branch-likely wall -- target emits `bgezl`/`bnezl` on the two
// forward success checks where our rebuild only emits the non-annul `bgez`/`bnez`
// (proven not source/flag-crackable, s84) -- plus list-scheduler adjacent store
// reordering and minor GPR coloring. PARK.
extern int func_0010BF10(short id, char *p);
extern int func_00120578(char *h, short id, void *stat);
extern char *func_00120F40(char *h, int n);
extern int func_00120038;
extern int vtable_a0_at_0011FE90_off28;

void func_00120B98(char *arg0) {
    unsigned short fl;
    short id;
    int mode;
    int is2000;
    char *p;
    char stat[0x70];
    int size = 0x400;

    fl = *(unsigned short *)(arg0 + 0xC);
    if (fl & 2) {
        *(int *)(arg0 + 0x14) = 1;
        *(char **)(arg0 + 0x10) = arg0 + 0x43;
        *(char **)(arg0 + 0) = arg0 + 0x43;
        return;
    }

    id = *(short *)(arg0 + 0xE);
    if (id < 0) {
        is2000 = 0;
        *(unsigned short *)(arg0 + 0xC) = fl | 0x800;
    } else if (func_00120578(*(char **)(arg0 + 0x54), id, stat) < 0) {
        is2000 = 0;
        *(unsigned short *)(arg0 + 0xC) = *(unsigned short *)(arg0 + 0xC) | 0x800;
    } else {
        mode = *(int *)(stat + 4) & 0xF000;
        is2000 = (mode == 0x2000);
        if (mode == 0x8000 &&
            *(int *)(arg0 + 0x28) == (int)&vtable_a0_at_0011FE90_off28) {
            *(int *)(arg0 + 0x4C) = size;
            *(unsigned short *)(arg0 + 0xC) = *(unsigned short *)(arg0 + 0xC) | 0x400;
        } else {
            *(unsigned short *)(arg0 + 0xC) = *(unsigned short *)(arg0 + 0xC) | 0x800;
        }
    }

    p = func_00120F40(*(char **)(arg0 + 0x54), size);
    if (p == 0) {
        *(char **)(arg0 + 0x10) = arg0 + 0x43;
        *(int *)(arg0 + 0x14) = 1;
        *(unsigned short *)(arg0 + 0xC) = *(unsigned short *)(arg0 + 0xC) | 2;
        *(char **)(arg0 + 0) = arg0 + 0x43;
    } else {
        *(char **)(arg0 + 0x10) = p;
        *(int *)(*(char **)(arg0 + 0x54) + 0x3C) = (int)&func_00120038;
        *(unsigned short *)(arg0 + 0xC) = *(unsigned short *)(arg0 + 0xC) | 0x80;
        *(int *)(arg0 + 0x14) = size;
        *(char **)(arg0 + 0) = p;
        if (is2000) {
            if (func_0010BF10(*(short *)(arg0 + 0xE), p) != 0) {
                *(unsigned short *)(arg0 + 0xC) = *(unsigned short *)(arg0 + 0xC) | 1;
            }
        }
    }
}

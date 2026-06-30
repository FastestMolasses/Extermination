// NEARMISS func_001FD580  (vram 0x001FD580, 0x118 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.19% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation permutation: instruction stream is 1:1 with the target (same opcodes, same order) but physical register coloring differs (target key->t0/base->a1/ctr->a0/matchptr->a2/termptr->a3 vs mine a4/a0/a3/a1/a2). No idiom flips this; permuter territory. Body byte-faithful.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (o233 91.2%, o991 76.9%): logic fully recovered; instruction stream is
// 1:1 with the target -- sole residual is a register-allocation permutation.
//
// Two-phase lookup keyed by the global state byte D_00810700:
//   base = D_00264DD4[key]            // pointer into a per-state entry array
// Phase 1: walk the parallel table D_0026EC60 (0x10-byte entries) while entry->f0
//   != -1. When entry->f0 == key AND entry->f8 == arg0, this (key,arg0) pair is
//   already "claimed": clear D_008106F4, set *arg1 = -1, return 2.
// Phase 2: from base, scan entries of stride 8 starting at index arg0, skipping
//   slots whose short at +2 == -1. If such a skipped slot has a nonzero byte at +5,
//   bail: *arg1 = -1, return 0. On the first slot with short@+2 != -1: set
//   D_008106F5 = 2, func_001FA5A0(slot->short@+2), *arg1 = arg0 + n, return 1.
//
// WALL: register-allocation permutation. Every instruction matches in opcode and
// order; only the physical registers differ -- the target colors key->t0, base->a1,
// counter->a0, match-ptr->a2, term-ptr->a3, whereas mwcc here picks a4/a0/a3/a1/a2.
// This is permuter territory (no idiom flips the coloring); body is byte-faithful.

extern unsigned char D_00810700;
extern int D_00264DD4[];
extern int D_0026EC60[];
extern unsigned char D_008106F4;
extern unsigned char D_008106F5;
extern void func_001FA5A0(int a);

int func_001FD580(int arg0, int *arg1) {
    unsigned char key;
    int *base;
    int *pa;
    int *pb;
    int i;
    int n;
    short *e;

    key = D_00810700;
    base = (int *)D_00264DD4[key];
    pa = D_0026EC60;
    pb = D_0026EC60;
    i = 0;
    while (*pb != -1) {
        if (key == *pa) {
            if (arg0 == pa[2]) {
                D_008106F4 = 0;
                *arg1 = -1;
                return 2;
            }
        }
        pa += 4;
        pb += 4;
        i++;
    }

    n = 0;
    e = (short *)(base + (arg0 + n) * 2);
    while (*e == -1) {
        if (*((unsigned char *)e + 5) != 0) {
            *arg1 = -1;
            return 0;
        }
        n++;
        e = (short *)(base + (arg0 + n) * 2);
    }
    D_008106F5 = 2;
    func_001FA5A0(*e);
    *arg1 = arg0 + n;
    return 1;
}

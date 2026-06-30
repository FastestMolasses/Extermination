// NEARMISS func_001FD6A0  (vram 0x001FD6A0, 0xE4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 60.44% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation / loop-induction-variable permutation. Instruction content matches (same loads, same beqzl branch-likely, same call), but CW promotes arg0 to a callee-saved reg ($s2, 4 saved regs, frame 0x40) to recompute arg0+1+i after the call; mwcc keeps a running arg0+1 in a caller reg an...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS func_001FD6A0 (vram 0x001FD6A0, 0xE4 bytes) — readable decompilation, NOT byte-identical.
// 60.4% via mwcc 2.3.3 (best); 49.9% via mwcc 991202. Logic + structure fully recovered.
//
// Table-walk lookup. base = D_00264DD4[D_00810700] (D_00264DD4 is an array of base pointers,
// indexed by the current-set u8 D_00810700; each base points to 8-byte records, field +2 = s16
// id, field +5 = u8 flag). If the record at index arg0 has flag(+5)==1, store -1 to *arg1 and
// return 0. Otherwise scan records from index arg0+1 upward: the first record whose id(+2) != -1
// is the hit -> set the global state byte D_008106F5 = 2, call func_001FA5A0(id), store the
// found index (arg0+1+i) to *arg1, return 1. A record with id(+2)==-1 AND flag(+5)!=0 ends the
// scan as a miss (store -1, return 0); id==-1 with flag==0 skips to the next record. The +5!=0
// guard compiles to a branch-likely (beqzl) with the i++ in its delay slot.
//
// WALL: register-allocation / loop-induction permutation (NOT reachable from C). The instruction
// CONTENT matches (same loads, same beqzl, same calls), but the CW target promotes arg0 to a
// callee-saved reg ($s2) so it can recompute arg0+1+i for the post-call *arg1 store — 4 saved
// regs, frame 0x40. mwcc instead keeps a running arg0+1 in a caller reg and post-increments it
// (addiu a0,a0,1) — 3 saved regs, frame 0x30 — changing the loop back-edge and the saved-reg
// set. Tested index-as-counter, running-pointer, and inverted-condition loop shapes; none flips
// which quantity mwcc keeps callee-saved. This is the regalloc/IV-coloring class -> permuter.
extern unsigned char D_00810700;
extern char D_008106F5;
extern char *D_00264DD4[];

extern int func_001FA5A0(int);

int func_001FD6A0(int arg0, int *arg1) {
    char *base;
    char *e;
    int i;

    base = D_00264DD4[D_00810700];
    if (*(unsigned char *)(base + arg0 * 8 + 5) == 1) {
        *arg1 = -1;
        return 0;
    }
    for (i = 0; ; i++) {
        e = base + ((arg0 + 1) + i) * 8;
        if (*(short *)(e + 2) == -1) {
            if (*(unsigned char *)(e + 5) != 0) {
                *arg1 = -1;
                return 0;
            }
            continue;
        }
        D_008106F5 = 2;
        func_001FA5A0(*(short *)(e + 2));
        *arg1 = (arg0 + 1) + i;
        return 1;
    }
}

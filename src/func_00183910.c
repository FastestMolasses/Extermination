// NEARMISS func_00183910  (vram 0x00183910, 0x90 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.58% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-coloring / delay-slot scheduling: CW keeps arg0 in $a0 across the dispatch and copies it to callee-saved $s0 in the beq delay slot (dispatch byte in $a1); mwcc saves arg0->$s0 eagerly and loads the byte into $a0 (nop in slot). Body, beql branch-likely dispatch, and gp-rel load all byte-i...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Actor sub-state step keyed on the byte at +6 (3-state machine).
//  state 0: if the gp-rel global (&D_00275B00)[2] is nonzero, advance the +6
//           byte to 1 and clear the int at +0x2EC.
//  state 1: run func_00179880(self+0x2EC); then if func_00175900(self, 1) is
//           nonzero advance the +6 byte by 1.
//  state 2: no-op.
// gp-rel global access uses -sdatathreshold 4 ((&D_00275B00)[2] == offset 8).
// NEARMISS (94.6% vs build/expected/func_00183910.o, mwcc 2.3.3): body, the
// beql branch-likely dispatch, and the gp-rel load all match byte-for-byte. The
// sole residual is a register-allocation / delay-slot-scheduling difference:
// CodeWarrior keeps arg0 in $a0 across the dispatch and copies it to the
// callee-saved $s0 in the `beq` branch delay slot (loading the dispatch byte
// into $a1), whereas mwcc saves arg0->$s0 eagerly and loads the byte into $a0
// (nop in the slot). Pure coloring/scheduling artifact; logic is exact.
extern int func_00179880(int *);
extern int func_00175900(char *, int);
extern int D_00275B00;

void func_00183910(char *arg0) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        if ((&D_00275B00)[2] != 0) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(int *)(arg0 + 0x2EC) = 0;
        }
        break;
    case 1:
        func_00179880((int *)(arg0 + 0x2EC));
        if (func_00175900(arg0, 1) != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        }
        break;
    case 2:
        break;
    }
}

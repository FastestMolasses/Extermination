// NEARMISS func_001F18C0  (vram 0x001F18C0, 0x1E0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.25% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Scheduling/canonicalization permutation (not the clean-store nop, 2.3.3 cannot fix). Body, control flow, all struct offsets and call sites fully recovered and verified. Residual = 3 instructions at 98.25% (mwcc233): (1) idx28 a nop the target keeps in the func_001F1550 result-branch delay slot th...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// NEARMISS 98.25% (mwcc233). Body/structure fully recovered; residual is pure
// compiler artifact: 2 scheduling nops (delay-slot/loop-align) the target keeps
// + one commutative `addu base,row*4` operand-order canonicalization.
//
// State machine on the byte at arg0+4, driving a per-frame effect on a
// substruct at arg0+0x1F0 (p): p[0]=param table, p[4]=row cursor, p[8]=slot.
//   state 0: init  - if func_001F1550(arg0,3) succeeds, walk the 3 slot lists in
//            *D_00275B40 (gp-rel sdata ptr table), zero each row's +0xA0 field
//            and seed both halves via func_001026A0; reset cursors, advance->1.
//   state 1: step  - process one row: store 10.0f (0x41200000) at +0xA0, seed
//            the row pair via func_001026A0, run func_001F15F0, advance the row
//            cursor by 2; wrap row(>=0xA)->next slot, slot(>=3)->0.
//   state 2/3: tear down via func_001AFC10(arg0).
extern int D_00275B40;
extern int func_001F1550();
extern void func_001026A0();
extern void func_001F15F0();
extern void func_001AFC10();

void func_001F18C0(char *arg0) {
    int i, j;
    int base;
    char *p;

    p = arg0 + 0x1F0;
    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        if (func_001F1550(arg0, 3) != 0) {
            for (i = 0; i < 3; i++) {
                base = *(int *)(D_00275B40 + i * 4);
                for (j = 0; j < 0xA; j++) {
                    *(int *)(base + j * 4 + 0xA0) = 0;
                    func_001026A0(base + j * 0x10, *(int *)p, p + 0x10);
                    func_001026A0(base + (j + 1) * 0x10, *(int *)p, p + 0x20);
                }
            }
            *(int *)(p + 4) = 0;
            *(int *)(p + 8) = 0;
            *(unsigned char *)(arg0 + 4) = 1;
        } else {
            return;
        }
        /* fallthrough */
    case 1:
        base = *(int *)(D_00275B40 + *(int *)(p + 8) * 4);
        *(int *)(base + *(int *)(p + 4) * 4 + 0xA0) = 0x41200000;
        func_001026A0(base + *(int *)(p + 4) * 0x10, *(int *)p, p + 0x10);
        func_001026A0(base + (*(int *)(p + 4) + 1) * 0x10, *(int *)p, p + 0x20);
        func_001F15F0(arg0, *(int *)(p + 8), *(int *)(p + 4));
        *(int *)(p + 4) = *(int *)(p + 4) + 2;
        if (*(int *)(p + 4) >= 0xA) {
            *(int *)(p + 4) = 0;
            *(int *)(p + 8) = *(int *)(p + 8) + 1;
            if (*(int *)(p + 8) >= 3) {
                *(int *)(p + 8) = 0;
            }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}

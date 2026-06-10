// CFLAGS: -O4,p -sdatathreshold 0
// task_register(slot_idx, fn): arms frame-task slot `idx` of the 3-slot table
// at D_0028A750 (0x20 bytes/slot): state byte +0 = 1 ("just registered" — the
// dispatcher func_001AB6A0 promotes 1 -> 2 and then calls fn each frame),
// fn pointer word +4 (D_0028A754), and clears the four state words +8..+0x14.
//
// Matching notes (100%): the dsll32/dsra32 head is the (long) widening of the
// int index before <<5. `fnp` must be built from the SEPARATE symbol
// D_0028A754 (two reloc pairs, like the original), split across two
// statements with `idx = 1` between them — mwcc emits independent scalar
// statements in source order, which reproduces CW 2.3.1's
// [pair][const][addu][sb] emission (statement-order idiom; a single-statement
// `fnp = &D_0028A754 + off` gets sunk below the sb instead). Reusing the dead
// `idx` param for the constant 1 lands it in $a0 like the original.
extern char D_0028A750;
extern char D_0028A754;

void func_001AB740(int idx, int fn, int off, char *slot) {
    long l = idx;
    char *fnp;
    off = (int)(l << 5);
    slot = &D_0028A750 + off;
    fnp = &D_0028A754;
    idx = 1;
    fnp += off;
    *slot = idx;
    *(int *)fnp = fn;
    *(int *)(slot + 0x8) = 0;
    *(int *)(slot + 0xc) = 0;
    *(int *)(slot + 0x10) = 0;
    *(int *)(slot + 0x14) = 0;
}

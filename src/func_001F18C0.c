// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// State machine on the status byte at arg0+4, driving the per-frame bone-matrix
// effect over the substruct at arg0+0x1F0 (p): p[0]=param table, p[4]=row cursor,
// p[8]=slot index into the gp-relative node-pointer table D_00275B40.
//   state 0: init - func_001F1550(arg0,3) must succeed, then walk all 3 slot lists,
//            clear each row's +0xA0 field and seed both halves via func_001026A0;
//            reset both cursors and advance to state 1 (falls through into it).
//   state 1: step - store 10.0f (0x41200000) at the current row's +0xA0, seed the
//            row pair via func_001026A0, run func_001F15F0, advance the row cursor
//            by 2; wrap row(>=0xA) to the next slot and slot(>=3) back to 0.
//   state 2/3: tear down via func_001AFC10(arg0).
extern int D_00275B40;
extern int func_001F1550(void *, int);
extern void func_001026A0(void *, void *, void *);
extern void func_001F15F0(void *, int, int);
extern void func_001AFC10(void *);

void func_001F18C0(char *arg0) {
    int i, j;
    int base;
    char *p;

    p = arg0 + 0x1F0;
    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        if (func_001F1550(arg0, 3) == 0) {
            return;
        }
        for (i = 0; i < 3; i++) {
            base = *(int *)(D_00275B40 + i * 4);
            for (j = 0; j < 0xA; j++) {
                *(int *)(base + j * 4 + 0xA0) = 0;
                func_001026A0((void *)(base + j * 0x10), *(void **)p, p + 0x10);
                func_001026A0((void *)(base + (j + 1) * 0x10), *(void **)p, p + 0x20);
            }
        }
        *(int *)(p + 4) = 0;
        *(int *)(p + 8) = 0;
        *(unsigned char *)(arg0 + 4) = 1;
        /* fallthrough */
    case 1:
        base = *(int *)(D_00275B40 + *(int *)(p + 8) * 4);
        *(int *)((char *)(*(int *)(p + 4) * 4) + base + 0xA0) = 0x41200000;
        func_001026A0((void *)(base + *(int *)(p + 4) * 0x10), *(void **)p, p + 0x10);
        func_001026A0((void *)(base + (*(int *)(p + 4) + 1) * 0x10), *(void **)p, p + 0x20);
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

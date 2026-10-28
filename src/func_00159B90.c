// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 90.64% NEARMISS).
// Fixes read from the original instructions:
// - The outer state switch ends every case in `break` (no early returns), which
//   gives the original's branch-likely case entries and shared epilogue.
// - The result test of func_00157CE0 is an if / else-if chain testing 2 first,
//   then 3, else 3 is stored.
// - func_001BA1A0 is declared with its definition's prototype (int *, int); the
//   table address is passed converted to int, which sets it up ahead of the
//   first argument as in the original.
//
// State handler on p[4]: state 0 (when func_001B0FD0 returns 0) sets fields and
// four scratchpad floats and calls func_001C5570; state 1 steps a sub-state on
// p[5], then for two scratchpad vectors calls func_001026A0 with p+0xD0 and
// func_00158590, then func_001B17A0 and the callback at p+0x4C; state 2
// advances to state 3; state 3 calls func_001AFC10.

extern int func_001B0FD0(unsigned char *p);
extern void func_001C6380(unsigned char *p);
extern int func_00157CE0(unsigned char *p, unsigned char *ev);
extern void func_00102948(void *dst, void *src);
extern void func_001FB9F0(int a, int b, int c, int d);
extern void func_001BA1A0(int *ev, int tbl);
extern int func_001BA1F0(unsigned char *p);
extern void func_001C5570(unsigned char *p, void *v, int a, int b);
extern void func_001026A0(void *v, void *a, void *b);
extern void func_00158590(unsigned char *p, int a, int b);
extern void func_001B17A0(unsigned char *p);
extern void func_001AFC10(unsigned char *p);

extern void *D_00275460;
extern char D_002470E0[];
extern float D_00810350[];
extern float D_00810370[];
extern float D_00810710[];
extern float D_00810720[];
extern float D_700038A0[4];

void func_00159B90(unsigned char *p) {
    unsigned char st;
    unsigned char st2;
    int r;
    unsigned char *e;

    st = p[4];
    e = p + 0x1F0;
    switch (st) {
    case 0:
        if (func_001B0FD0(p) == 0) {
            func_001C6380(p);
            p[0xA] = 0;
            *(void **)(p + 0x30) = &D_00275460;
            *(short *)(p + 0x34) = 2;
            p[0] = 1;
            *(volatile float *)0x700038A0 = 0.0f;
            *(volatile float *)0x700038A4 = 1.0f;
            *(volatile float *)0x700038A8 = 0.0f;
            *(volatile float *)0x700038AC = 1.0f;
            func_001C5570(p, D_700038A0, 0x76, 1);
        }
        break;
    case 1:
        st2 = p[5];
        switch (st2) {
        case 0:
            if ((r = func_00157CE0(p, e)) != 0) {
                func_00102948(D_00810710, D_00810350);
                func_00102948(D_00810720, D_00810370);
                func_001FB9F0(0x3E8, 0x1000, 0x1000, 0x1000);
                if (r == 2) {
                    p[5] = 2;
                } else if (r == 3) {
                    p[5] = p[5] + 1;
                } else {
                    p[5] = 3;
                }
            }
            break;
        case 1:
            p[5] = st2 + 1;
            func_001BA1A0((int *)e, (int)D_002470E0);
            break;
        case 2:
            if (func_001BA1F0(p) != 0) {
                p[0xB] = 0;
                p[0] = 1;
                p[5] = 0;
            }
            break;
        case 3:
            if (func_001BA1F0(p) != 0) {
                p[5] = 1;
            }
            break;
        }
        *(volatile float *)0x700038A0 = -1.75f;
        *(volatile float *)0x700038A4 = 1.7f;
        *(volatile float *)0x700038A8 = 2.03f;
        *(volatile float *)0x700038AC = 1.0f;
        func_001026A0(D_700038A0, p + 0xD0, D_700038A0);
        func_00158590(p, 1, -2);
        *(volatile float *)0x700038A0 = 1.75f;
        *(volatile float *)0x700038A4 = 1.7f;
        *(volatile float *)0x700038A8 = 2.03f;
        *(volatile float *)0x700038AC = 1.0f;
        func_001026A0(D_700038A0, p + 0xD0, D_700038A0);
        func_00158590(p, 1, -2);
        func_001B17A0(p);
        (*(void (**)(unsigned char *))(p + 0x4C))(p);
        break;
    case 2:
        p[4] = st + 1;
        break;
    case 3:
        func_001AFC10(p);
        break;
    }
}

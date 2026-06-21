// COMPILER: eegcc
// CFLAGS: -O2
// SDK/eegcc-convention leaf: 16-byte callee-save stride (sd s0/sd ra), ra-at-top
// frame. Allocates via func_00121AF8(a0, 1), then initializes the returned record:
// field [0x14] = caller's second arg, field [0x10] = 1; returns the pointer.
// (Pure mwcc maxes at ~46-50% here -- wrong frame/save convention; ee-gcc 2.9 -O2 is byte-exact.)
extern int *func_00121AF8(int a0, int a1);

int *func_00121F78(int a0, int a1) {
    int *p = func_00121AF8(a0, 1);
    p[5] = a1;
    p[4] = 1;
    return p;
}

// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Display-list / packet emitter: prime a sub-handle via func_001DF600(3, 0,
// 0x80808080), then write a fixed command header into the current packet
// cursor at (D_00275670)+0x1C: byte 0x60 at +3, zero the word at +4 and the
// half at +0, advance the cursor by 0x10, and fire func_001CB760(D_007635C0,
// 0xFFF000, handle, header). Near-identical sibling of func_001DF5A0 (which
// uses func_001DF180 and a different prime arg).
//
// NOTE: func_001DF600's 3rd parameter is 64-bit -- the original builds the
// 0x80808080 arg with ori/dsll16/ori (64-bit constant materialization), not
// lui/ori. Declaring that param `unsigned long long` is what reproduces it.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202, which
// reschedules the packet-write / delay-slot store-back sequence and caps at
// 74.07%. 2.3.3 is byte-identical (objdiff 100.0).
extern int func_001DF600(int a0, int a1, unsigned long long a2);
extern void func_001CB760(char *a0, int a1, int a2, char *a3);
extern char *D_00275670;
extern char D_007635C0[8];

void func_001DF9D0(void) {
    char *t1;
    char *a3;
    char *v0;
    int a2;
    int a1;
    int t0;

    a2 = func_001DF600(3, 0, 0x80808080);
    t1 = D_00275670;
    t0 = 0x60;
    a3 = *(char **)(t1 + 0x1c);
    a1 = 0x00fff000;
    a3[3] = t0;
    v0 = *(char **)(t1 + 0x1c);
    *(int *)(v0 + 4) = 0;
    v0 = *(char **)(t1 + 0x1c);
    *(short *)v0 = 0;
    v0 = *(char **)(t1 + 0x1c);
    v0 = v0 + 0x10;
    *(char **)(t1 + 0x1c) = v0;
    func_001CB760(D_007635C0, a1, a2, a3);
}

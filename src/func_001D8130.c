// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Copies 22 float fields from a table entry (returned by func_001D7B30) into
// the active record D_00275688 (= &D_00817BC0), zeroing offset 0xB0 and writing
// float 0x43000000 (128.0) at 0x12C. The dest global pointer is re-read on each
// store (it lives in sdata, gp-relative). mwcc 2.3.3 reproduces the exact
// instruction scheduling that the 991202 build does not (97.5%).
// Like its sibling stages func_001D8340 / func_001D8690 it is called with the
// 0x20 flag and the record pointer (func_001D88B0 passes (0x20, arg0),
// func_001D89D0 (flag & 0x20, arg0 + 0xB0)); this stage reads neither. The
// old C declared it (void) (decomp2 lane round 1; same bytes).
extern char *func_001D7B30(void);
extern char *D_00275688;
extern char D_00817BC0[];

void func_001D8130(int flag, int p) {
    char *src;

    D_00275688 = D_00817BC0;
    src = func_001D7B30();
    *(int *)(D_00275688 + 0xB0) = 0;
    *(float *)(D_00275688 + 0xB4) = *(float *)(src + 0x1C);
    *(float *)(D_00275688 + 0x80) = *(float *)(src + 0x20);
    *(float *)(D_00275688 + 0x84) = *(float *)(src + 0x24);
    *(float *)(D_00275688 + 0xF0) = *(float *)(src + 0x28);
    *(float *)(D_00275688 + 0xF4) = *(float *)(src + 0x2C);
    *(float *)(D_00275688 + 0xF8) = *(float *)(src + 0x30);
    *(float *)(D_00275688 + 0xFC) = *(float *)(src + 0x34);
    *(float *)(D_00275688 + 0x90) = *(float *)(src + 0x38);
    *(float *)(D_00275688 + 0x94) = *(float *)(src + 0x3C);
    *(float *)(D_00275688 + 0x100) = *(float *)(src + 0x40);
    *(float *)(D_00275688 + 0x104) = *(float *)(src + 0x44);
    *(float *)(D_00275688 + 0x108) = *(float *)(src + 0x48);
    *(float *)(D_00275688 + 0x10C) = *(float *)(src + 0x4C);
    *(float *)(D_00275688 + 0xA0) = *(float *)(src + 0x50);
    *(float *)(D_00275688 + 0xA4) = *(float *)(src + 0x54);
    *(float *)(D_00275688 + 0x110) = *(float *)(src + 0x58);
    *(float *)(D_00275688 + 0x114) = *(float *)(src + 0x5C);
    *(float *)(D_00275688 + 0x118) = *(float *)(src + 0x60);
    *(float *)(D_00275688 + 0x11C) = *(float *)(src + 0x64);
    *(float *)(D_00275688 + 0x120) = *(float *)(src + 0x68);
    *(float *)(D_00275688 + 0x124) = *(float *)(src + 0x6C);
    *(float *)(D_00275688 + 0x128) = *(float *)(src + 0x70);
    *(int *)(D_00275688 + 0x12C) = 0x43000000;
}

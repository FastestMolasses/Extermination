// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// SEMANTICS: per-frame tick for the EX_DATA screen/overlay object at arg0.
// func_001FE9A0(1, arg0+0x50, arg0+0x54) polls the underlying stream/decoder;
// a 0 result aborts the tick (return 0). Otherwise the frame is composed:
// func_00122D10 blits/converts into the D_00822010 work buffer using the two
// lookup/palette tables D_00273960 and D_002738E0 selected by the mode byte at
// arg0+0xA, then func_001FE7C0 uploads it (dst arg0+0x64, the two ints at
// arg0+0x48/0x4C, source D_00822010, heap D_00810700, size 0x640).
// The gp-relative flag D_00275C58 is an abort/skip request: when set the tick
// reports 2 immediately. Otherwise the uploader's status is mapped to this
// function's own status code: -1 (error) -> 2, 1 -> 1, 0 -> 0, anything else 1.
//
// NOTE: the -sdatathreshold must be 8 (not 0) so that the 4-byte extern
// D_00275C58 is addressed gp-relative, matching `lw v1, %gp_rel(D_00275C58)(gp)`.
// The case labels are written ASCENDING (0, 1, -1); mwcc reverses the compare
// chain to the target's descending -1 / 1 / 0 dispatch with its beql shapes.

extern int func_001FE9A0(int mode, void *a, void *b);
extern void func_00122D10(void *dst, void *src1, void *src2, int idx);
extern int func_001FE7C0(void *dst, int a, int b, void *buf, void *heap, int size);
extern int D_00275C58;
extern unsigned char D_00822010[];
extern unsigned char D_00273960[];
extern unsigned char D_002738E0[];
extern unsigned char D_00810700[];

int sub_s_EX_DATA_02d(unsigned char *p) {
    int r;

    if (func_001FE9A0(1, p + 0x50, p + 0x54) == 0) {
        return 0;
    }
    func_00122D10(D_00822010, D_00273960, D_002738E0, p[0xA]);
    r = func_001FE7C0(p + 0x64, *(int *)(p + 0x48), *(int *)(p + 0x4C), D_00822010,
                      D_00810700, 0x640);
    if (D_00275C58 != 0) {
        return 2;
    }
    switch (r) {
    case 0:
        return 0;
    case 1:
        return 1;
    case -1:
        return 2;
    }
    return 1;
}

// NEARMISS func_00110048  (vram 0x00110048, 0x104 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 76.20% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc forward-branch-likely wall (confirmed s84) + frame-stride wall. Body fully recovered: func_0010FAD0(a0)<0 -> 0xFFFF0000; sub__9PsIIlibkernl2000_4()!=0 -> 0xFFFEFFFC; two func_00123418 copies into a 0x1FC-byte block at D_00279448-8 with NUL terminators at +0x103/+0x1FF; func_0010E8A8(D_00279640, a3, 0, blk, blk...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_0010FAD0(int);
extern int sub__9PsIIlibkernl2000_4(void);
extern void func_00123418(char *dst, char *src, int n);
extern int func_0010E8A8(char *a, int *b, int c, void *d, void *e, int f, int g, int h, int i);

extern char D_00279448[];
extern char D_00279640[];

int func_00110048(char *a0, char *a1, int *a2, int *a3) {
    int v0;
    char *blk;

    v0 = func_0010FAD0((int)a0);
    if (v0 < 0) {
        return 0xFFFF0000;
    }
    if (sub__9PsIIlibkernl2000_4() != 0) {
        return 0xFFFEFFFC;
    }

    blk = D_00279448 - 8;
    func_00123418(D_00279448, a0, 0xFC);
    *(char *)(blk + 0x103) = 0;
    func_00123418(D_00279448 + 0xFC, a1, 0xFC);
    *(char *)(blk + 0x1FF) = 0;

    v0 = func_0010E8A8(D_00279640, a3, 0, blk, blk, 0x200, 0x10, 0, 0);
    if (v0 < 0) {
        return 0xFFFEFFFF;
    }
    if (v0 != 0) {
        a2[0] = v0;
        return 0xFFFEFFFD;
    }
    a2[1] = *(int *)(blk + 4);
    return 0;
}

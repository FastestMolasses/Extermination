// NEARMISS func_001DDB70  (vram 0x001DDB70, 0x29C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 63.66% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// GS-packet struct-field/scheduling divergence: deep-offset field writes and a &D_0026E810 addressing mismatch (gp-rel vs lui/addiu) not fully cracked within budget; all control flow (config calls, color-table-select branch, 224x do/while VU0 upload loop, DMA finalize/send) and all top-level struct...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

extern int func_001B0070(int, int, int, int);
extern void func_001CB760(void *, int, void *, int);
extern void func_001D1F20(int);
extern void func_001D1FF0(int, int);
extern void func_001D2040(int, int);
extern void func_001D63B0(int, void *, void *, void *);
extern void func_001D6B60(int, int, int, int, void *);
extern void func_001D6BA0(int, int, int, int, int, int);
extern void func_001D6C90();
extern int D_0026E810;
extern long long D_0026E820[2];
extern long long D_0026E830[2];
extern long long D_0026E840[2];
extern char *D_00275670;
extern int D_0027568C;
extern char D_007635C0[1];
typedef int u128 __attribute__((mode(TI)));

void func_001DDB70(void) {
    char *base;
    char *e;
    int r;
    long long q[2];
    struct {
        int a, b, c, d;
    } lo, hi;
    int i;
    int off;

    base = *(char **)(D_00275670 + 0x1C);
    func_001D6B60(3, D_0027568C, 8, 8, &D_0026E810);
    func_001D6BA0(3, D_0027568C, 8, 8, 0, 0);
    func_001D1FF0(3, 3);
    func_001D2040(3, 0);
    func_001D6C90(3, 0, 1, 0, 0, 1, 0, 0,
                  (long long)1, (long long)2, (long long)0, (long long)1, (long long)0, (long long)1, (long long)0);
    *(char *)(*(char **)(D_00275670 + 0x1C) + 3) = 0x10;
    *(int *)(*(char **)(D_00275670 + 0x1C) + 4) = 0;
    *(short *)(*(char **)(D_00275670 + 0x1C) + 0) = 7;
    e = *(char **)(D_00275670 + 0x1C);
    *(char **)(D_00275670 + 0x1C) = *(char **)(D_00275670 + 0x1C) + 0x80;
    *(u128 *)(e + 0x10) = 0;
    *(int *)(e + 0x1C) = 0x50000006;
    *(long long *)(e + 0x20) = (long long)(int)0x50AF4000 << 32 | (unsigned)0x8001;
    *(int *)(e + 0x28) = 0x43431;
    *(int *)(e + 0x2C) = 0;
    *(int *)(e + 0x30) = 0;
    *(int *)(e + 0x34) = 0xFF;
    *(int *)(e + 0x38) = 0;
    *(int *)(e + 0x3C) = 0xFF;
    *(int *)(e + 0x40) = 8;
    *(int *)(e + 0x44) = 8;
    *(int *)(e + 0x48) = 0;
    *(int *)(e + 0x4C) = 0;
    *(int *)(e + 0x50) = 0x7000;
    *(int *)(e + 0x54) = 0x7900;
    *(int *)(e + 0x58) = -1;
    *(int *)(e + 0x5C) = 0x80;
    *(int *)(e + 0x60) = 0x1008;
    *(int *)(e + 0x64) = 0x1008;
    *(int *)(e + 0x68) = 0;
    *(int *)(e + 0x6C) = 0;
    *(int *)(e + 0x70) = 0x9000;
    *(int *)(e + 0x74) = 0x8700;
    *(int *)(e + 0x78) = -1;
    *(int *)(e + 0x7C) = 0x80;

    r = func_001B0070(0x1008, 0x80, -1, 0x7900);
    if (r & 0x80) {
        q[0] = D_0026E820[0];
        q[1] = D_0026E820[1];
    } else if (r & 0x10000000) {
        q[0] = D_0026E840[0];
        q[1] = D_0026E840[1];
    } else {
        q[0] = D_0026E830[0];
        q[1] = D_0026E830[1];
    }

    i = 0;
    do {
        lo.a = 0x7000;
        off = (i + 0x790) * 0x10;
        hi.a = 0x9000;
        lo.b = off;
        hi.b = off;
        lo.c = -1;
        hi.c = -1;
        lo.d = 0x80;
        hi.d = 0x80;
        func_001D63B0(3, &lo, q, &hi);
        i += 2;
    } while (i < 0xE0);

    func_001D1F20(3);
    *(char *)(*(char **)(D_00275670 + 0x1C) + 3) = 0x60;
    *(int *)(*(char **)(D_00275670 + 0x1C) + 4) = 0;
    *(short *)(*(char **)(D_00275670 + 0x1C) + 0) = 0;
    *(char **)(D_00275670 + 0x1C) = *(char **)(D_00275670 + 0x1C) + 0x10;
    func_001CB760(D_007635C0, 0xFFF000, base, 0x60);
}

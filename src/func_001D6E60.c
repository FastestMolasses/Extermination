// NEARMISS func_001D6E60  (vram 0x001D6E60, 0xF8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.68% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order permutation: target materialises constants into temporaries t0-t6 and colours arg1->s2 / p->s1; mwcc packs constants into a4-a7, swaps s1/s2, and schedules the two dsra32 sign-extends one slot later. Frame size (0x40, 4 saves), 16-bit sign-extension of the shift args, an...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

extern void func_001006D8(char *, int, long long, long long);
extern int D_00275670;

char *func_001D6E60(int arg0, int arg1, int arg2, int arg3) {
    char *e;
    char *p;
    char *q;
    char **slot;

    slot = (char **)(D_00275670 + arg0 * 4);
    *(char *)(slot[4] + 3) = 0x10;
    *(int *)(slot[4] + 4) = 0;
    *(short *)(slot[4] + 0) = 0xA;
    e = slot[4];
    slot[4] = e + 0xB0;
    *(int *)(e + 0x10) = 0;
    *(int *)(e + 0x14) = 0;
    p = e + 0x10;
    *(int *)(e + 0x18) = 0x11000000;
    *(int *)(e + 0x1C) = 0x50000009;
    *(long long *)(e + 0x20) = 0x8008 | ((long long)0x10000000 << 0x20);
    *(long long *)(e + 0x28) = 0xE;
    q = p + 0x20;
    func_001006D8(q, 0, (long long)(short)(1 << arg2), (long long)(short)(1 << arg3));
    *(unsigned short *)q = (*(unsigned short *)q & ~0x1FF) | ((arg1 >> 0xD) & 0x1FF);
    return p;
}

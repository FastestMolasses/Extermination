// NEARMISS func_002081A0  (vram 0x002081A0, 0x110 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 50.85% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Pervasive register-allocation coloring + mwcc list-scheduling of the GIF-tag constant computations (permuter-class). Body offsets, __int128 sq-zero, gp-rel table base, doubleword tags, and the fill loop all match modulo register names; the original is hand-written-ASM-style packet code with a fix...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern char *D_00275670;

void func_002081A0(int arg0, char *arg1, int arg2, int arg3, int arg4) {
    short n;
    int i;
    long *q;
    char *base;
    char *pk;

    n = (arg3 >> 1) + 4;
    base = D_00275670 + arg0 * 4;
    *(char *)(*(char **)(base + 0x10) + 3) = 0x10;
    *(int *)(*(char **)(base + 0x10) + 4) = 0;
    *(short *)(*(char **)(base + 0x10) + 0) = n;
    pk = *(char **)(base + 0x10);
    *(char **)(base + 0x10) = pk + (n + 1) * 0x10;
    *(__int128 *)(pk + 0x10) = 0;
    *(int *)(pk + 0x1C) = (n - 1) | 0x50000000;
    *(long *)(pk + 0x20) = (long)0x8001 | ((long)0x24000000 << 0x20);
    *(long *)(pk + 0x28) = 0x10;
    *(long *)(pk + 0x30) = (long)arg2 | 0x140;
    *(long *)(pk + 0x38) = (unsigned int)arg4;
    *(long *)(pk + 0x40) = (long)arg3 | ((long)0x8000 | ((long)0x14000000 << 0x20));
    *(long *)(pk + 0x48) = 4;
    q = (long *)(pk + 0x50);
    i = 0;
    if (arg3 > 0) {
        do {
            i += 1;
            *q = (long)(int)(*(int *)(arg1 + 0) | (*(int *)(arg1 + 4) << 0x10)) | ((long)0xFFFFFF << 0x20);
            q += 1;
            arg1 += 0x10;
        } while (i < arg3);
    }
}

// NEARMISS func_00114708  (vram 0x00114708, 0x140 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 63.02% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// Semantics fully decoded and compile correct (D_00241D68 guard, D_0027B0C0->0x24 check returning -100, D_0027B140 config fill with the a2<0x11 vs >=0x11 alignment split, byte copy loop, DisableDmacHandler(0), func_0010E8A8(&D_0027B0C0,6,1,&D_0027B140,0x30,4,0), D_00241D68=6 on success). Two blockers: (1) frame-stride...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int DisableDmacHandler(int ch);
extern int func_0010E8A8(void *a, int b, int c, void *d, int e, int f, int g);

extern int D_00241D68;
extern unsigned char D_0027B0C0;
extern unsigned char D_0027B140;
extern unsigned char D_0027C680;

int func_00114708(int a0, unsigned char *src, int a2)
{
    int *cfg = (int *)&D_0027B140;
    int v0;
    int r;
    unsigned int i;

    v0 = D_00241D68;
    if (v0 != 0) {
        return v0;
    }

    if (*(int *)((&D_0027B0C0) + 0x24) == 0) {
        return -100;
    }

    cfg[0] = a0;                       /* D_0027B140 = a0 */

    if (a2 < 0x11) {
        cfg[5] = a2;                   /* 0x14 */
        cfg[6] = 0;                    /* 0x18 */
        cfg[3] = 0;                    /* 0xC */
    } else {
        int len = (((int)src - 1) & ~0xf) - ((int)src - 16);
        cfg[6] = (int)src + len;       /* 0x18 */
        cfg[3] = a2 - len;             /* 0xC */
        cfg[5] = len;                  /* 0x14 */
    }

    if (cfg[5] != 0) {                 /* 0x14 */
        i = 0;
        do {
            ((unsigned char *)cfg)[0x20 + i] = src[i];
            i++;
        } while (i < (unsigned int)cfg[5]);
    }

    DisableDmacHandler(0);
    r = func_0010E8A8(&D_0027B0C0, 6, 1, &D_0027B140, 0x30, 4, 0);
    if (r != 0) {
        return r;
    }
    D_00241D68 = 6;
    return r;
}

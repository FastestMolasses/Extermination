// NEARMISS func_001F1AA0  (vram 0x001F1AA0, 0x328 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 76.06% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Full logic recovered: the FPU-MAC dot-product (dx*dx+dy*dy+dz*dz) auto-fuses to adda.s/madd.s under mwcc233 exactly as in the func_001545B0 precedent; the 12-iteration particle-spawn loop is a classic LCG (seed = seed*37+11) driving 5 derived fields (angle mod-128 signed, index mod-48 via div, tw...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void copy_qw4(void *dst, void *src);
extern int float_to_int(float a);
extern void func_001026A0(void *dst, void *matrix, void *vec);
extern void func_001028D0(void *dst, void *a, void *b);
extern float func_0011E748(float a);
extern int func_00122BB8(void);
extern void func_001AFC10(void *a);
extern void func_001CD940(int mode, void *a, void *b, void *c, void *d);

extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700038D0[];

void func_001F1AA0(char *arg0) {
    unsigned char st;
    char *self;
    int seed, seed2, seed3;
    int i;
    int hi;
    float dx, dy, dz, d2;
    int amt;

    st = *(unsigned char *)(arg0 + 4);
    self = arg0 + 0x1F0;
    switch (st) {
    case 0:
        copy_qw4(self, *(char **)(arg0 + 0x24));
        *(char **)(self + 0x40) = *(char **)(arg0 + 0x24);
        *(int *)(self + 0x44) = func_00122BB8();
        *(unsigned char *)(arg0 + 4) = 1;
        /* fallthrough */
    case 1:
        func_001028D0(D_700038A0, self + 0x30, *(char **)(self + 0x40) + 0x30);
        dx = *(float *)(D_700038A0 + 0);
        dy = *(float *)(D_700038A0 + 4);
        dz = *(float *)(D_700038A0 + 8);
        d2 = func_0011E748(dx * dx + dy * dy + dz * dz);
        *(float *)0x70003A20 = d2;
        if (!(d2 <= 7.0f)) {
            *(float *)0x70003A20 = 7.0f;
            *(unsigned char *)(arg0 + 4) = 3;
        }
        d2 = *(float *)0x70003A20;
        if (!(d2 <= 4.0f)) {
            *(float *)0x700038A4 = 0.0f;
            amt = float_to_int(128.0f * (1.0f - ((d2 - 4.0f) / 3.0f)));
        } else {
            *(float *)0x700038A4 = 0.0f;
            amt = 0x80;
        }
        *(int *)0x700038AC = amt;
        *(int *)0x700038B4 = 0;
        *(float *)0x700038BC = 1.0f;
        seed = *(int *)(self + 0x44);
        i = 0;
        do {
            seed2 = seed * 37 + 11;
            *(int *)0x700038A0 = ((seed >> 16) & 0xFFFF) % 128;
            seed3 = seed2 * 37 + 11;
            hi = (int)((unsigned int)seed2 >> 16);
            *(int *)0x700038A8 = hi % 48;
            hi = (int)((unsigned int)seed3 >> 16);
            *(float *)0x700038B0 = ((float)hi / 65535.0f + 0.0001f) - 0.5f;
            seed = seed3 * 37 + 11;
            hi = (int)((unsigned int)seed >> 16);
            *(float *)0x700038B8 = 2.4f * ((float)hi / 65535.0f + 0.0001f) - 1.2f;
            seed = seed * 37 + 11;
            func_001026A0(D_700038C0, self, D_700038B0);
            func_001026A0(D_700038D0, *(char **)(self + 0x40), D_700038B0);
            func_001CD940(1, D_700038C0, D_700038A0, D_700038D0, D_700038A0);
            i += 1;
        } while (i < 0xC);
        return;
    case 2:
    case 3:
        func_001AFC10(arg0);
        return;
    }
}

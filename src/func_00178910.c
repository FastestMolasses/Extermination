// NEARMISS func_00178910  (vram 0x00178910, 0x27C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.67% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring permutation of the loop-pointer set (flags/tsin/tpos/i shifted by one register vs target) plus one FP-coloring artifact on the 4.0f scale constant. Not the clean-store delay-slot nop; 2.3.3 does not close it. Permuter/regalloc territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern float func_0011DF78(float a);
extern float func_0011E620(float y, float x);
extern int func_0019BC40(void *p);
extern float func_001B1470(float a);

extern float D_00282250;
extern float D_700030F0;
extern unsigned short D_70003170;
extern float D_700038B0;

int func_00178910(char *arg0, int arg1) {
    char *p;
    unsigned short *flags;
    float *tsin;
    float *tpos;
    int i;
    float d;

    if (*(unsigned char *)(arg0 + 0x316) != 0) {
        char *cam1 = *(char **)0x700031D0;
        *(float *)0x700038B0 = *(float *)0x700031B0 - (4.0f * *(float *)(cam1 + 0x24));
        *(float *)0x700038B8 = *(float *)0x700031B8 - (4.0f * *(float *)(cam1 + 0x2C));
    } else {
        char *cam2 = *(char **)0x700031D0;
        *(float *)0x700038B0 = *(float *)0x700031B0 - *(float *)(cam2 + 0x24);
        *(float *)0x700038B8 = *(float *)0x700031B8 - *(float *)(cam2 + 0x2C);
    }
    *(float *)0x700038B4 = 20.5f + *(float *)(arg0 + 0xB4);
    *(int *)0x700038BC = 0x3F800000;
    func_0019BC40(&D_700038B0);

    i = 0;
    flags = &D_70003170;
    tsin = &D_00282250;
    tpos = &D_700030F0;
    goto guard;
    for (;;) {
        if ((*flags & 1) && *tsin < 0.62831855f) {
            d = func_0011DF78(*(float *)0x700038B4 - *tpos);
            *(float *)0x70003A20 = d;
            if (d < 1.0f) {
                if (arg1 != 0) {
                    p = *(char **)0x700031D0;
                    *(float *)(arg0 + 0x2E0) = *(float *)0x700031B0 + 1.5f * *(float *)(p + 0x24);
                    *(float *)(arg0 + 0x2E8) = *(float *)0x700031B8 + 1.5f * *(float *)(*(char **)0x700031D0 + 0x2C);
                    *(float *)(arg0 + 0x2E4) = (&D_700030F0)[i] - 20.5f;
                    p = *(char **)0x700031D0;
                    *(float *)0x70003A20 = func_0011E620(-*(float *)(p + 0x2C), *(float *)(p + 0x24));
                    *(float *)(arg0 + 0x218) = func_001B1470(4.712389f + *(float *)0x70003A20);
                }
                return 1;
            }
        }
        flags += 1;
        tsin += 1;
        tpos += 1;
        i++;
    guard:
        if (i >= *(int *)0x700031E0) {
            return 0;
        }
    }
}

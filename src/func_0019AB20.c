// NEARMISS func_0019AB20  (vram 0x0019AB20, 0x1E0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.99% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Corrected body (func_0019F730 takes no arguments). Left: idiom-32 scratchpad lui delay-slot fills and part of the loop-temp coloring.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// CORRECTED (decomp lane round 5, 2026-10-20): func_0019F730 takes no arguments
// (its byte-matched definition is (void) and the target sets none; the old C passed
// the three loop cursors). Locals reordered by a declaration-order sweep.

extern int func_0019F730(void);
extern int func_0019C830(void);
extern char D_70003190[];

int func_0019AB20(char *arg0, float *arg1, char *arg2, int arg3) {
    float f20;
    float sp60[3];
    int i;
    int v0;
    float *a0;
    int s0;
    float *a3;
    float *s1;
    float *a1;

    s0 = 0;
    a1 = arg1;
    a0 = (float *)D_70003190;
    s1 = sp60;
    i = 0;
    a3 = s1;
    do {
        float t = *a1;
        i += 1;
        *(float *)((char *)a0 + 0x10) = t;
        *a0 = t;
        *a3 = t;
        a1 += 1;
        a0 += 1;
        a3 += 1;
    } while (i < 3);
    *(float *)0x70003194 = *(float *)0x70003194 - *(float *)(arg2 + 4);
    if (*(float *)(arg2 + 4) < 0.0f) {
        f20 = 0.001f;
    } else {
        f20 = -0.001f;
    }
    *(float *)0x70003194 = *(float *)0x70003194 + f20;
    *(int *)0x700031AC = 0;
    *(int *)0x7000319C = 0;
    *(int *)0x700031D4 = 0;
    *(short *)0x7000324E = *(unsigned char *)(arg0 + 2) & 0x1F;
    if (arg3 & 2) {
        *(int *)0x70003254 = *(int *)(arg0 + 0x14);
        if (func_0019F730() == 0) {
            s0 = 2;
        }
    }
    if ((arg3 & 4) && func_0019C830() == 0) {
        s0 = 4;
    }
    *(float *)0x70003194 = *(float *)0x70003194 - f20;
    if (s0 != 0) {
        float *v1 = (float *)D_70003190;
        i = 0;
        do {
            i += 1;
            *(float *)((char *)v1 + 0x10) = *s1;
            s1 += 1;
            *(float *)((char *)v1 + 0x30) = *(float *)((char *)v1 + 0x20) - *(float *)((char *)v1 + 0x10);
            v1 += 1;
        } while (i < 3);
        if (arg3 & 0x80000000) {
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + *(float *)0x700031C4;
        }
    } else {
        *(int *)0x700031D0 = 0;
    }
    *(int *)0x700031D8 = s0;
    return s0;
}

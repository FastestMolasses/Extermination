// NEARMISS func_0019B8C0  (vram 0x0019B8C0, 0x1B4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.32% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation / instruction-scheduling permutation. Body+structure fully recovered (matches in instruction count and shape). Residuals: first/second loop-pointer coloring (target a3/v1/t0 walker/counter/sp-ptr vs mwcc v0/a3/v1), s1(&sp) prologue setup ordering relative to `sq s0`, and a bra...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_0019E640(void);
extern int func_001A32C0(void);
extern float D_70003190[];

int func_0019B8C0(unsigned char *arg0, float *arg1, unsigned char *arg2, int arg3) {
    float sp[4];
    float *src;
    float *g;
    float *st;
    float *stbase;
    float f20;
    int s0;
    int i;
    float t;

    src = arg1;
    stbase = &sp[0];
    g = &D_70003190[0];
    s0 = 0;
    i = 0;
    st = stbase;
    do {
        t = *src;
        i += 1;
        g[4] = t;
        g[0] = t;
        *st = t;
        src += 1;
        g += 1;
        st += 1;
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
    *(short *)0x7000324E = arg0[2] & 0x1F;

    if (arg3 & 2) {
        *(int *)0x70003254 = *(int *)(arg0 + 0x14);
        if (func_001A32C0() == 0) {
            s0 = 2;
        }
    }
    if ((arg3 & 4) && func_0019E640() == 0) {
        s0 = 4;
    }

    *(float *)0x70003194 = *(float *)0x70003194 - f20;
    if (s0 != 0) {
        g = &D_70003190[0];
        st = stbase;
        i = 0;
        do {
            i += 1;
            g[4] = *st;
            st += 1;
            g[12] = g[8] - g[4];
            g += 1;
        } while (i < 3);
    } else {
        *(int *)0x700031D0 = 0;
    }
    *(int *)0x700031D8 = s0;
    return s0;
}

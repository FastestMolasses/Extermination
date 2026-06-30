// NEARMISS func_0019A440  (vram 0x0019A440, 0x124 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 83.41% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// GPR register-coloring permutation: target colors ret=$s0 / buf-base=$s1, my build swaps to ret=$s1 / buf-base=$s0, cascading through every saved-reg reference; the loop-counter compare also lands in a different temp ($at vs $v0). Body, control flow, frame size (0x50), saved regs ($s0/$s1/$s2 + ra...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern float D_70003190[];

int func_0019A440(float *a0, float *a1, int arg2) {
    float sp40[4];
    float *buf;
    float *d;
    float *t0;
    int ret;
    int i;
    int m;
    float tf;

    buf = sp40;
    ret = 0;
    d = &D_70003190[0];
    i = 0;
    t0 = buf;
    do {
        i += 1;
        d[0] = *a0;
        tf = *a1;
        a0 += 1;
        *t0 = tf;
        d[4] = tf;
        d += 1;
        a1 += 1;
        t0 += 1;
    } while (i < 3);
    *(int *)0x700031AC = 0x3F800000;
    *(int *)0x7000319C = 0x3F800000;
    m = arg2 & 0xFF;
    *(int *)&sp40[3] = 0x3F800000;
    *(int *)0x700031D4 = 0;
    if (m & 2) {
        *(int *)0x70003254 = 0;
        if (func_001A1B80(a0, a1, d) != 0) {
            ret = 2;
        }
    }
    if ((m & 4) && func_0019DB50() != 0) {
        ret = 4;
    }
    if (ret != 0) {
        float *d2 = &D_70003190[0];
        i = 0;
        do {
            i += 1;
            d2[4] = *buf;
            buf += 1;
            d2 += 1;
        } while (i < 3);
    } else {
        *(int *)0x700031D0 = 0;
    }
    *(int *)0x700031D8 = ret;
    return ret;
}

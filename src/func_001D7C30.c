// NEARMISS func_001D7C30  (vram 0x001D7C30, 0x370 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.53% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring (fv0/fv1/ft0 assignment permutation) plus a mwcc-vs-CW branch-lowering/tail-duplication difference in the shared func_001029C0(s1+0x40) call site (target reaches it via 3 converging branches lowered differently than mwcc's merge). Body/control-flow and every constant fully re...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

#define RNORM 4.656612873e-10f

extern void func_00102900(void *dst, void *src, float s);
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(void *a0);
extern void func_00102B08(void *dst, void *src, float v);
extern void func_00102BB0(void *dst, void *src, float v);
extern int func_00122BB8(void);
extern unsigned char *D_00275670;
extern unsigned char D_00810700[4];
extern unsigned char D_00810701[4];

void func_001D7C30(void) {
    unsigned char *e;
    unsigned char *s1;
    float blend;
    float f;
    int i;
    int off;
    int count;
    float scratch[4];

    i = 0;
    off = 0;
    do {
        e = D_00275670 + off;
        blend = *(float *)(e + 0x24C);
        s1 = e + 0x220;
        if (blend > 0.0f) {
            f = *(float *)(s1 + 4) + blend * *(float *)s1;
            if (f < 0.0f) {
                f = 0.0f;
            }
            *(float *)(s1 + 0x2C) = f;
            *(float *)(s1 + 0x20) = *(float *)(s1 + 0x20) * *(float *)s1;
            *(float *)(s1 + 0x24) = *(float *)(s1 + 0x24) * *(float *)s1;
            *(float *)(s1 + 0x28) = *(float *)(s1 + 0x28) * *(float *)s1;
            if (*(float *)s1 != 1.0f || ((D_00810700[0] << 8) + D_00810701[0]) == 0xF00) {
                func_001029C0(s1 + 0x40);
            } else if (*(int *)(s1 + 8) == 1) {
                float rx = *(float *)(s1 + 0x30) + (-0.001f + 0.002f * ((float)func_00122BB8() * RNORM));
                if (rx < -0.03141593f) {
                    rx = -0.03141593f;
                }
                if (rx > 0.03141593f) {
                    rx = 0.03141593f;
                }
                *(float *)(s1 + 0x30) = rx;
                {
                    float ry = *(float *)(s1 + 0x34) + (-0.001f + 0.002f * ((float)func_00122BB8() * RNORM));
                    if (ry < -0.03141593f) {
                        ry = -0.03141593f;
                    }
                    if (ry > 0.03141593f) {
                        ry = 0.03141593f;
                    }
                    *(float *)(s1 + 0x34) = ry;
                }
                func_001029C0(scratch);
                func_00102B08(scratch, scratch, *(float *)(s1 + 0x30));
                func_00102BB0(s1 + 0x40, scratch, *(float *)(s1 + 0x34));
            } else {
                func_001029C0(s1 + 0x40);
            }
        }
        i += 1;
        off += 0x80;
    } while (i < 0x20);

    count = *(int *)(D_00275670 + 0x214);
    if (count != 0) {
        if (count > 0) {
            int n;
            int off2;
            unsigned char *src;

            n = 0;
            off2 = 0;
            do {
                unsigned char *p;
                int j;

                p = D_00275670;
                j = 0;
                src = D_00275670 + off2 + 0x1220;
                for (;;) {
                    s1 = p + 0x220;
                    if (*(float *)(p + 0x24C) <= 0.0f) {
                        *(float *)(s1 + 0) = *(float *)(src + 0);
                        *(float *)(s1 + 4) = *(float *)(src + 4);
                        func_00102948(s1 + 0x10, src + 0x10);
                        func_00102900(s1 + 0x20, src + 0x20, 128.0f);
                        *(int *)(s1 + 0xC) = *(int *)(src + 0xC);
                        *(int *)(s1 + 8) = *(int *)(src + 8);
                        *(int *)(s1 + 0x30) = 0;
                        *(int *)(s1 + 0x34) = 0;
                        *(int *)(s1 + 0x38) = 0;
                        break;
                    }
                    j += 1;
                    p += 0x80;
                    if (j >= 0x20) {
                        break;
                    }
                }
                n += 1;
                off2 += 0x80;
            } while (n < count);
        }
        *(int *)(D_00275670 + 0x214) = 0;
    }
}

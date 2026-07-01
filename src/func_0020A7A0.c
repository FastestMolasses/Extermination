// NEARMISS func_0020A7A0  (vram 0x0020A7A0, 0x4C8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.67% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order permutation. Target keeps the per-slot pointer in s5 and the loop index in s1; mwcc233 places the same two live values in s1/s2 (permuted numbers, identical lifetimes). All three switch(i) dispatch shapes (check order 2, then 1, then implicit-0 fallback via beqz), the cl...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_00207D00(int a0, int a1);
extern void func_00207E40(int a0, int a1, int a2, int a3, int t0, unsigned int t1, int t2);
extern int func_00122BB8(void);
extern int func_00128250(float a);
extern int float_to_int(float a);
extern float func_0011E2A8(float a);

extern char D_002655A0[];

void func_0020A7A0(int arg0) {
    char *e;
    int i;
    int inner;
    int outer;
    int color;
    float f0;
    float f1;
    float f20;
    float f21;

    func_00207D00(1, 0);

    e = D_002655A0;
    for (i = 0; i < 3; i++, e += 0x20) {
        switch (i) {
        case 0:
            break;
        case 1:
        case 2:
            if (*(int *)(e + 0xC) < 0) {
                f1 = *(float *)(e + 8) + 0.25f;
                *(float *)(e + 8) = f1;
                if (!(f1 < 180.0f)) {
                    *(int *)(e + 0xC) = ((func_00122BB8() % 60) * 3) + 0x3C;
                    *(float *)(e + 8) = 0.0f;
                    *(float *)(e + 0) = 0.0f;
                    *(float *)(e + 4) = 0.0f;
                }
            } else {
                *(int *)(e + 0xC) = *(int *)(e + 0xC) - 1;
            }
            break;
        }

        switch (i) {
        case 0:
            f1 = *(float *)(e + 0) - 0.5f;
            if (f1 < 0.0f) {
                f1 = 255.0f;
            }
            *(float *)(e + 0) = f1;
            *(float *)(e + 0x10) = 96.0f;
            *(float *)(e + 0x14) = 96.0f;
            *(float *)(e + 0x18) = 96.0f;
            *(float *)(e + 0x1C) = 64.0f;
            f0 = *(float *)(e + 0x1C) * func_0011E2A8((3.1415927f * *(float *)(e + 8)) / 180.0f);
            *(float *)(e + 0x1C) = f0;
            break;
        case 1:
            f1 = *(float *)(e + 4) - 0.5f;
            if (f1 < 0.0f) {
                f1 = 255.0f;
            }
            *(float *)(e + 4) = f1;
            *(float *)(e + 0x10) = 96.0f;
            *(float *)(e + 0x14) = 96.0f;
            *(float *)(e + 0x18) = 96.0f;
            *(float *)(e + 0x1C) = 64.0f;
            f0 = *(float *)(e + 0x1C) * func_0011E2A8((3.1415927f * *(float *)(e + 8)) / 180.0f);
            *(float *)(e + 0x1C) = f0;
            break;
        case 2:
            if (*(int *)(e + 0xC) < 0) {
                *(float *)(e + 0) += 0.3f;
                *(float *)(e + 4) += 0.3f;
                *(float *)(e + 0x10) = 96.0f;
                *(float *)(e + 0x14) = 96.0f;
                *(float *)(e + 0x18) = 96.0f;
                *(float *)(e + 0x1C) = 64.0f;
                f0 = *(float *)(e + 0x1C) * func_0011E2A8((3.1415927f * *(float *)(e + 8)) / 180.0f);
                *(float *)(e + 0x1C) = f0;
            }
            break;
        }

        switch (i) {
        case 0:
        case 1:
            outer = 0x400;
            do {
                inner = 0x400;
                do {
                    color = func_00128250(*(float *)(e + 0x10));
                    color |= func_00128250(*(float *)(e + 0x14)) << 8;
                    color |= func_00128250(*(float *)(e + 0x18)) << 0x10;
                    color |= func_00128250(*(float *)(e + 0x1C)) << 0x18;
                    func_00207E40(1, (inner + float_to_int(*(float *)(e + 0))) << 4,
                                  (outer + float_to_int(*(float *)(e + 4))) << 4,
                                  0x100, 0x80, color, arg0);
                    inner += 0x100;
                } while (inner < 0xC00);
                outer += 0x40;
            } while (outer < 0xC00);
            break;
        case 2:
            if (*(int *)(e + 0xC) < 0) {
                f20 = *(float *)(e + 0);
                f21 = *(float *)(e + 4);
                outer = float_to_int(16.0f * (1792.0f - f20));
                inner = float_to_int(16.0f * (1936.0f + (-f21 / 2.0f)));
                color = func_00128250(*(float *)(e + 0x10));
                color |= func_00128250(*(float *)(e + 0x14)) << 8;
                color |= func_00128250(*(float *)(e + 0x18)) << 0x10;
                color |= func_00128250(*(float *)(e + 0x1C)) << 0x18;
                func_00207E40(1, outer, inner,
                              float_to_int(2.0f * f20) + 0x200,
                              float_to_int(2.0f * f21) + 0x1C0,
                              color, arg0);
            }
            break;
        }
    }
}

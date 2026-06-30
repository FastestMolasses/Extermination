// NEARMISS func_00107098  (vram 0x00107098, 0xE0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 70.09% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc register-allocation near-miss (permuter territory). Body logic decompiled correctly and verified: the if/else control flow, the D_002411D4 toggle + D_00241198+=0x400, the slt(a0<v1) D_002411A0-gate setting D_002411D4=1, the D_00241538=D_00241198+D_00241478 sum, the D_002411D8<D_00241478 path, and the final max...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_0024147C;
extern int D_00241478;
extern int D_002411D8;
extern int D_002411D4;
extern int D_00241198;
extern int D_0024119C;
extern int D_002411A0;
extern int D_00241538;
extern int D_00240400;

void func_00107098(void)
{
    int a0, v3;

    if (D_0024147C != 3) {
        a0 = D_00241478;
        v3 = D_002411D8;
        if (a0 != v3) {
            if (D_002411D4 != 0) {
                D_002411D4 = 0;
                D_00241198 += 0x400;
            }
            if (a0 < v3) {
                if (D_002411A0 == 0)
                    D_002411D4 = 1;
            }
            D_002411A0 = 0;
            D_002411D8 = D_00241478;
        }
    }

    {
        int sum = D_00241198 + D_00241478;
        D_00241538 = sum;
        if (D_002411D4 != 0) {
            if (D_002411D8 < D_00241478) {
                D_00241538 = (int)&D_00240400;
            }
        }
        {
            int cur = D_00241538;
            if (D_0024119C < cur)
                D_0024119C = cur;
        }
    }
}

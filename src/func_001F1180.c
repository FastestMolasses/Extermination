// NEARMISS func_001F1180  (vram 0x001F1180, 0x3C4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.62% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Deep register-allocation / block-layout divergence in the state-machine dispatch + GS-packet render tail. IMPORTANT FIX: the prior decoded C had the v1==0/v1==1 branch polarity INVERTED (a genuine logic bug, not just a codegen artifact) -- target's asm shows 'beq v1,1,STATE_SELECTOR' (v1==1 jumps...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001026A0(float *a0, char *a1, float *a2);
extern float func_00102738(float *a0, float *a1);
extern void func_00102760(float *a0, float *a1);
extern void func_001028D0(float *a0, char *a1, void *a2);
extern float func_0011E2A8(int a0, float f);
extern int func_00122BB8(void);
extern int float_to_int(float f);
extern void func_001F0A60(int a0, int a1, float *a2, int a3, int t0, float f12, float f13, float f14);
extern char D_00259DD0[];
extern char D_00259EE0[];
extern char D_00259F90[];
extern char D_0025A040[];
extern char D_008105D0[];
extern unsigned char D_00810700;

void func_001F1180(char *arg0) {
    char *s3;
    char *s2;
    int v1;
    short v1_2;

    s3 = arg0 + 0x2D0;
    v1 = *(int *)(arg0 + 0x2DC);
    if (v1 != 1) {
        if (v1 == 0) {
            float f1 = *(float *)(s3 + 4) - 1.0f;
            *(float *)(s3 + 4) = f1;
            if (f1 < 0.0f) {
                int r;
                *(float *)(s3 + 4) = 0.0f;
                r = func_00122BB8() % 4;
                *(short *)(s3 + 0xA) = (short) r;
                *(int *)(s3 + 0xC) = 1;
            }
        }
        return;
    }

    v1_2 = *(short *)(s3 + 8);
    if (v1_2 != 1) {
        s2 = D_00259DD0 + v1_2 * 0x2C;
    } else if (D_00810700 != 2) {
        if (D_00810700 != 1) {
            s2 = D_00259EE0 + (*(short *)(s3 + 0xA)) * 0x2C;
        } else {
            s2 = D_00259F90 + (*(short *)(s3 + 0xA)) * 0x2C;
        }
    } else {
        s2 = D_0025A040 + (*(short *)(s3 + 0xA)) * 0x2C;
    }

    if (v1_2 != 4 && v1_2 != 2 && v1_2 != 1) {
        float sp80[1];
        float sp70[4];
        func_001028D0((float *)sp80, arg0 + 0x100, D_008105D0);
        func_00102760((float *)sp80, (float *)sp80);
        func_00102760(sp70, (float *)(arg0 + 0xF0));
        if (func_00102738(sp70, (float *)sp80) <= 0.0f) {
            goto skip;
        }
    }
    {
        float ang = *(float *)(s3 + 4);
        float f0 = func_0011E2A8(1, 3.1415927f * ang);
        float f21 = *(float *)(s2 + 0x24) * f0;
        int s1 = *(int *)(s2 + 0xC) | (*(int *)(s2 + 0x10) << 8) | (*(int *)(s2 + 0x14) << 0x10);
        int a1v = *(int *)(s2 + 4) << 8;
        int a0v = *(int *)(s2 + 8) << 0x10;
        int s0 = *(int *)(s2 + 0) | a1v | a0v;
        int s0_2 = s0 | (float_to_int(128.0f * f0) << 0x18);
        int s1_2 = s1 | (float_to_int(16.0f * f0) << 0x18);
        float sp70[4];
        sp70[0] = *(float *)(s2 + 0x18);
        sp70[1] = *(float *)(s2 + 0x1C);
        sp70[2] = *(float *)(s2 + 0x20);
        *(int *)&sp70[3] = 0x3F800000;
        func_001026A0(sp70, arg0 + 0xD0, sp70);
        func_001F0A60(0, 1, sp70, s0_2, s1_2, (3.1415927f * (*(float *)(arg0 + 0x2D0))) / 180.0f, f21, *(float *)(s2 + 0x28));

        {
            float f1_2 = *(float *)(arg0 + 0x2D0) + 6.0f;
            *(float *)(arg0 + 0x2D0) = f1_2;
            if (!(f1_2 <= 180.0f)) {
                *(float *)(arg0 + 0x2D0) = *(float *)(arg0 + 0x2D0) - 360.0f;
            }
        }
        {
            float f1_3 = *(float *)(s3 + 4) + 0.05f;
            *(float *)(s3 + 4) = f1_3;
            if (!(f1_3 <= 1.0f)) {
                *(float *)(s3 + 4) = 40.0f + (float) (func_00122BB8() % 120);
skip:
                *(int *)(s3 + 0xC) = 0;
            }
        }
    }
}

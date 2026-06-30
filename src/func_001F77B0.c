// NEARMISS func_001F77B0  (vram 0x001F77B0, 0x540 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 77.13% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Callee-saved register-allocation permutation. The persistent work pointer (s0+0x1F0) lands in $s2 in the target but $s1 in mwcc (off-by-one), cascading through every $s1/$s2/$s3/$s4 reference in both loops plus an FP-coloring shift. Logic verified instruction-by-instruction: per-kind param table ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 77.1% (mwcc 2.3.3). Logic fully recovered (verified instruction by
// instruction: scratchpad 0x70003A20/24 temporaries, both loops, the 4-way
// corner switch, func_001CE300 GIF submit, life clamp all match). Residual is
// a callee-saved register-allocation permutation: the work pointer s0+0x1F0
// lands in $s2 in the target but $s1 in mwcc (off-by-one), cascading through
// every $s1/$s2/$s3/$s4 + FP-coloring reference. Permuter territory.
//
// SEMANTICS: spawn/animate a radial debris burst attached to the actor.
// State (+0x04): 0 INIT (pick per-kind params by self[0xD], scatter N
// particles on a disc), 1 ANIMATE (per-particle build 4 corner verts,
// transform via func_001CE300, advance life), 2/3 -> func_001AFC10 teardown.
extern float D_00810360;
extern float D_00810364;
extern float D_00810368;
extern int func_00122BB8(void);
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern void func_001CE300(int a, void *verts, long long gif, int b);
extern void func_001AFC10(unsigned char *self);

void func_001F77B0(unsigned char *s0) {
    unsigned char *s2 = s0 + 0x1F0;
    int s1, s3, s4;
    float f20, f21, f22;
    float verts[16];

    switch (s0[4]) {
    case 3:
    case 2:
        func_001AFC10(s0);
        return;
    case 1:
        break;
    case 0:
        switch (s0[0xD]) {
        case 2:
            *(int *)(s2 + 0xA4) = 4;
            *(int *)(s2 + 0xA8) = 0;
            *(float *)(s2 + 0xB8) = 210.0f;
            *(float *)(s2 + 0xBC) = 7.0f;
            *(float *)(s2 + 0xC0) = 2.0f;
            *(long long *)(s2 + 0xB0) = 0x2004108555322080LL;
            *(int *)(s2 + 0xAC) = 0x80020220;
            break;
        case 3:
            *(int *)(s2 + 0xA4) = 8;
            *(int *)(s2 + 0xA8) = 4;
            *(float *)(s2 + 0xB8) = 1.0f;
            *(float *)(s2 + 0xBC) = 7.0f;
            *(float *)(s2 + 0xC0) = 5.0f;
            *(long long *)(s2 + 0xB0) = 0x2004108555322080LL;
            *(int *)(s2 + 0xAC) = 0x80200220;
            break;
        case 4:
            *(int *)(s2 + 0xA4) = 8;
            *(int *)(s2 + 0xA8) = 8;
            *(float *)(s2 + 0xB8) = 1.0f;
            *(float *)(s2 + 0xBC) = 15.0f;
            *(float *)(s2 + 0xC0) = 10.0f;
            *(long long *)(s2 + 0xB0) = 0x2004108555322080LL;
            *(int *)(s2 + 0xAC) = 0x80200220;
            break;
        case 5:
            *(int *)(s2 + 0xA4) = 8;
            *(int *)(s2 + 0xA8) = 8;
            *(float *)(s2 + 0xB8) = 1.0f;
            *(float *)(s2 + 0xBC) = 15.0f;
            *(float *)(s2 + 0xC0) = 10.0f;
            *(long long *)(s2 + 0xB0) = 0x2004108555322080LL;
            *(int *)(s2 + 0xAC) = 0x80200220;
            break;
        default:
            break;
        }
        {
            unsigned char *p0 = s2;
            unsigned char *p80 = s2;
            for (s1 = 0; s1 < *(int *)(s2 + 0xA4); s1++) {
                float az = 6.2831855f * ((float)func_00122BB8() / 2147483648.0f) - 3.1415927f;
                float r;
                *(float *)0x70003A20 = az;
                r = (float)func_00122BB8() / 2147483648.0f;
                *(float *)0x70003A24 = r;
                *(float *)(p0 + 0) = *(float *)0x70003A24 * (*(float *)(s2 + 0xC0) * func_0011E2A8(*(float *)0x70003A20));
                *(float *)(p0 + 8) = *(float *)0x70003A24 * (*(float *)(s2 + 0xC0) * func_0011DE90(*(float *)0x70003A20));
                *(int *)(p80 + 0x80) = 0;
                p0 += 0x10;
                p80 += 4;
            }
            *(int *)(s2 + 0xA0) = func_00122BB8();
            s0[4] = 1;
        }
        break;
    default:
        return;
    }

    f20 = D_00810360 - *(float *)(s0 + 0xC0);
    f21 = D_00810364 - *(float *)(s0 + 0xC4);
    f22 = D_00810368 - *(float *)(s0 + 0xC8);

    {
        unsigned char *s1p = s2;
        unsigned char *s3p = s2;
        for (s4 = 0; s4 < *(int *)(s2 + 0xA4); s4++) {
            float *t0 = verts;
            float yoff = f21 - 1.5f;
            int a3;
            for (a3 = 0; a3 < 4; a3++) {
                float d;
                t0[0] = *(float *)(s0 + 0xB0) + *(float *)(s1p + 0);
                t0[1] = *(float *)(s0 + 0xB4);
                t0[2] = *(float *)(s0 + 0xB8) + *(float *)(s1p + 8);
                t0[3] = 1.0f;
                if (s0[0xD] == 2) {
                    t0[0] = t0[0] + f20;
                    t0[1] = t0[1] + yoff;
                    t0[2] = t0[2] + f22;
                }
                d = *(float *)(s2 + 0xBC) * *(float *)(s3p + 0x80);
                if (a3 == 3) {
                    t0[0] = t0[0] + d;
                    t0[2] = t0[2] + d;
                } else if (a3 == 2) {
                    t0[0] = t0[0] - d;
                    t0[2] = t0[2] + d;
                } else if (a3 == 1) {
                    t0[0] = t0[0] + d;
                    t0[2] = t0[2] - d;
                } else if (a3 == 0) {
                    t0[0] = t0[0] - d;
                    t0[2] = t0[2] - d;
                }
                t0 += 4;
            }
            func_001CE300(1, verts, *(long long *)(s2 + 0xB0), *(int *)(s2 + 0xAC));
            {
                float nl = *(float *)(s3p + 0x80) + 1.0f / *(float *)(s2 + 0xB8);
                *(float *)(s3p + 0x80) = nl;
                if (!(nl <= 1.0f)) {
                    *(float *)(s3p + 0x80) = 1.0f;
                }
            }
            s1p += 0x10;
            s3p += 4;
        }
    }
}

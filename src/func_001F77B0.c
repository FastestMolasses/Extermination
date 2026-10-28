// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 77.13% NEARMISS).
// Fixes read from the original instructions:
//  - The state dispatch is one switch in source order 0,1,2,3: state 0 falls
//    through into state 1 (the animate pass runs in the init frame too), and
//    states 2/3 call func_001AFC10 and break; any other state returns.
//  - The per-corner offset is a switch on the corner index (cases 0..3, each
//    ending in break) that re-reads +0xBC and the particle life for every
//    component; no shared product local.
//  - The scratchpad temporaries 0x70003A20 / 0x70003A24 are relocated externs
//    (idiom-32), so the angle reload is scheduled before the radius store.
//  - The "off-by-one saved register" was declaration order: all loop locals
//    declared at the top in the order (pos16, counter, work, pos16b, life4,
//    life4b, counter2); loop counters are initialised before the pointers in
//    the for-init; the inner loop declares the corner counter first.
//  - The life advance is `+=` with a `> 1.0f` clamp.
//
// Spawn/animate a radial debris burst attached to the actor. Work block at
// self+0x1F0. State (+0x04): 0 INIT (pick per-kind params by self[0xD]:
// count +0xA4, +0xA8, life rate +0xB8, size +0xBC, radius +0xC0, GIF tag
// +0xB0, colour +0xAC; scatter the particles on a disc using two random
// draws per particle and the sin/cos helpers; seed +0xA0; go to state 1),
// 1 ANIMATE (per particle build 4 corner verts around the actor position,
// offset by the camera delta for kind 2, submit through func_001CE300 and
// advance the particle life, clamped to 1.0), 2/3 -> func_001AFC10 teardown.
extern float D_70003A20;
extern float D_70003A24;
extern float D_00810360;
extern float D_00810364;
extern float D_00810368;
extern int func_00122BB8(void);
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern void func_001CE300(int a, void *verts, long long gif, int b);
extern void func_001AFC10(unsigned char *self);

void func_001F77B0(unsigned char *s0) {
    unsigned char *s1p;
    int s1;
    unsigned char *s2;
    unsigned char *p0;
    unsigned char *s3p;
    unsigned char *p80;
    int s4;
    float f20, f21, f22;
    float verts[16];

    s2 = s0 + 0x1F0;

    switch (s0[4]) {
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
        }
        {
            for (s1 = 0, p0 = s2, p80 = s2; s1 < *(int *)(s2 + 0xA4); s1++) {
                D_70003A20 = 6.2831855f * ((float)func_00122BB8() / 2147483648.0f) - 3.1415927f;
                D_70003A24 = (float)func_00122BB8() / 2147483648.0f;
                *(float *)(p0 + 0) = D_70003A24 * (*(float *)(s2 + 0xC0) * func_0011E2A8(D_70003A20));
                *(float *)(p0 + 8) = D_70003A24 * (*(float *)(s2 + 0xC0) * func_0011DE90(D_70003A20));
                *(int *)(p80 + 0x80) = 0;
                p0 += 0x10;
                p80 += 4;
            }
        }
        *(int *)(s2 + 0xA0) = func_00122BB8();
        s0[4] = 1;
    case 1:
        f20 = D_00810360 - *(float *)(s0 + 0xC0);
        f21 = D_00810364 - *(float *)(s0 + 0xC4);
        f22 = D_00810368 - *(float *)(s0 + 0xC8);
        {
            for (s4 = 0, s1p = s2, s3p = s2; s4 < *(int *)(s2 + 0xA4); s4++) {
                int a3;
                float *t0;
                t0 = verts;
                for (a3 = 0; a3 < 4; a3++) {
                    t0[0] = *(float *)(s0 + 0xB0) + *(float *)(s1p + 0);
                    t0[1] = *(float *)(s0 + 0xB4);
                    t0[2] = *(float *)(s0 + 0xB8) + *(float *)(s1p + 8);
                    t0[3] = 1.0f;
                    if (s0[0xD] == 2) {
                        t0[0] = t0[0] + f20;
                        t0[1] = t0[1] + (f21 - 1.5f);
                        t0[2] = t0[2] + f22;
                    }
                    switch (a3) {
                    case 0:
                        t0[0] = t0[0] - *(float *)(s2 + 0xBC) * *(float *)(s3p + 0x80);
                        t0[2] = t0[2] - *(float *)(s2 + 0xBC) * *(float *)(s3p + 0x80);
                        break;
                    case 1:
                        t0[0] = t0[0] + *(float *)(s2 + 0xBC) * *(float *)(s3p + 0x80);
                        t0[2] = t0[2] - *(float *)(s2 + 0xBC) * *(float *)(s3p + 0x80);
                        break;
                    case 2:
                        t0[0] = t0[0] - *(float *)(s2 + 0xBC) * *(float *)(s3p + 0x80);
                        t0[2] = t0[2] + *(float *)(s2 + 0xBC) * *(float *)(s3p + 0x80);
                        break;
                    case 3:
                        t0[0] = t0[0] + *(float *)(s2 + 0xBC) * *(float *)(s3p + 0x80);
                        t0[2] = t0[2] + *(float *)(s2 + 0xBC) * *(float *)(s3p + 0x80);
                        break;
                    }
                    t0 += 4;
                }
                func_001CE300(1, verts, *(long long *)(s2 + 0xB0), *(int *)(s2 + 0xAC));
                *(float *)(s3p + 0x80) += 1.0f / *(float *)(s2 + 0xB8);
                if (*(float *)(s3p + 0x80) > 1.0f) {
                    *(float *)(s3p + 0x80) = 1.0f;
                }
                s1p += 0x10;
                s3p += 4;
            }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(s0);
        break;
    }
}

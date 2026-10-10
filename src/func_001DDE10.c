// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// SEMANTICS: depth-of-field pass (not a HUD bar builder, as an earlier note
// here said). Runs every world frame while render-context flag bit 1 is set
// (func_001D1EA0 -> func_001DDA00 -> func_001DDAA0 -> here; func_001DE920 is the
// alternative for area keys 0xB00-0xE00 with D_008106C8 & 0x60). It picks a focus
// point and transforms it through the camera matrix D_70003AC0: in a cinematic
// (func_0022EBE0()!=0) the camera look-at target, otherwise the player
// (D_00810360). Mode comes from func_0015D2F0 (forced to 2 on level ids
// 0x29/0xC/0xD) and selects the spring constants. The view depth then eases four
// per-pass Z thresholds (persistent block player+0x24F0) with per-pass gains:
// cinematic 4/2/1/0.5 with alpha 0x3E on all four; area key 0xB00 8/7/6/5 with
// alphas 24/40/56/72; default 8/4/1.5/1, alpha 0x3E; when func_001D2910(7) is up
// the gains and alpha follow 2*player.f1F4. For each of the four passes it emits:
// a copy of the field being drawn into a 256x256 PSMCT32 buffer at GS byte
// 0x258000 (func_001D6B10 -> func_001D6930, bilinear 2:1 shrink), the frame's
// draw environment again (func_001D1F20(3)), the copy bound as the texture
// (func_001D6BA0) with TEST/ALPHA set by func_001D6C90 (alpha test NEVER, colour
// only, Z test GEQUAL; blend (Cs-Cd)*As+Cd), and one full-field textured sprite
// whose Z is the pass's threshold and whose alpha is the pass's alpha, so only
// pixels at or behind that plane are blended with the blurred copy. The list is
// built at player+0x1C, closed with a 0x60 end packet and kicked once with
// func_001CB760(D_007635C0, 0xFFF000, base, 0x60) at slot 0xFFF.

typedef int u128 __attribute__((mode(TI)));

extern char *D_00275670;
extern int D_0027568C;
extern float D_00275690;
extern float D_00275694;
extern int D_008104E0[];
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];
extern char D_00810360[];
extern char D_70003AC0[];
extern char D_007635C0[];

extern int func_0015D2F0(char *);
extern int func_0022EBE0(void);
extern void func_00102948(void *, void *);
extern void func_001026A0(void *, void *, void *);
extern float func_0011DF78(float);
extern int func_001D2910(int);
extern int float_to_int(float);
extern void func_001D6B10(int, int, int, int);
extern void func_001D6BA0(int, int, int, int, int, int);
extern void func_001D1FF0(int, int);
extern void func_001D6C90();
extern void func_001D1F20(int);
extern void func_001CB760(void *, int, void *, int);

void func_001DDE10(void)
{
    char *base;
    int key;
    float arr1[4];
    float arr2[4];
    float q[4];
    char *rp;
    int mode;
    int v;
    int i;
    int r1;
    int r2;
    float f20;
    char *pp;
    int j;
    char *qq;
    int jj;
    int cc;
    int rr;
    int k;
    char *e;

    rp = D_00275670 + 0x24F0;
    key = (D_00810700[0] << 8) + D_00810701[0];
    mode = func_0015D2F0(D_00275670);
    switch (D_008104E0[0]) {
    case 0xD:
    case 0xC:
    case 0x29:
        mode = 2;
        break;
    }
    v = func_0022EBE0();
    if (v != 0) {
        func_00102948(q, D_00275670 + 0x2450);
        func_001026A0(q, D_70003AC0, q);
        q[2] = func_0011DF78(q[2]);
        q[3] = func_0011DF78(q[3]);
        q[3] += 100.0f;
        q[2] = (16.0f * q[2]) / q[3];
        q[2] = q[2] / 4.0f;
    } else {
        func_00102948(D_00275670 + 0x2450, D_00810360);
        *(float *)(D_00275670 + 0x2460) = 1.6777215e7f / D_00275690;
        *(float *)(D_00275670 + 0x2464) = 1.0f;
        func_00102948(q, D_00275670 + 0x2450);
        func_001026A0(q, D_70003AC0, q);
        q[3] = q[3] + D_00275694;
        q[2] = (16.0f * q[2]) / q[3];
        q[2] = q[2] / 4.0f;
    }

    switch (mode) {
    default:
        D_00275690 += 0.05f * (8500.0f - D_00275690);
        D_00275694 = 0.05f * (50.0f - D_00275694);
        break;
    case 1:
        D_00275690 += 0.05f * (30500.0f - D_00275690);
        D_00275694 = 0.05f * (1500.0f - D_00275694);
        break;
    case 2:
    case 3:
    case 0x82:
        D_00275690 += 0.05f * (40500.0f - D_00275690);
        D_00275694 = 0.05f * (1500.0f - D_00275694);
        break;
    }

    base = *(char **)(D_00275670 + 0x1C);
    for (i = 0; i < 4; i++) {
        if (func_001D2910(7) != 0) {
            f20 = 2.0f * *(float *)(D_00275670 + 0x1F4);
            switch (i) {
            case 0: {
                float t = *(float *)(D_00275670 + 0x2460);
                r1 = float_to_int(t + ((8.0f * (q[2] - t)) / 8.0f));
                break;
            }
            case 1: {
                float t = *(float *)(D_00275670 + 0x2460);
                r1 = float_to_int(t + (((4.0f + (3.5f * f20)) * (q[2] - t)) / 8.0f));
                break;
            }
            case 2: {
                float t = *(float *)(D_00275670 + 0x2460);
                r1 = float_to_int(t + (((1.5f + (5.5f * f20)) * (q[2] - t)) / 8.0f));
                break;
            }
            case 3: {
                float t = *(float *)(D_00275670 + 0x2460);
                r1 = float_to_int(t + (((1.0f + (5.0f * f20)) * (q[2] - t)) / 8.0f));
                break;
            }
            }
            r2 = float_to_int(62.0f + (30.0f * f20));
        } else if (v != 0) {
            switch (i) {
            case 0: {
                float t = *(float *)(D_00275670 + 0x2460);
                r1 = float_to_int(t + ((4.0f * (q[2] - t)) / 8.0f));
                r2 = 0x3E;
                break;
            }
            case 1: {
                float t = *(float *)(D_00275670 + 0x2460);
                r1 = float_to_int(t + ((2.0f * (q[2] - t)) / 8.0f));
                r2 = 0x3E;
                break;
            }
            case 2: {
                float t = *(float *)(D_00275670 + 0x2460);
                r1 = float_to_int(t + ((q[2] - t) / 8.0f));
                r2 = 0x3E;
                break;
            }
            case 3: {
                float t = *(float *)(D_00275670 + 0x2460);
                r1 = float_to_int(t + ((0.5f * (q[2] - t)) / 8.0f));
                r2 = 0x3E;
                break;
            }
            }
        } else if (key == 0xB00) {
            switch (i) {
            case 0: {
                float t = *(float *)(D_00275670 + 0x2460);
                r1 = float_to_int(t + ((8.0f * (q[2] - t)) / 8.0f));
                r2 = 0x18;
                break;
            }
            case 1: {
                float t = *(float *)(D_00275670 + 0x2460);
                r1 = float_to_int(t + ((7.0f * (q[2] - t)) / 8.0f));
                r2 = 0x28;
                break;
            }
            case 2: {
                float t = *(float *)(D_00275670 + 0x2460);
                r1 = float_to_int(t + ((6.0f * (q[2] - t)) / 8.0f));
                r2 = 0x38;
                break;
            }
            case 3: {
                float t = *(float *)(D_00275670 + 0x2460);
                r1 = float_to_int(t + ((5.0f * (q[2] - t)) / 8.0f));
                r2 = 0x48;
                break;
            }
            }
        } else {
            switch (i) {
            case 0: {
                float t = *(float *)(D_00275670 + 0x2460);
                r1 = float_to_int(t + ((8.0f * (q[2] - t)) / 8.0f));
                r2 = 0x3E;
                break;
            }
            case 1: {
                float t = *(float *)(D_00275670 + 0x2460);
                r1 = float_to_int(t + ((4.0f * (q[2] - t)) / 8.0f));
                r2 = 0x3E;
                break;
            }
            case 2: {
                float t = *(float *)(D_00275670 + 0x2460);
                r1 = float_to_int(t + ((1.5f * (q[2] - t)) / 8.0f));
                r2 = 0x3E;
                break;
            }
            case 3: {
                float t = *(float *)(D_00275670 + 0x2460);
                r1 = float_to_int(t + ((q[2] - t) / 8.0f));
                r2 = 0x3E;
                break;
            }
            }
        }
        arr1[i] = (float)r1;
        arr2[i] = (float)r2;
    }

    if ((v == 0) && (*(int *)(rp + 0x20) == 0)) {
        j = 0;
        pp = rp;
        for (; j < 4; j++) {
            float a = arr1[j];
            float b = *(float *)pp;
            *(float *)pp += 0.15f * (a - b);
            a = arr2[j];
            b = *(float *)(pp + 0x10);
            *(float *)(pp + 0x10) += 0.15f * (a - b);
            pp += 4;
        }
    } else {
        jj = 0;
        qq = rp;
        for (; jj < 4; jj++) {
            *(float *)qq = arr1[jj];
            *(float *)(qq + 0x10) = arr2[jj];
            qq += 4;
        }
    }
    *(int *)(rp + 0x20) = v;

    for (k = 0; k < 4; k++) {
        cc = float_to_int(*(float *)(rp + k * 4));
        rr = float_to_int(*(float *)(rp + k * 4 + 0x10));
        func_001D6B10(3, D_0027568C, 8, 8);
        func_001D6BA0(3, D_0027568C, 8, 8, 0, 0);
        func_001D1FF0(3, 3);
        func_001D6C90(3, 0, 1, 0, 0, 1, 0, 0,
                      (long long)1, (long long)2, (long long)0, (long long)1, (long long)0, (long long)1, (long long)0);
        {
            char *g = D_00275670;
            *(char *)(*(char **)(g + 0x1C) + 3) = 0x10;
            *(int *)(*(char **)(g + 0x1C) + 4) = 0;
            *(short *)(*(char **)(g + 0x1C) + 0) = 7;
            e = *(char **)(g + 0x1C);
            *(char **)(g + 0x1C) = e + 0x80;
        }
        *(u128 *)(e + 0x10) = 0;
        *(int *)(e + 0x1C) = 0x50000006;
        *(long long *)(e + 0x20) = (long long)(int)0x50AB4000 << 32 | (unsigned)0x8001;
        *(long long *)(e + 0x28) = 0x43431;
        *(int *)(e + 0x38) = 0x80;
        *(int *)(e + 0x34) = 0x80;
        *(int *)(e + 0x30) = 0x80;
        *(int *)(e + 0x3C) = rr;
        *(int *)(e + 0x40) = 8;
        *(int *)(e + 0x44) = 8;
        *(int *)(e + 0x50) = 0x7000;
        *(int *)(e + 0x54) = 0x7900;
        *(int *)(e + 0x58) = cc;
        *(int *)(e + 0x5C) = 0x80;
        *(int *)(e + 0x60) = 0x1008;
        *(int *)(e + 0x64) = 0x1008;
        *(int *)(e + 0x70) = 0x9000;
        *(int *)(e + 0x74) = 0x8700;
        *(int *)(e + 0x78) = cc;
        *(int *)(e + 0x7C) = 0x80;
    }

    func_001D1F20(3);
    {
        char *g = D_00275670;
        *(char *)(*(char **)(g + 0x1C) + 3) = 0x60;
        *(int *)(*(char **)(g + 0x1C) + 4) = 0;
        *(short *)(*(char **)(g + 0x1C) + 0) = 0;
        *(char **)(g + 0x1C) = *(char **)(g + 0x1C) + 0x10;
        func_001CB760(D_007635C0, 0xFFF000, base, 0x60);
    }
}

// NEARMISS func_00199220  (vram 0x00199220, 0x544 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.89% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP+GPR register-coloring permutation (documented wall class)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// AUTO-AIM target acquisition (R2 stance family). Copies the player's muzzle
// (arg0+0x20's +0xA0) into scratch D_700038A0, clears the 3 result slots
// D_008106E0/E4/E8 and seeds the best-distance triple f21/f22/f23=1000.0.
// Walks the global entity list D_00275B8C/D_00275B94: for each live,
// targetable (func_00183B80), "has-model" (+0x34!=0) candidate, computes its
// aim point (func_00183C40) and the distance from the PLAYER position
// (arg0+0x20's +0xA0, NOT the muzzle) to that aim point via the classic
// dx*dx+dy*dy+dz*dz -> func_0011E748 idiom; reject >= 260.0. Transforms the
// aim point by the camera view-proj (func_001026A0) to get screen pixels off
// the 2048 GS center (sx, 1.5*sy); reject points behind the camera (16/w<0).
// By aim option D_00810CA4: mode 1 (lock-on) is a radial cone
// sqrt(sx*sx+sy*sy) <= 50+55*s; else a box |sx|<=66+50*s, |sy|<=45+45*s
// (s = gun scratch +0x24, i.e. temp_s3->0x24). Survivors must pass an actor
// ray test (muzzle -> aim*1.2 overshoot) that hits the SAME candidate
// (func_0019A570 mask 0x20) and then a clear world LOS check (mask 0/6).
// Winners run a 3-slot insertion sort on distance (E0<=E4<=E8). Tail: reticle
// markers (func_001DD170) -- lock-on mode draws E0 only, manual draws all
// three slots (D_008106B0+0x30/+0x34/+0x38, which alias E0/E4/E8).
extern void func_00102948(void *dst, void *src);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_00102760(void *a, void *b);
extern void func_001028B8(void *a, void *b, void *c);
extern void func_00103230(void *a, void *b, float angle);
extern float func_0011DF78(float x);
extern float func_0011E748(float x);
extern int func_00183B80(unsigned char *p);
extern void func_00183C40(unsigned char *p, void *out);
extern int func_0019A570(void *a, void *b, int c, int d);
extern int func_001DD170(int a, void *b, int c, int d, int e);

extern unsigned char **D_00275B8C;
extern short D_00275B94;
extern void *D_008106E0[4];
extern void *D_008106E4[4];
extern void *D_008106E8[4];
extern unsigned char D_00810CA4[16];
extern char D_700031B0[];
extern char *D_700031D4[4];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038D0[];
extern char D_70003AC0[];

extern char D_008106B0[];

void func_00199220(char *arg0) {
    char *self0;
    char *scan;
    unsigned char **pp;
    unsigned char *e;
    short n;
    float f21, f22, f23;
    float dx, dy, dz, dist;
    float t;
    int ok;
    int i;
    char *slot;

    self0 = *(char **)(arg0 + 0x20);
    scan = self0 + 0x1F0;
    func_00102948(D_700038A0, self0 + 0xA0);
    n = D_00275B94;
    /* NOTE: player position is read from arg0+0xA0 directly (arg0 kept
       alive across the whole loop), NOT from self0+0xA0. */
    D_008106E0[0] = 0;
    f21 = 1000.0f;
    D_008106E4[0] = 0;
    pp = D_00275B8C;
    D_008106E8[0] = 0;
    f22 = 1000.0f;
    f23 = 1000.0f;

    if (n != 0) {
        do {
            e = *pp;
            n -= 1;
            pp += 1;
            if (*(unsigned char *)e != 0 && func_00183B80(e) != 0 && *(short *)(e + 0x34) != 0) {
                func_00183C40(e, D_700038B0);
                dx = *(float *)(arg0 + 0xA0) - *(float *)0x700038B0;
                dy = *(float *)(arg0 + 0xA4) - *(float *)0x700038B4;
                dz = *(float *)(arg0 + 0xA8) - *(float *)0x700038B8;
                dist = func_0011E748(dx * dx + dy * dy + dz * dz);
                if (dist < 260.0f) {
                    func_00102948(D_700038D0, D_700038B0);
                    *(float *)0x700038DC = 1.0f;
                    func_001026A0(D_700038D0, D_70003AC0, D_700038D0);
                    t = 16.0f / *(float *)0x700038DC;
                    if (!(t < 0.0f)) {
                        ok = 0;
                        {
                            int mode;
                            float xs;
                            xs = *(float *)0x700038D0 * t;
                            mode = D_00810CA4[0];
                            *(float *)0x700038D0 = xs / 16.0f - 2048.0f;
                            *(float *)0x700038D4 = (*(float *)0x700038D4 * t) / 16.0f - 2048.0f;
                            *(float *)0x700038D4 = 1.5f * *(float *)0x700038D4;
                            if (mode == 1) {
                                if (func_0011E748(*(float *)0x700038D0 * *(float *)0x700038D0 +
                                                   *(float *)0x700038D4 * *(float *)0x700038D4) <=
                                    (50.0f + 55.0f * *(float *)(scan + 0x24))) {
                                    ok = 1;
                                }
                            } else if (func_0011DF78(*(float *)0x700038D0) <= (66.0f + 50.0f * *(float *)(scan + 0x24)) &&
                                       func_0011DF78(*(float *)0x700038D4) <= (45.0f + 45.0f * *(float *)(scan + 0x24))) {
                                ok = 1;
                            }
                        }
                        if (ok != 0) {
                            *(float *)0x700038D0 = *(float *)0x700038B0 - *(float *)0x700038A0;
                            *(float *)0x700038D4 = *(float *)0x700038B4 - *(float *)0x700038A4;
                            *(float *)0x700038D8 = *(float *)0x700038B8 - *(float *)0x700038A8;
                            *(float *)0x700038DC = 1.0f;
                            func_00103230(D_700038D0, D_700038D0, 1.2f);
                            func_001028B8(D_700038D0, D_700038A0, D_700038D0);
                            if (func_0019A570(D_700038A0, D_700038D0, 1, 0x20) != 0 &&
                                e == (unsigned char *)D_700031D4[0] &&
                                func_0019A570(D_700038A0, D_700031B0, 6, 0) == 0) {
                                if (dist < f21) {
                                    f23 = f22;
                                    f22 = f21;
                                    f21 = dist;
                                    D_008106E8[0] = D_008106E4[0];
                                    D_008106E4[0] = D_008106E0[0];
                                    D_008106E0[0] = e;
                                } else if (dist < f22) {
                                    f23 = f22;
                                    f22 = dist;
                                    D_008106E8[0] = D_008106E4[0];
                                    D_008106E4[0] = e;
                                } else if (dist < f23) {
                                    D_008106E8[0] = e;
                                    f23 = dist;
                                }
                            }
                        }
                    }
                }
            }
        } while (n != 0);
    }

    if (D_00810CA4[0] == 1) {
        if (D_008106E0[0] != 0) {
            func_00183C40(D_008106E0[0], D_700038B0);
            func_001DD170(1, D_700038B0, 0, 0x80808080, 0);
        }
    } else {
        slot = D_008106B0;
        i = 0;
        do {
            e = *(unsigned char **)(slot + 0x30);
            if (e != 0) {
                func_00183C40(e, D_700038B0);
                func_001DD170(1, D_700038B0, 0, 0x80808080, 0);
            }
            i += 1;
            slot += 4;
        } while (i < 3);
    }
}

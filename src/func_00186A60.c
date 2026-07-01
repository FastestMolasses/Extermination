// NEARMISS func_00186A60  (vram 0x00186A60, 0x860 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.43% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered (521/536 instrs identical); 3 residual regions, all 2.3.1.01-vs-available-builds scheduler artifacts: (1) same at-lui taken-path speculation into the `bnez i` delay slot (1 site, as in func_001607D0); (2) the spread clamp `if (20.0f <= spread) spread = 20.0f;` lower...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Player weapon fire: casts up to 10 spread rays from the aim transform.
// Builds an aim segment (scratchpad D_700038A0/D_70003900) from the actor's
// pos (+0xC0) and dir (+0xA0), probes the world with func_0019A570; on a hit
// it computes the hit distance (clamped to >= 70) and a spread radius
// (20 * dist/80), then loops 10 rays (ray 0 = center, rays 1..9 jittered by
// rand inside the spread cone) resolving each hit: damage (0x2A/0x23/0x19 by
// distance), impact classification via func_001839A0 (0x101/0x201/0x300 ...),
// impact FX func_001EFD90, target reaction func_001B41F0, and a hit-record
// from func_001AFA90 (kind at +0x2E, phase at +0xD, pos at +0xB0, callback
// func_0018ABA0 at +0x10). Always returns 0.
extern char D_700031B0[];
extern char D_700037A0[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700038E0[];
extern char D_70003900[];
extern void func_001026A0(char *dst, char *mtx, char *src);
extern float func_00102738(char *a, char *b);
extern void func_001028B8(char *dst, char *a, char *b);
extern void func_001028D0(char *dst, char *a, char *b);
extern void func_00102918(char *dst, char *a, char *b);
extern void func_00102948(char *dst, char *src);
extern void func_00103230(char *dst, char *src, float len);
extern float func_0011DE90(float a);
extern float func_0011E2A8(float a);
extern float func_0011E748(float a);
extern int func_00122BB8(void);
extern int func_001839A0(char *tgt);
extern int func_001860A0(char *a, char *b);
extern int func_0019A570(char *a, char *b, int c, int d);
extern char *func_001AFA90(int a);
extern float func_001B1470(float a);
extern void func_001B41F0(char *tgt, char *pos, char *from, int parm, int a, short b);
extern void func_001CD390(char *dst, char *src);
extern void func_001EFD90(int id, char *pos, char *nrm);
extern int func_0018ABA0();

int func_00186A60(char *p) {
    int fired;
    int type;
    int parm;
    short kind;
    int i;
    short dmg;
    int hit;
    char *tgt;
    char *src;
    char *hb;
    float minr;
    float spread;
    float d2;

    fired = 0;
    func_00103230(D_700038A0, p + 0xC0, 100.0f);
    func_001028B8(D_700038A0, D_700038A0, p + 0xA0);
    *(float *)0x700038AC = 1.0f;
    func_00102948(D_70003900, p + 0xA0);
    *(float *)0x7000390C = 1.0f;
    if (func_0019A570(D_70003900, D_700038A0, 7, 0x20) != 0) {
        func_001028D0(D_700038B0, D_700031B0, p + 0xA0);
        d2 = func_0011E748(func_00102738(D_700038B0, D_700038B0));
        *(float *)0x70003A20 = d2;
        if (d2 < 70.0f) {
            *(float *)0x70003A20 = 70.0f;
        }
        fired = 1;
        *(float *)0x70003A38 = 20.0f * (*(float *)0x70003A20 / 80.0f);
    } else {
        func_001860A0(D_70003900, D_700038A0);
    }
    if (fired != 0) {
        func_001CD390(D_700037A0, p + 0xC0);
        func_00102918(D_700037A0, D_700037A0, D_700038A0);
        *(float *)0x70003A3C = 4.656613e-10f * (float)func_00122BB8();
        for (i = 0; i < 10; i++) {
            if (i == 0) {
                func_00102948(D_700038B0, D_700038A0);
            } else {
                spread = *(float *)0x70003A38;
                if (20.0f <= spread) {
                    spread = 20.0f;
                }
                minr = 4.0f;
                *(float *)0x70003A24 = spread;
                if (!(4.0f <= spread)) {
                    minr = spread;
                }
                *(float *)0x70003A28 = minr;
                *(float *)0x70003A2C = minr + ((spread - minr) * (4.656613e-10f * (float)func_00122BB8()));
                *(float *)0x70003A20 = func_001B1470(*(float *)0x70003A3C + (0.62831855f * (float)i));
                *(float *)0x700038B0 = *(float *)0x70003A2C * func_0011DE90(*(float *)0x70003A20);
                *(float *)0x700038B4 = *(float *)0x70003A2C * func_0011E2A8(*(float *)0x70003A20);
                *(float *)0x700038B8 = 0.0f;
                *(float *)0x700038BC = 1.0f;
                func_001026A0(D_700038B0, D_700037A0, D_700038B0);
                *(float *)0x700038BC = 1.0f;
            }
            if ((hit = func_0019A570(D_70003900, D_700038B0, 7, 0x20)) != 0) {
                func_001028D0(D_700038C0, D_700031B0, p + 0xB0);
                *(float *)0x70003A20 = func_0011E748(func_00102738(D_700038C0, D_700038C0));
                func_00102948(D_700038C0, D_700031B0);
                *(float *)0x700038CC = 1.0f;
                src = *(char **)0x700031D0;
                *(float *)0x700038E0 = *(float *)(src + 0x24);
                *(float *)0x700038E4 = *(float *)(src + 0x28);
                *(float *)0x700038E8 = *(float *)(src + 0x2C);
                if (i == 0) {
                    *(float *)0x700038EC = 1.0f;
                } else {
                    *(float *)0x700038EC = 0.0f;
                }
                d2 = *(float *)0x70003A20;
                if (d2 < 20.0f) {
                    dmg = 0x2A;
                } else if (d2 < 50.0f) {
                    dmg = 0x23;
                } else {
                    dmg = 0x19;
                }
                tgt = *(char **)0x700031D4;
                type = *(unsigned char *)(src + 0x1A);
                parm = *(int *)(src + 0x1C);
                kind = 0;
                *(int *)0x700031E8 = 0;
                if (func_001860A0(D_70003900, D_700038C0) != 0) {
                    *(int *)0x700031E8 = -1;
                }
                if (hit == 1) {
                    if (tgt != 0) {
                        if ((*(unsigned char *)tgt == 1) && ((*(unsigned char *)(tgt + 2) & ~0xE0) == 2)) {
                            func_001B41F0(tgt, D_700038C0, p + 0xC0, parm, 0, dmg);
                            *(int *)0x700031E8 = -1;
                        } else {
                            kind = 0x100;
                        }
                    }
                } else if (hit == 2) {
                    if (tgt != 0) {
                        if (*(unsigned char *)tgt != 0) {
                            kind = func_001839A0(tgt);
                            if (kind == 0x101) {
                                if ((unsigned int)(type - 2) < 3U) {
                                    kind = 0x201;
                                } else if (type == 5) {
                                    func_001EFD90(0x8000002C, D_700038C0, D_700038E0);
                                    *(int *)0x700031E8 = -1;
                                } else if (type == 8) {
                                    func_001EFD90(0x80000067, D_700038C0, D_700038E0);
                                    *(int *)0x700031E8 = -1;
                                }
                            } else if (kind == 0x300) {
                                func_001EFD90(0x80000067, D_700038C0, D_700038E0);
                                *(int *)0x700031E8 = -1;
                            }
                            *(short *)(tgt + 0x36) = dmg;
                            func_00102948(tgt + 0x70, p + 0xC0);
                        }
                    } else if ((unsigned int)(type - 2) < 3U) {
                        kind = 0x201;
                    } else if (type == 5) {
                        func_001EFD90(0x8000002C, D_700038C0, D_700038E0);
                        *(int *)0x700031E8 = -1;
                    } else if (type == 8) {
                        func_001EFD90(0x80000067, D_700038C0, D_700038E0);
                        *(int *)0x700031E8 = -1;
                    } else {
                        kind = 0x101;
                    }
                } else if ((unsigned int)(type - 2) < 3U) {
                    kind = 0x201;
                } else if (type == 5) {
                    func_001EFD90(0x8000002C, D_700038C0, D_700038E0);
                    *(int *)0x700031E8 = -1;
                } else if (type == 8) {
                    func_001EFD90(0x80000067, D_700038C0, D_700038E0);
                    *(int *)0x700031E8 = -1;
                } else {
                    kind = 0x101;
                }
                hb = func_001AFA90(1);
                if (hb != 0) {
                    if (*(int *)0x700031E8 == -1) {
                        *(char *)(hb + 0xD) = 0;
                    } else {
                        if (i == 0) {
                            *(char *)(hb + 0xD) = 1;
                        } else {
                            *(char *)(hb + 0xD) = 2;
                        }
                        *(short *)(hb + 0x2E) = kind;
                    }
                    *(char *)(hb + 3) = 3;
                    *(float *)(hb + 0xB0) = *(float *)0x700038C0;
                    *(float *)(hb + 0xB4) = *(float *)0x700038C4;
                    *(float *)(hb + 0xB8) = *(float *)0x700038C8;
                    *(int *)(hb + 0x10) = (int)func_0018ABA0;
                    if (hit != 1) {
                        *(float *)0x700038EC = 1.0f;
                        func_00102948(hb + 0xC0, D_700038E0);
                    } else {
                        *(int *)(hb + 0xCC) = 0;
                    }
                }
            }
        }
    }
    return 0;
}

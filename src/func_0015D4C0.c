// NEARMISS func_0015D4C0  (vram 0x0015D4C0, 0x9F8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.34% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 991202 list-scheduler / FP-coloring shapes that mwcc233 cannot reproduce — body and structure fully correct, 3 residual regions all artifact-class: (1) load shadow-fill: in the scratch-vector builds (0x700038C0 block in case 0x37/38 and 0x700038A0 block in case 0x3D) the target interleaves the NE...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Entity AI action dispatcher on action byte e[0x23B]. Each case validates a
// precondition (input via func_00177030 / global gates D_00810C7C/7D), builds a
// probe point in scratchpad vectors (0x700038A0/B0/C0), raycasts or volume-tests
// the world (func_0019AD00 / func_0019A570 / func_0019AFE0 / func_00180300),
// and on success commits a new state: e[5]=state, e[6]=substate 0, e[0x1F0]=anim id,
// plus per-action pose/height fields (e+0xB0 camera pos, e+0x254 target height,
// e+0x290 grab point). Returns 1 if the action started, 0 otherwise.
extern int func_00176F90(unsigned char *e);
extern int func_00177030(unsigned char *e, int n);
extern void build_trs_matrix(void *mtx, void *pos, void *rot, void *scale);
extern void func_001026A0(void *dst, void *src, void *xf);
extern void func_00102948(void *dst, void *a, void *b);
extern void func_001031E0(void *dst, void *src);
extern int func_00180300(unsigned char *e, void *m, int b);
extern int func_0019A570(void *a, void *b, int n, int m);
extern int func_0019AD00(unsigned char *e, void *v, int n);
extern int func_0019AFE0(unsigned char *e, void *a, void *b, int n);
extern void func_0019BC40(void *v, void *q);
extern void func_00199DB0(void *v);
extern int func_00199FA0(char *arg0, char *arg1);
extern float func_0011E620(float y, float x);
extern float func_001B1470(float a);

extern float D_002488B0;
extern unsigned char D_00810C7C;
extern unsigned char D_00810C7D;
extern float D_700030F0;
extern unsigned short D_70003170;
extern int D_700031B0;
extern int D_700038A0;
extern int D_700038B0;
extern int D_700038C0;

int func_0015D4C0(unsigned char *e)
{
    unsigned char *q;
    unsigned short *flg;
    float *ds;
    float d;
    float dx;
    float dz;
    float fy;
    float f;
    float oldC4;
    int found;
    int i;
    int n;
    unsigned char v1;
    float sp40[4];
    float sp50[4];

    if (func_00176F90(e)) {
        switch (e[0x23B]) {
        case 0x37:
        case 0x38:
            if (func_00177030(e, 1)) {
                e[5] = 0x18;
                e[6] = 0;
                e[0x1F0] = 0x2C;
                if (e[0x23B] == 0x37) {
                    e[0xD] = 0;
                } else {
                    build_trs_matrix(e + 0xD0, e + 0xB0, e + 0xC0, e + 0x60);
                    *(int *)0x700038A0 = 0;
                    *(int *)0x700038A4 = 0x408051EC;
                    *(int *)0x700038A8 = 0x41200000;
                    *(int *)0x700038AC = 0x3F800000;
                    func_001026A0(&D_700038B0, e + 0xD0, &D_700038A0);
                    if (func_0019AD00(e, &D_700038B0, 7)) {
                        found = 0;
                        q = *(unsigned char **)0x700031D0;
                        dx = *(float *)0x700031B0 - *(float *)(q + 0x24);
                        dz = *(float *)0x700031B8;
                        fy = *(float *)0x700038B4;
                        *(float *)0x700038C0 = dx;
                        *(float *)0x700038C8 = dz - *(float *)(q + 0x2C);
                        *(float *)0x700038C4 = fy;
                        *(int *)0x700038CC = 0x3F800000;
                        func_0019BC40(&D_700038C0, q);
                        n = *(int *)0x700031E0;
                        if (n != 0) {
                            flg = &D_70003170;
                            ds = &D_700030F0;
                            for (i = 0; i < n; i++) {
                                if ((*flg & 1) && !(*ds <= 4.01f + *(float *)(e + 0xB4))) {
                                    d = (&D_700030F0)[i];
                                    if (!(d < *(float *)(e + 0xB4) + D_002488B0)) {
                                        found = 1;
                                    }
                                    *(float *)(e + 0x254) = d;
                                    break;
                                }
                                flg++;
                                ds++;
                            }
                        }
                    }
                    if (found) {
                        e[0xD] = 3;
                        *(float *)(e + 0xB0) = *(float *)0x700031B0 + 1.5f * *(float *)(*(int *)0x700031D0 + 0x24);
                        *(float *)(e + 0xB8) = *(float *)0x700031B8 + 1.5f * *(float *)(*(int *)0x700031D0 + 0x2C);
                    } else {
                        e[0xD] = 1;
                        *(int *)0x700038A0 = 0;
                        *(int *)0x700038A4 = 0;
                        *(int *)0x700038A8 = 0xBF800000;
                        *(int *)0x700038AC = 0x3F800000;
                        func_001026A0(e + 0xB0, e + 0xD0, &D_700038A0);
                    }
                }
                return 1;
            }
            break;
        case 0x32:
            if (func_00177030(e, 4)) {
                build_trs_matrix(e + 0xD0, e + 0xB0, e + 0xC0, e + 0x60);
                e[0xD] = 0;
                e[5] = 0xB;
                e[6] = 0;
                func_001031E0(&D_700038A0, e + 0xB0);
                *(float *)0x700038A4 += 10.0f;
                if (func_00180300(e, &D_700038A0, 0) == 0) {
                    e[0x1F0] = 0x15;
                } else {
                    e[0x1F0] = 0x16;
                }
                return 1;
            }
            break;
        case 0x3B:
            if (func_00177030(e, 1)) {
                e[0xD] = 1;
                e[5] = 0xB;
                e[6] = 0;
                e[0x1F0] = 0x15;
                return 1;
            }
            break;
        case 0x33:
            if (D_00810C7D != 0 && func_00177030(e, 1)) {
                build_trs_matrix(e + 0xD0, e + 0xB0, e + 0xC0, e + 0x60);
                *(int *)0x700038A0 = 0;
                *(int *)0x700038A4 = 0;
                *(int *)0x700038A8 = 0xBF800000;
                *(int *)0x700038AC = 0x3F800000;
                func_001026A0(e + 0xB0, e + 0xD0, &D_700038A0);
                build_trs_matrix(e + 0xD0, e + 0xB0, e + 0xC0, e + 0x60);
                e[0xD] = 2;
                e[5] = 0xD;
                e[6] = 0;
                func_001031E0(&D_700038A0, e + 0xB0);
                *(float *)0x700038A4 += 10.0f;
                if (func_00180300(e, &D_700038A0, 2) == 0) {
                    e[0x1F0] = 0x1B;
                } else {
                    e[0x1F0] = 0x1C;
                }
                return 1;
            }
            break;
        case 0x3A:
            func_001031E0(&D_700038A0, e + 0xB0);
            *(float *)0x700038A4 += 40.0f;
            if (func_0019A570(e + 0xB0, &D_700038A0, 4, 0)) {
                e[0x23B] = *(unsigned char *)(*(int *)0x700031D0 + 0x1A);
            }
            v1 = e[0x23B];
            if (v1 == 0x1E) {
                e[5] = 0x11;
                e[6] = 0;
                e[0x1F0] = 0x20;
                *(float *)(e + 0x254) = *(float *)0x700031B4 - 20.5f;
                return 1;
            }
            if (v1 == 0x34) {
                *(int *)(e + 0x30C) = *(int *)0x700031D0;
                func_00176F90(e);
                *(float *)(e + 0x2E0) = *(float *)(*(int *)0x700031D0 + 0x34);
                *(float *)(e + 0x2E8) = *(float *)(*(int *)0x700031D0 + 0x3C);
                if (func_00177030(e, 2)) {
                    func_001031E0(&D_700038A0, e + 0xB0);
                    *(float *)0x700038A4 += 40.0f;
                    if (func_0019A570(e + 0xB0, &D_700038A0, 4, 0)) {
                        e[0x23B] = 0x34;
                        *(float *)(e + 0x254) = *(float *)0x700031B4 - 20.5f;
                        func_00199DB0(e + 0x290);
                        e[5] = 0xF;
                        e[6] = 0;
                        e[0x1F0] = 0x20;
                    }
                    return 1;
                }
            }
            break;
        case 0x20:
            if (D_00810C7C != 0) {
                func_00199DB0(&D_700038A0);
                func_001031E0(&D_700038B0, &D_700038A0);
                *(float *)0x700038B4 += 40.0f;
                if (func_0019A570(&D_700038A0, &D_700038B0, 4, 0)) {
                    q = *(unsigned char **)0x700031D0;
                    if (*(unsigned char *)(q + 0x1A) == 0x3C) {
                        *(float *)0x70003A20 = func_0011E620(-*(float *)(q + 0x3C), *(float *)(q + 0x34));
                        f = func_001B1470(1.5707964f + *(float *)0x70003A20);
                        *(float *)0x70003A20 = f;
                        *(float *)(e + 0x218) = f;
                        *(float *)(e + 0xB0) = *(float *)0x700038A0;
                        *(float *)(e + 0xB8) = *(float *)0x700038A8;
                        e[5] = 0x16;
                        e[6] = 0;
                        e[0x1F0] = 0x29;
                        return 1;
                    }
                }
            }
            break;
        case 0x3D:
            oldC4 = *(float *)(e + 0xC4);
            if (func_00177030(e, 0)) {
                build_trs_matrix(e + 0xD0, e + 0xB0, e + 0xC0, e + 0x60);
                *(int *)0x700038A0 = 0;
                *(int *)0x700038A4 = 0x41200000;
                *(int *)0x700038A8 = 0x41200000;
                *(int *)0x700038AC = 0x3F800000;
                func_001026A0(&D_700038B0, e + 0xD0, &D_700038A0);
                if (func_0019AD00(e, &D_700038B0, 7)) {
                    q = *(unsigned char **)0x700031D0;
                    dx = *(float *)0x700031B0 + 3.8f * *(float *)(q + 0x24);
                    dz = *(float *)0x700031B8;
                    fy = *(float *)0x700038B4;
                    *(float *)0x700038A0 = dx;
                    *(float *)0x700038A8 = dz + 3.8f * *(float *)(q + 0x2C);
                    *(float *)0x700038A4 = fy;
                    *(int *)0x700038AC = 0x3F800000;
                    func_00102948(&D_700038B0, &D_700038A0, q);
                    *(float *)0x700038B4 += 30.0f;
                    if (func_0019A570(&D_700038A0, &D_700038B0, 6, 0)) {
                        func_001031E0(&D_700038A0, &D_700031B0);
                        *(int *)0x700038B0 = 0;
                        *(int *)0x700038B4 = 0;
                        f = *(float *)0x700038A4 + 0.5f;
                        *(int *)0x700038B8 = 0xC1200000;
                        *(int *)0x700038BC = 0x3F800000;
                        *(float *)0x700038A4 = f;
                        func_001026A0(&D_700038C0, e + 0xD0, &D_700038B0);
                        *(float *)0x700038C4 = *(float *)0x700038A4;
                        if (func_0019AFE0(e, &D_700038C0, &D_700038A0, 6)) {
                            *(float *)(e + 0x290) = *(float *)0x700031B0 + 1.5f * *(float *)(*(int *)0x700031D0 + 0x24);
                            *(float *)(e + 0x298) = *(float *)0x700031B8 + 1.5f * *(float *)(*(int *)0x700031D0 + 0x2C);
                            func_00199FA0((char *)sp40, (char *)sp50);
                            *(float *)(e + 0x294) = sp50[1] - 20.5f;
                            e[5] = 0xA;
                            e[6] = 0;
                            e[0x1F0] = 0x14;
                            return 1;
                        }
                    }
                }
                *(float *)(e + 0xC4) = oldC4;
            }
            break;
        }
    }
    return 0;
}

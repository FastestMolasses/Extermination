// NEARMISS func_00130AB0  (vram 0x00130AB0, 0x754 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.50% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 6 instructions in two spots. (1) 0x198/0x19C -- call-arg materialization ORDER for func_001B1EA0(0, D_00810360, &m, 4): target emits `addiu a1,v0,%lo(D_00810360)` then `addiu a2,sp,0x50`; mwcc emits `addiu a2,sp,0x50` first (the stack-address arg is scheduled before the global-reloc arg). Tried: ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef int u128 __attribute__((mode(TI)));

typedef struct {
    u128 q0;
    u128 q1;
    u128 q2;
    u128 q3;
} Mtx4;

extern void anim_clip_init(unsigned char *, int, float, float);
extern void func_001028B8(unsigned char *, unsigned char *, unsigned char *);
extern void func_001029C0(unsigned char *);
extern void func_00102BB0(unsigned char *, unsigned char *, float);
extern float func_0011DF78(float);
extern void func_00131F20(unsigned char *, unsigned char *, unsigned char *);
extern void func_00132490(unsigned char *, unsigned char *);
extern int func_00133CD0(unsigned char *);
extern void func_00182F90(unsigned char *, unsigned char *);
extern float func_001B1470(float);
extern int func_001B1EA0(int, unsigned char *, unsigned char *, int);
extern void func_001B2B10(unsigned char *, unsigned char *, unsigned char *);
extern void func_001EFD90(int, unsigned char *, unsigned char *);
extern void func_001EFEB0(int, unsigned char *);
extern void func_001FBD50(unsigned char *, int, int, float);
extern int func_0021BED0(unsigned char *);
extern void func_0021BF90(unsigned char *, unsigned char *);

extern Mtx4 D_00244F90;
extern volatile unsigned char D_008102B0;
extern char D_008102BF;
extern float D_00810354;
extern unsigned char D_00810360[];
extern float D_00810374;
extern volatile float D_008104D0;
extern volatile float D_008104D4;
extern float D_008104DC;
extern unsigned char D_008106BD;
extern unsigned char D_0081070A;
extern unsigned char D_700036A0;
extern unsigned char D_700038A0;
extern unsigned char D_700038B0;

void func_00130AB0(unsigned char *p, unsigned char *e) {
    Mtx4 m;
    int i;
    unsigned char *q;
    unsigned char st;
    unsigned short t;
    float d;
    float v;
    float w3;
    float w2;
    float w1;
    float w0;

    m = D_00244F90;
    st = p[6];
    switch (st) {
    case 0:
        p[6] = *(volatile unsigned char *)(p + 6) + 1;
        e[0x63] = 0;
        e[0x6B] = 1;
        anim_clip_init(p, 0x17, 5.0f, 0.0f);
        break;
    case 1:
        if (*(unsigned short *)(e + 0x58) & 0x1000) {
            p[6] = st + 1;
            anim_clip_init(p, 0x18, 1.0f, 0.0f);
            func_001FBD50(p, 0x7D0, 0, 300.0f);
        }
        break;
    case 2:
        if (*(unsigned short *)(e + 0x58) & 0x1000) {
            if (func_0021BED0((unsigned char *)&D_008102B0) == 0 && func_00133CD0(p) != 0 &&
                func_0011DF78(D_00810354 - *(float *)(p + 0xB4)) <= 6.0f) {
                q = (unsigned char *)&m;
                i = 0;
                do {
                    func_00131F20(p, q, q);
                    func_001028B8(q, q, p + 0xB0);
                    i++;
                    q += 0x10;
                } while (i < 4);
                if (func_001B1EA0(0, D_00810360, (unsigned char *)&m, 4) != 0) {
                    p[6] = p[6] + 1;
                    func_0021BF90((unsigned char *)&D_008102B0, p);
                    e[0x65] = 1;
                    D_00810374 = func_001B1470(3.1415927410125732f + *(float *)(p + 0xC4));
                    *(volatile int *)0x700038A0 = 0;
                    *(volatile int *)0x700038A4 = 0;
                    *(volatile float *)0x700038A8 = 15.0f;
                    *(volatile int *)0x700038AC = 0;
                    func_00131F20(p, &D_700038A0, &D_700038A0);
                    func_001028B8(&D_700038A0, &D_700038A0, p + 0xB0);
                    func_00182F90((unsigned char *)&D_008102B0, &D_700038A0);
                    anim_clip_init(p, 0x1A, 1.0f, 0.0f);
                    break;
                }
            }
            p[6] = 5;
            anim_clip_init(p, 0x19, 1.0f, 0.0f);
        }
        break;
    case 3:
        if (*(unsigned short *)(e + 0x58) & 0x1000) {
            if (D_008106BD != 1) {
                p[6] = st + 1;
                *(unsigned short *)(e + 0x50) = 0;
                *(unsigned short *)(e + 0x52) = 0;
                anim_clip_init(p, 0x1B, 1.0f, 0.0f);
            } else {
                p[6] = 5;
                e[0x65] = 0;
                D_008106BD = 0;
                anim_clip_init(p, 0x1C, 1.0f, 0.0f);
            }
        }
        break;
    case 4:
        t = *(unsigned short *)(e + 0x50) + 1;
        *(unsigned short *)(e + 0x50) = t;
        if (t >= 0x12C) {
            p[6] = p[6] + 1;
            D_008102BF = 3;
            e[0x65] = 0;
            if (p[0xD] & 1) {
                if (D_0081070A != 0) {
                    D_008104DC = 45.0f;
                } else {
                    D_008104DC = 40.0f;
                }
            } else {
                if (D_0081070A != 0) {
                    D_008104DC = 50.0f;
                } else {
                    D_008104DC = 40.0f;
                }
            }
            anim_clip_init(p, 0x1D, 1.0f, 0.0f);
        } else if (D_008106BD == 1) {
            p[6] = 5;
            e[0x65] = 0;
            D_008106BD = 0;
            anim_clip_init(p, 0x1C, 1.0f, 0.0f);
        } else {
            t = *(unsigned short *)(e + 0x52) + 1;
            *(unsigned short *)(e + 0x52) = t;
            if (t >= 0x1E) {
                *(unsigned short *)(e + 0x52) = 0;
                if (p[0xD] & 0x80) {
                    if (D_0081070A != 0) {
                        *(volatile float *)0x70003A20 = 8.0f;
                    } else {
                        *(volatile float *)0x70003A20 = 6.0f;
                    }
                } else {
                    if (D_0081070A != 0) {
                        *(volatile float *)0x70003A20 = 5.0f;
                    } else {
                        *(volatile float *)0x70003A20 = 3.0f;
                    }
                }
                d = D_008104D0;
                v = *(volatile float *)0x70003A20;
                if (d <= v) {
                    p[6] = 5;
                    e[0x65] = 0;
                    D_008102BF = 5;
                    D_008104D4 = D_008104D0;
                    D_008102B0 |= 2;
                    anim_clip_init(p, 0x1D, 1.0f, 0.0f);
                } else {
                    D_008104D4 = v;
                    D_008102B0 |= 2;
                }
                *(volatile int *)0x700038A8 = 0;
                *(volatile int *)0x700038A4 = 0;
                *(volatile int *)0x700038A0 = 0;
                *(volatile float *)0x700038AC = 1.0f;
                *(volatile float *)0x700038A4 = *(float *)(p + 0xC4);
                *(volatile float *)0x700038B0 = 2.0f;
                *(volatile float *)0x700038B4 = 15.5f;
                *(volatile float *)0x700038B8 = 1.0f;
                *(volatile float *)0x700038BC = 1.0f;
                func_001B2B10(p, &D_700038B0, &D_700038B0);
                func_001028B8(&D_700038B0, p + 0xB0, &D_700038B0);
                func_001EFD90(0x80000022, &D_700038B0, &D_700038A0);
            }
        }
        break;
    case 5:
        if (*(float *)(p + 0x3C) == 72.0f) {
            func_001029C0(&D_700036A0);
            func_00102BB0(&D_700036A0, &D_700036A0, *(float *)(p + 0xC4));
            *(volatile float *)0x700038A0 = 2.0f;
            *(volatile float *)0x700038A4 = 15.5f;
            *(volatile float *)0x700038A8 = 1.0f;
            *(volatile float *)0x700038AC = 1.0f;
            func_001B2B10(p, &D_700038A0, &D_700038A0);
            func_001028B8(&D_700038A0, p + 0xB0, &D_700038A0);
            w0 = *(volatile float *)0x700038A0;
            w1 = *(volatile float *)0x700038A4;
            w2 = *(volatile float *)0x700038A8;
            w3 = *(volatile float *)0x700038AC;
            *(volatile float *)0x700036D0 = w0;
            *(volatile float *)0x700036D4 = w1;
            *(volatile float *)0x700036D8 = w2;
            *(volatile float *)0x700036DC = w3;
            func_001EFEB0(0x8000002A, &D_700036A0);
        }
        if (*(unsigned short *)(e + 0x58) & 0x1000) {
            p[5] = 0;
            p[6] = 0;
            e[0x62] = 0;
            *(unsigned short *)(e + 0x50) = 0;
            *(unsigned short *)(e + 0x52) = 0;
            *(short *)(e + 0x5A) = 0;
            e[0x6B] = 0;
        }
        break;
    }
    if (p[6] != 7) {
        func_00132490(p, e);
    }
}

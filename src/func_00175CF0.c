// NEARMISS func_00175CF0  (vram 0x00175CF0, 0x3C4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.15% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Clean-store/delay-slot-fill duplication: at the 'v1==0x35' guard bne, mwcc233 (and 991202 identically) fills the branch's delay slot with a duplicated 'lui $at,0x7000' materialization for the shared-page global address (0x700031D0 / 0x700031C0 both resolve to the same upper-16 lui), instead of le...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001029C0(void *a0);
extern void func_00102BB0(void *obj, void *obj2, float v);
extern float func_0011E398(float f);
extern float func_0011E620(float y, float x);
extern int func_00175640(char *a0);
extern void func_0017F9E0(char *a0);
extern void func_0017FB90(char *a0, unsigned char a1);
extern void func_0019A310(char *a0);
extern float func_001B1470(float a);
extern int D_700036A0;
extern int D_700038A0;
extern int D_700038B0;

void func_00175CF0(char *arg0, int arg1, int arg2) {
    char *p;
    unsigned char v1;
    unsigned char v1b;
    unsigned short st;
    float t;
    float f0;
    float f1;

    *(unsigned short *)(arg0 + 0x238) = *(unsigned short *)(*(char **)0x700031D0 + 0x1A) & 0xFF00;
    *(unsigned char *)(arg0 + 0x23B) = *(unsigned short *)(*(char **)0x700031D0 + 0x1A);
    func_0019A310(arg0 + 0x9C);

    v1 = *(unsigned char *)(arg0 + 0x23B);
    if (v1 == 0x35) {
        p = *(char **)0x700031D0;
        f0 = func_0011E620(-*(float *)(p + 0x3C), *(float *)(p + 0x34));
        *(float *)0x70003A20 = f0;
        f1 = func_001B1470(f0 - 1.5707964f);
        *(float *)0x70003A20 = f1;
        *(float *)(arg0 + 0x310) = f1;
    }

    *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)0x700031C0;
    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + *(float *)0x700031C4;
    *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)0x700031C8;

    if (arg1 & 2) {
        int t1 = *(int *)0x700031D4;
        if (t1 != 0) {
            *(int *)(arg0 + 0x214) = t1;
        }
    }

    st = *(unsigned short *)(arg0 + 0x238);
    if (st == 0x2000 || (st == 0x1000 && func_00175640((char *)(*(int *)(arg0 + 0x214))) != 0)) {
        float ang = *(float *)(arg0 + 0x9C);
        if (!(ang < 0.017453292f) && ang <= 3.1241393f) {
            p = *(char **)0x700031D0;
            *(float *)0x70003A20 = func_0011E620(-*(float *)(p + 0x2C), *(float *)(p + 0x24));
            *(float *)0x700031E4 = func_001B1470(1.5707964f + *(float *)0x70003A20);
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) - *(float *)0x700031C4;
            t = func_0011E398(*(float *)(arg0 + 0x9C));
            *(volatile int *)0x700038A0 = 0;
            *(volatile int *)0x700038A4 = 0;
            *(volatile float *)0x700038A8 = *(float *)0x700031C4 / t;
            *(volatile int *)0x700038AC = 0;
            func_001029C0(&D_700036A0);
            func_00102BB0(&D_700036A0, &D_700036A0, *(float *)0x700031E4);
            func_001026A0(&D_700038B0, &D_700036A0, &D_700038A0);
            *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(volatile float *)0x700038B0;
            *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(volatile float *)0x700038B8;
        }
    } else {
        if (arg2 == 0) {
            *(unsigned char *)(arg0 + 0xA) = 1;
        } else if (arg2 < 6) {
            *(unsigned char *)(arg0 + 0xA) = 3;
        } else {
            *(unsigned char *)(arg0 + 0xA) = 2;
        }
        if (*(int *)(arg0 + 0x214) != 0) {
            *(unsigned char *)(arg0 + 0xA) = *(unsigned char *)(arg0 + 0xA) | 0x80;
        }

        if (*(unsigned char *)(arg0 + 0x23B) == 0x39) {
            if (arg2 < 6 && *(unsigned char *)(arg0 + 4) == 1 && *(unsigned char *)(arg0 + 5) == 1) {
                func_0017F9E0(arg0);
                return;
            }
            v1b = *(unsigned char *)(arg0 + 4);
            if (v1b == 1 && *(unsigned char *)(arg0 + 5) == 8) {
                func_0017FB90(arg0, v1b);
                return;
            }
            if (v1b == 1 && *(unsigned char *)(arg0 + 5) == 0) {
                func_0017FB90(arg0, v1b);
            }
        } else {
            unsigned short st2 = *(unsigned short *)(arg0 + 0x238);
            if (st2 == 0x1000) {
                *(char *)(arg0 + 0x237) = 1;
                p = *(char **)0x700031D0;
                *(float *)0x70003A20 = func_0011E620(-*(float *)(p + 0x2C), *(float *)(p + 0x24));
                *(float *)(arg0 + 0x218) = func_001B1470(1.5707964f + *(float *)0x70003A20);
                return;
            }
            *(char *)(arg0 + 0x237) = 0;
        }
    }
}

// NEARMISS func_001328D0  (vram 0x001328D0, 0x6D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.65% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered (96.6% on 233). Two residual classes, both genuine compiler artifacts: (1) a delay-slot-fill difference on the bc1f before the D_00244FE0 lookup loop -- target leaves the slot empty (nop) but mwcc233 hoists the independent 'lui at,%hi(D_700038B0)' from the loop body...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001028B8(void *a0, void *a1, void *a2);
extern void func_001029C0(void *a0);
extern void func_00102BB0(void *a0, void *a1, float f);
extern void func_00102C58(void *a0, void *a1, void *a2);
extern void func_001339E0(char *a0, unsigned char *a1);
extern int func_0019AB20(char *arg0, float *arg1, char *arg2, int arg3);
extern int func_0019B6C0(void *a0, void *a1);
extern void func_001B2B10(char *arg0, void *arg1, void *arg2);
extern int func_001B2F70(void *a0, void *a1);
extern int func_001B32F0(int a0, void *a1, float f);
extern int func_001B3390(int a0, void *a1, void *a2, float f);
extern int func_001B3440(int a0, void *a1, void *a2, float f);
extern float D_00244FE0[4][4];
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700038D0[];

void func_001328D0(char *arg0, char *arg1) {
    int i;
    float (*p)[4];
    unsigned char flags;

    *(unsigned char *)(arg1 + 0x60) = 0;
    *(float *)0x700038A0 = 0.0f;
    *(float *)0x700038A4 = 3.0f;
    *(float *)0x700038A8 = 3.0f;
    *(float *)0x700038AC = 1.0f;
    if (*(float *)(arg1 + 0x3C) < 0.0f) {
        *(float *)0x700038A8 = -3.0f;
    }
    func_001B2B10(arg0, D_700038A0, D_700038A0);
    func_001028B8(D_700038A0, arg0 + 0xB0, D_700038A0);
    if (func_001B32F0((int)arg0, D_700038A0, 0.78539816f) != 0) {
        flags = *(unsigned char *)(arg1 + 0x60) | 4;
        goto set_flags;
    }
    if (*(float *)(arg1 + 0x38) < -0.1f) {
        *(float *)0x700038A0 = 0.0f;
        *(float *)0x700038A4 = 0.0f;
        *(float *)0x700038B0 = 0.0f;
        *(float *)0x700038B4 = 0.0f;
        *(float *)0x700038A8 = -3.0f;
        *(float *)0x700038C0 = -3.0f;
        *(float *)0x700038C4 = 0.0f;
        *(float *)0x700038C8 = 0.0f;
        *(float *)0x700038AC = 1.0f;
        *(float *)0x700038CC = 1.0f;
        *(float *)0x700038B8 = 3.0f;
        *(float *)0x700038D0 = 3.0f;
        *(float *)0x700038BC = 1.0f;
        *(float *)0x700038D4 = 0.0f;
        *(float *)0x700038D8 = 0.0f;
        *(float *)0x700038DC = 1.0f;
        func_001029C0(D_700036A0);
        func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg0 + 0xC4));
        func_001026A0(D_700038A0, D_700036A0, D_700038A0);
        func_001026A0(D_700038B0, D_700036A0, D_700038B0);
        func_001026A0(D_700038C0, D_700036A0, D_700038C0);
        func_001026A0(D_700038D0, D_700036A0, D_700038D0);
        func_001028B8(D_700038A0, arg0 + 0xB0, D_700038A0);
        func_001028B8(D_700038B0, arg0 + 0xB0, D_700038B0);
        func_001028B8(D_700038C0, arg0 + 0xB0, D_700038C0);
        func_001028B8(D_700038D0, arg0 + 0xB0, D_700038D0);
        if (func_001B3390((int)arg0, D_700038A0, D_700038B0, 0.78539816f) != 0) {
            flags = *(unsigned char *)(arg1 + 0x60) | 4;
            *(unsigned char *)(arg1 + 0x60) = flags;
        } else if (func_001B3390((int)arg0, D_700038B0, D_700038A0, 0.78539816f) != 0) {
            flags = *(unsigned char *)(arg1 + 0x60) | 4;
            *(unsigned char *)(arg1 + 0x60) = flags;
        }
        if (*(unsigned char *)(arg1 + 0x60) == 0) {
            if (func_001B3390((int)arg0, D_700038C0, D_700038D0, 0.78539816f) != 0) {
                flags = *(unsigned char *)(arg1 + 0x60) | 4;
                goto set_flags;
            }
            if (func_001B3390((int)arg0, D_700038D0, D_700038C0, 0.78539816f) != 0) {
                flags = *(unsigned char *)(arg1 + 0x60) | 4;
set_flags:
                *(unsigned char *)(arg1 + 0x60) = flags;
            }
        }
    }
    if (*(unsigned char *)(arg1 + 0x60) & 4) {
        *(unsigned short *)(arg0 + 0x52) = *(unsigned short *)(arg0 + 0x52) | 1;
    }
    if (*(float *)(arg1 + 0x38) <= 0.0f) {
        p = D_00244FE0;
        i = 0;
        do {
            *(float *)0x700038B0 = p[0][0];
            *(float *)0x700038B4 = p[0][1];
            *(float *)0x700038B8 = p[0][2];
            *(float *)0x700038BC = p[0][3];
            func_001029C0(D_700036A0);
            func_00102C58(D_700036A0, D_700036A0, arg0 + 0xC0);
            func_001026A0(D_700038B0, D_700036A0, D_700038B0);
            func_001028B8(D_700038B0, D_700038B0, arg0 + 0xB0);
            *(float *)0x700038A0 = 0.0f;
            *(float *)0x700038A4 = -15.0f;
            *(float *)0x700038A8 = 0.0f;
            *(float *)0x700038AC = 1.0f;
            if (func_001B3440((int)arg0, D_700038B0, D_700038A0, 0.78539816f) != 0) {
                *(float *)(arg1 + 0x38) = 0.0f;
                *(unsigned char *)(arg1 + 0x60) = *(unsigned char *)(arg1 + 0x60) | 2;
                break;
            }
            i += 1;
            p += 1;
        } while (i < 4);
        if (!(*(unsigned char *)(arg1 + 0x60) & 2) && *(unsigned char *)(arg0 + 4) == 1) {
            unsigned char sub_state = *(unsigned char *)(arg0 + 5);
            if (sub_state != 3 && sub_state != 4) {
                if (func_001B2F70(arg0 + 0xB0, D_700038A0) == 0) {
                    *(float *)0x700038A0 = *(float *)(arg0 + 0xB4) - 10.0f;
                }
                if (!(*(float *)(arg0 + 0xB4) - *(float *)0x700038A0 < 10.0f) && *(float *)(arg1 + 0x38) < -0.5f) {
                    *(unsigned char *)(arg0 + 5) = 4;
                    *(char *)(arg0 + 6) = 0;
                    func_001339E0(arg0, (unsigned char *)arg1);
                }
            }
        }
        *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
        *(float *)0x700038A4 = *(float *)(arg0 + 0xB4);
        *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
        *(float *)0x700038AC = 1.0f;
        *(float *)0x700038A4 = *(float *)0x700038A4 + 18.0f;
        *(unsigned char *)(arg1 + 0x69) = *(unsigned char *)(arg1 + 0x69) & 0xFE;
        if (func_0019B6C0(D_700038A0, arg0 + 0xB0) != 0 && *(unsigned char *)(*(int *)0x700031D0 + 0x1A) == 0x5B) {
            *(unsigned char *)(arg1 + 0x69) = *(unsigned char *)(arg1 + 0x69) | 1;
            *(float *)(arg1 + 0x40) = *(float *)0x700031B4;
        }
    } else {
        *(float *)0x700038A0 = 0.0f;
        *(float *)0x700038A4 = 15.0f;
        *(float *)0x700038A8 = 0.0f;
        *(float *)0x700038AC = 1.0f;
        func_001028B8(D_700038B0, D_700038A0, arg0 + 0xB0);
        *(float *)0x700038B4 = *(float *)0x700038B4 + 0.002f;
        if (func_0019AB20(arg0, (float *)D_700038B0, D_700038A0, 0x80000007) != 0) {
            *(float *)(arg1 + 0x38) = 0.0f;
            *(unsigned char *)(arg1 + 0x60) = *(unsigned char *)(arg1 + 0x60) | 1;
        }
    }
}

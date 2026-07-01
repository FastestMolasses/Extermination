// NEARMISS func_00133640  (vram 0x00133640, 0x394 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.45% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation/scheduling permutation on the parallel struct-copy burst that seeds the two work vectors at 0x700038A0/0x700038B0 from D_00810350/54/58 + arg0->0xB0/B4/B8 plus the two +10.0f offsets. Logic and the entire surrounding state machine (guards, func_0019A570/func_0011DF78/func_001B...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_0021BE40(float *p, char *a);
extern int func_0019A570(float *a, float *b, int c, int d);
extern float func_0011DF78(float d);
extern float func_001B15D0(float *a, float *b);
extern int func_001B1560(char *p, float *q, float ang);
extern int func_0021BED0(float *p);
extern int func_00122BB8(void);

extern float D_00810350;
extern float D_00810354;
extern float D_00810358;
extern float D_00810360;
extern char D_008102B0;
extern unsigned char D_00245120[];
extern unsigned char D_00245130[];

extern volatile float D_700038A0;
extern volatile float D_700038B0;

#define WA0 (*(volatile float *)0x700038A0)
#define WA1 (*(volatile float *)0x700038A4)
#define WA2 (*(volatile float *)0x700038A8)
#define WA3 (*(volatile int *)0x700038AC)
#define WB0 (*(volatile float *)0x700038B0)
#define WB1 (*(volatile float *)0x700038B4)
#define WB2 (*(volatile float *)0x700038B8)
#define WB3 (*(volatile int *)0x700038BC)
#define WD (*(volatile float *)0x70003A20)

int func_00133640(char *a0, char *a1) {
    float dist;

    if (*(unsigned short *)(a1 + 0x5A) != 0) {
        return 0;
    }
    if (*(unsigned char *)(a1 + 0x61) != 0) {
        return 0;
    }
    if (*(short *)(a0 + 0x2C) == 3 || *(short *)(a0 + 0x2C) == 4) {
        return 0;
    }
    if (func_0021BE40((float *)&D_008102B0, a0) != 0) {
        return 0;
    }
    WA0 = *(float *)(a0 + 0xB0);
    WB0 = D_00810350;
    WA2 = *(float *)(a0 + 0xB8);
    WB2 = D_00810358;
    WA3 = 0x3F800000;
    WB3 = 0x3F800000;
    WA1 = *(float *)(a0 + 0xB4) + 10.0f;
    WB1 = D_00810354 + 10.0f;
    if (func_0019A570((float *)&D_700038A0, (float *)&D_700038B0, 6, 0) != 0) {
        return 0;
    }
    dist = func_0011DF78(WA1 - WB1);
    if (!(*(unsigned char *)(a0 + 0xD) & 2) && !(dist < 13.0f)) {
        return 0;
    }
    WB1 = 0.0f;
    WA1 = 0.0f;
    WD = func_001B15D0((float *)&D_700038A0, (float *)&D_700038B0);
    if (!(WD <= 50.0f)) {
        return 0;
    }
    if (func_001B1560(a0, &D_00810360, 0.7853982f) == 0) {
        return 0;
    }
    if (WD <= 24.0f) {
        if (D_00245120[(func_00122BB8() >> 4) & 0xF] != 0) {
            if (!(*(unsigned char *)(a0 + 0xD) & 2) && func_0021BED0((float *)&D_008102B0) == 0) {
                *(unsigned char *)(a1 + 0x63) = 0;
                *(unsigned char *)(a0 + 5) = 7;
                *(unsigned char *)(a0 + 6) = 0;
                return 1;
            }
        }
        if (dist < 13.0f) {
            *(unsigned char *)(a1 + 0x64) = (func_00122BB8() >> 0xB) & 1;
            *(unsigned char *)(a1 + 0x63) = 0;
            *(unsigned char *)(a0 + 5) = 5;
            *(unsigned char *)(a0 + 6) = 0;
            return 1;
        }
        *(unsigned char *)(a1 + 0x63) = 0;
        *(unsigned char *)(a0 + 5) = 5;
        *(unsigned char *)(a0 + 6) = 0;
        *(unsigned char *)(a1 + 0x64) = 2;
        return 1;
    }

    {
        unsigned char sel = D_00245130[(func_00122BB8() >> 0x11) & 0xF];
        if (sel == 0) {
            return 0;
        }
        if (sel == 2) {
            if (*(unsigned char *)(a0 + 0xD) & 1) {
                *(unsigned char *)(a1 + 0x63) = 0;
                *(unsigned char *)(a0 + 5) = 6;
                *(unsigned char *)(a0 + 6) = 0;
                return 1;
            }
        }
        *(unsigned char *)(a1 + 0x63) = 0;
        *(unsigned char *)(a0 + 5) = 5;
        *(unsigned char *)(a0 + 6) = 0;
        *(unsigned char *)(a1 + 0x64) = 2;
        return 1;
    }
}

// NEARMISS func_00132FB0  (vram 0x00132FB0, 0x434 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.62% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure/semantics fully recovered (same family as func_001416D0/func_0012F6C0: seeds scratch vec at 0x700038A0, calls func_001B2B10/func_001028B8/func_001B2BF0(pi/4) to get a flags bitmask, branches on flags&1/flags&4 into anim-clip selection via func_00122BB8/func_001B3580/func_001B37D0, ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001B2B10(void *a, void *b, void *c);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_001B2BF0(char *, int *, int *, float);
extern int func_00122BB8(void);
extern float func_001B3580(char *, float *);
extern float func_001B37D0(char *, float, float);
extern int func_0019A570(void *a, void *b, int c, int d);
extern int func_0019AD00(char *, void *, int);

extern int D_700038A0;
extern int D_700038B0;
extern int D_700038D0;
extern unsigned char D_00810700;

int func_00132FB0(char *arg0, char *arg1) {
    float sp40[4];
    int flags;
    char *p;
    unsigned char c;

    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0x40400000;
    *(int *)0x700038A8 = 0x41200000;
    *(int *)0x700038AC = 0x3F800000;
    func_001B2B10(arg0, &D_700038A0, &D_700038A0);
    func_001028B8(&D_700038A0, &D_700038A0, arg0 + 0xB0);
    flags = func_001B2BF0(arg0, &D_700038A0, &D_700038D0, 0.7853981852531433f);
    if (!(flags & 1)) {
        if (flags & 4) {
            arg1[0x63] = 0;
            arg1[0x62] = (func_00122BB8() >> 8 & 0x1F) + 1;
            p = *(char **)0x700031D0;
            sp40[0] = *(float *)(p + 0x24);
            sp40[1] = *(float *)(p + 0x28);
            sp40[2] = *(float *)(p + 0x2C);
            sp40[3] = 1.0f;
            *(float *)(arg1 + 0x30) = func_001B3580(arg0, sp40);
        } else {
            arg1[0x62] = func_00122BB8() >> 5 & 7;
            *(float *)(arg1 + 0x30) = func_001B37D0(arg0, 10.0f, 15.0f);
        }
        return 1;
    }
    if (flags & 4) {
        p = *(char **)0x700031D0;
        sp40[0] = *(float *)(p + 0x24);
        sp40[1] = *(float *)(p + 0x28);
        sp40[2] = *(float *)(p + 0x2C);
        sp40[3] = 1.0f;
        c = D_00810700;
        if (c == 7 || c == 8 || c == 0x10 || c == 0x12 || c == 0x13 ||
            15.0f + *(float *)(arg0 + 0xB4) < *(float *)0x700038D0 ||
            *(unsigned char *)(arg1 + 0x61) != 0 ||
            (*(unsigned char *)(arg0 + 0xD) & 2) ||
            *(unsigned char *)(p + 0x1A) == 0x46) {
            arg1[0x63] = 0;
            arg1[0x62] = (func_00122BB8() >> 8 & 0x1F) + 1;
            *(float *)(arg1 + 0x30) = func_001B3580(arg0, sp40);
            return 1;
        }
        {
            int mask;
            if (*(unsigned short *)(arg1 + 0x56) != 0) {
                mask = 3;
            } else {
                mask = 1;
            }
            if (mask & (func_00122BB8() >> 14)) {
                float t1;
                float t2;
                *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
                *(float *)0x700038A4 = *(float *)(arg0 + 0xB4);
                *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
                *(float *)0x700038AC = *(float *)(arg0 + 0xBC);
                t1 = *(float *)0x700038A4 + 10.0f;
                *(float *)0x700038B0 = *(float *)(arg0 + 0xB0);
                *(float *)0x700038B4 = *(float *)(arg0 + 0xB4);
                *(float *)0x700038B8 = *(float *)(arg0 + 0xB8);
                *(float *)0x700038BC = *(float *)(arg0 + 0xBC);
                *(float *)0x700038A4 = t1;
                *(int *)0x700038AC = 0x3F800000;
                t2 = *(float *)0x700038B4 + 40.0f;
                *(float *)0x700038B4 = t2;
                *(int *)0x700038BC = 0x3F800000;
                if (func_0019A570(&D_700038A0, &D_700038B0, 6, 0) == 0) {
                    *(int *)0x700038A0 = 0;
                    *(float *)0x700038A4 = 15.5f;
                    *(int *)0x700038A8 = 0x41200000;
                    *(int *)0x700038AC = 0x3F800000;
                    func_001B2B10(arg0, &D_700038A0, &D_700038A0);
                    func_001028B8(&D_700038A0, &D_700038A0, arg0 + 0xB0);
                    if (func_0019AD00(arg0, &D_700038A0, 7) == 0) {
                        arg0[5] = 3;
                        arg0[6] = 0;
                        return 2;
                    }
                }
            }
        }
        arg1[0x63] = 0;
        arg1[0x62] = (func_00122BB8() >> 8 & 0x1F) + 1;
        *(float *)(arg1 + 0x30) = func_001B3580(arg0, sp40);
        return 1;
    }
    if (*(float *)(arg0 + 0xB4) - 15.0f < *(float *)0x700038D0) {
        return 0;
    }
    arg1[0x62] = func_00122BB8() >> 9 & 7;
    *(float *)(arg1 + 0x30) = func_001B37D0(arg0, 10.0f, 15.0f);
    return 1;
}

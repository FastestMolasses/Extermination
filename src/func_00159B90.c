// NEARMISS func_00159B90  (vram 0x00159B90, 0x2D4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.72% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered: outer 4-state actor dispatch (p[4], cases 0-3) with case1 holding a nested 4-state sub-machine (p[5]) that probes func_00157CE0/func_001BA1F0 and drives a shared func_001026A0/func_00158590 pose-blend tail + vtable call through p+0x4C. Verified: idiom-20 ascending-...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern int func_001B0FD0(unsigned char *e, int a, int b);
extern void func_001C6380(unsigned char *p);
extern int func_00157CE0(unsigned char *e, int a, int b);
extern void func_00102948(void *dst, void *src);
extern void func_001FB9F0(int a, int b, int c, int d);
extern int func_001BA1A0(unsigned char *e, void *tbl, int a, int b);
extern int func_001BA1F0(unsigned char *e, int a, int b);
extern void func_001C5570(unsigned char *p, void *v, int a, int b);
extern void func_001026A0(void *v, void *a, void *b);
extern void func_00158590(unsigned char *p, int a, int b);
extern void func_001B17A0(unsigned char *p);
extern void func_001AFC10(unsigned char *e, int a);

extern void *D_00275460;
extern int D_002470E0;
extern float D_00810350;
extern float D_00810370;
extern float D_00810710;
extern float D_00810720;
extern float D_700038A0[4];

void func_00159B90(unsigned char *p) {
    unsigned char st;
    unsigned char st2;
    int r;
    unsigned char *e;

    st = p[4];
    e = p + 0x1F0;
    switch (st) {
    case 0:
        if (func_001B0FD0(e, 2, 3) == 0) {
            func_001C6380(p);
            p[0xA] = 0;
            *(void **)(p + 0x30) = &D_00275460;
            *(short *)(p + 0x34) = 2;
            p[0] = 1;
            *(volatile float *)0x700038A0 = 0.0f;
            *(volatile float *)0x700038A4 = 1.0f;
            *(volatile float *)0x700038A8 = 0.0f;
            *(volatile float *)0x700038AC = 1.0f;
            func_001C5570(p, D_700038A0, 0x76, 1);
            return;
        }
        return;
    case 1:
        st2 = p[5];
        switch (st2) {
        case 0:
            r = func_00157CE0(e, 2, 3);
            if (r != 0) {
                func_00102948(&D_00810710, &D_00810350);
                func_00102948(&D_00810720, &D_00810370);
                func_001FB9F0(0x3E8, 0x1000, 0x1000, 0x1000);
                if (r != 2) {
                    if (r == 3) {
                        p[5] = p[5] + 1;
                    } else {
                        p[5] = 3;
                    }
                } else {
                    p[5] = 2;
                }
            }
            break;
        case 1:
            p[5] = st2 + 1;
            func_001BA1A0(e, &D_002470E0, 2, 3);
            break;
        case 2:
            if (func_001BA1F0(e, 2, 3) != 0) {
                p[0xB] = 0;
                p[0] = 1;
                p[5] = 0;
            }
            break;
        case 3:
            if (func_001BA1F0(e, 2, 3) != 0) {
                p[5] = p[5] + 1;
            }
            break;
        }
        *(volatile float *)0x700038A0 = -1.75f;
        *(volatile float *)0x700038A4 = 1.7f;
        *(volatile float *)0x700038A8 = 2.03f;
        *(volatile float *)0x700038AC = 1.0f;
        func_001026A0(D_700038A0, p + 0xD0, D_700038A0);
        func_00158590(p, 1, -2);
        *(volatile float *)0x700038A0 = 1.75f;
        *(volatile float *)0x700038A4 = 1.7f;
        *(volatile float *)0x700038A8 = 2.03f;
        *(volatile float *)0x700038AC = 1.0f;
        func_001026A0(D_700038A0, p + 0xD0, D_700038A0);
        func_00158590(p, 1, -2);
        func_001B17A0(p);
        (*(void (**)(unsigned char *))(p + 0x4C))(p);
        return;
    case 2:
        p[4] = st + 1;
        return;
    case 3:
        func_001AFC10(e, 3);
        break;
    }
}

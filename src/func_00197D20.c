// NEARMISS func_00197D20  (vram 0x00197D20, 0x324 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.28% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered including two fixed callee-signature bugs (func_001D2610 is a single-float-arg call, not (int,int) as m2c misdecoded; func_0021B9A0 is (int,float,float)). The inner switch's case-declaration ORDER (12/41, default, 13/42, matching the literal fallthrough shape inferr...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_00102948(void *dst, void *src);
extern float func_0011DE90(float a); /* cos */
extern float func_0011E2A8(float a); /* sin */
extern float func_0011E620(float a, float b);
extern float func_0011E748(float a);
extern void func_0018C4B0(void *dst, float val, float rate);
extern void func_0018C6A0(void *dst, void *src, float rate);
extern int func_0018D7B0(char *a, int b);
extern void func_00191210(void);
extern void func_001912B0(char *a);
extern void func_00197490(int a);
extern void func_00197740(char *a, char *b);
extern void func_00197870(char *a, char *b, int c);
extern void func_00198440(char *a, char *b, int c);
extern void func_001999C0(char *a, int b);
extern float func_001B1470(float a);
extern void func_001D2610(float a);
extern void func_001D2830(int a, int b);
extern void func_0021B9A0(int a, float b, float c);
extern void func_0022E7F0(char *a);

extern float D_008105D0;
extern float D_008105D4;
extern float D_008105D8;
extern unsigned char D_00810CA5;
extern unsigned char D_00810CA7;
extern char D_70003040[];

void func_00197D20(char *arg0, char *arg1) {
    unsigned char st;
    int mode;
    float dx, dz, d2, ang;

    st = *(unsigned char *)(arg0 + 1);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 1) = st + 1;
        *(char *)(arg0 + 2) = 0;
        *(unsigned char *)(arg0 + 0x8B) = 0;
        func_00102948(&D_70003040, arg1 + 0xA0);
        /* fallthrough */
    case 1:
        mode = *(int *)(arg1 + 0x230);
        if (mode != 0xD && mode != 0x2A) {
            *(unsigned char *)(arg0 + 1) = 3;
        } else {
            if (*(unsigned char *)(arg1 + 0x1F1) == 1) {
                *(unsigned char *)(arg0 + 1) = *(unsigned char *)(arg0 + 1) + 1;
            }
            func_00197740(arg0, arg1);
        }
        func_0018D7B0(arg0, 2);
        func_0018C6A0(arg0 + 0x10, &D_008105D0, 4.0f);
        func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 4.0f);
        if (D_008105D4 < 23.0f + *(float *)(arg1 + 0xA4)) {
            dx = D_008105D0 - *(float *)(arg1 + 0xA0);
            dz = D_008105D8 - *(float *)(arg1 + 0xA8);
            d2 = func_0011E748(dx * dx + dz * dz);
            *(float *)0x70003A20 = d2;
            if (d2 < 8.0f) {
                ang = func_001B1470(func_0011E620(dx, dz));
                *(float *)0x70003A24 = ang;
                D_008105D0 = *(float *)(arg1 + 0xA0) + 8.0f * func_0011E2A8(ang);
                D_008105D8 = *(float *)(arg1 + 0xA8) + 8.0f * func_0011DE90(ang);
            }
        }
        func_00191210();
        return;
    case 4:
        *(int *)(arg0 + 0x68) = 0;
        func_001D2610(0.0f);
        if (D_00810CA7 == 9) {
            func_0021B9A0(0, 0.0f, 0.0f);
            func_001D2830(1, 1);
        }
        /* fallthrough */
    case 2:
        mode = *(int *)(arg1 + 0x230);
        switch (mode) {
        case 12:
        case 41:
            if (*(unsigned char *)(arg0 + 0x8B) == 0) {
                func_00102948(arg1 + 0xA0, &D_70003040);
            }
            *(char *)(arg0 + 6) = 2;
            *(unsigned char *)(arg0 + 1) = 2;
            func_001999C0(arg1, 0);
            func_00198440(arg0, arg1, 2);
            func_001912B0(arg1);
            break;
        default:
            *(unsigned char *)(arg0 + 1) = 3;
            break;
        case 13:
        case 42:
            func_00197870(arg0, arg1, 0);
            if (D_00810CA5 == 6) {
                func_0022E7F0(arg1);
            }
            break;
        }
        func_00191210();
        return;
    case 3:
        func_00197490(1);
        return;
    }
}

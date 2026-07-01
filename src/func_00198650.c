// NEARMISS func_00198650  (vram 0x00198650, 0x2D4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.17% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Idiom-13 conditional-branch delay-slot NOP (3 instances, mwcc fills with next compare's speculatable li) + FP odd/even register-coloring on the arg0+0x68 float ramp. Body/control-flow fully recovered and verified line-for-line against the splat .s; both residual classes are documented genuine mwc...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_00102948(void *, void *, ...);
extern void func_001912B0(char *);
extern void func_00197490(char *, char *, int);
extern void func_00197870(char *, char *, int);
extern void func_00198050(char *, char *);
extern void func_00198440(char *, char *, int);
extern int func_001999C0(char *, int);
extern void func_001D2610(float);
extern void func_001D2830(int, int);
extern unsigned char D_00810CA7;
extern unsigned short D_00810E70;
extern int D_70003040;

void func_00198650(char *arg0, char *arg1) {
    unsigned char st = *(unsigned char *)(arg0 + 1);
    float f;

    switch (st) {
    default:
        return;
    case 0:
        *(short *)(arg0 + 8) = 0;
        *(unsigned char *)(arg0 + 1) = *(unsigned char *)(arg0 + 1) + 1;
        *(char *)(arg0 + 2) = 0;
        *(float *)(arg0 + 0x68) = 0.0f;
        func_001912B0(arg1);
        *(unsigned char *)(arg0 + 0x8B) = 0;
        func_00102948(&D_70003040, arg1 + 0xA0);
        /* fallthrough */
    case 1:
        if (*(int *)(arg1 + 0x230) != 0xC && *(int *)(arg1 + 0x230) != 0x29) {
            *(unsigned char *)(arg0 + 1) = 3;
            return;
        }
        func_00198050(arg0, arg1);
        if (*(unsigned char *)(arg1 + 0x1F1) == 1) {
            *(unsigned char *)(arg0 + 1) = *(unsigned char *)(arg0 + 1) + 1;
            func_001999C0(arg1, 0);
            func_00198440(arg0, arg1, 1);
        }
        return;
    case 2: {
        int st230 = *(int *)(arg1 + 0x230);
        if (st230 != 0xC && st230 != 0x29) {
            func_001999C0(arg1, 1);
            st230 = *(int *)(arg1 + 0x230);
            if (st230 == 0xD || st230 == 0x2A) {
                if (*(unsigned char *)(arg0 + 0x8B) == 0) {
                    func_00102948(arg1 + 0xA0, &D_70003040, 1);
                }
                *(unsigned char *)(arg0 + 0x6) = 1;
                *(unsigned char *)(arg0 + 1) = 4;
                func_00197870(arg0, arg1, 1);
                return;
            }
            *(float *)(arg0 + 0x68) = 0.0f;
            func_001D2610(0.0f);
            if (D_00810CA7 == 9) {
                func_001D2830(1, 1);
            }
            func_00197490(arg0, arg1, 0);
            return;
        }
        func_00198440(arg0, arg1, 0);
        if (func_001999C0(arg1, 1) == 1) {
            if (D_00810E70 & 4) {
                f = *(float *)(arg0 + 0x68) + 0.02f;
                *(float *)(arg0 + 0x68) = f;
                if (!(f <= 1.0f)) {
                    *(float *)(arg0 + 0x68) = 1.0f;
                }
            } else if (D_00810E70 & 1) {
                f = *(float *)(arg0 + 0x68) - 0.02f;
                *(float *)(arg0 + 0x68) = f;
                if (f < 0.0f) {
                    *(float *)(arg0 + 0x68) = 0.0f;
                }
            }
        }
        return;
    }
    case 3:
        *(unsigned char *)(arg0 + 1) = st + 1;
        *(float *)(arg0 + 0x68) = 0.0f;
        func_001D2610(0.0f);
        if (D_00810CA7 == 9) {
            func_001D2830(1, 1);
        }
        /* fallthrough */
    case 4:
        func_00197490(arg0, arg1, 0);
        return;
    }
}

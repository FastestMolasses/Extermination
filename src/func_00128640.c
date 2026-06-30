// NEARMISS func_00128640  (vram 0x00128640, 0x1A4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.62% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring permutation. Full logic recovered (three early-return gates on 0x70003B8D / D_0028A9A0 / D_008104E0==0x28; squared distance of the transformed vec via func_001028D0 + func_0011E748; two distance bands <30.1 and <60.1 each sub-dispatched on func_00128600(4)/(0) to set the stat...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Distance-band state setter. Bails (return 0) on three gates: scratchpad byte
// 0x70003B8D, the short D_0028A9A0, and D_008104E0 == 0x28. Otherwise transforms
// a point via func_001028D0(scratch 0x70003600, D_00810350, arg0+0xB0), takes the
// squared length of the resulting vec (x*x+y*y+z*z), and runs it through
// func_0011E748. If the distance > 70.0 -> return 0. For <30.1 it polls
// func_00128600(4) and sets the state byte arg0+5 to 2/3/5; for <60.1 it polls
// func_00128600(0) -> 2/4/5; else state 2. Then clears arg0+6/+7 and returns 1.
//
// NEARMISS 99.62% via mwcc 2.3.3 (991202 reaches 84.86%). Body byte-identical;
// sole residual is FP register coloring of the squared-length sum (target uses
// the f0/f1 odd-half pair + f2, mwcc spreads to f4). FP-coloring permuter.
extern int func_001028D0(void *a, void *b, void *c);
extern float func_0011E748(float d);
extern int func_00128600(int a);
extern short D_0028A9A0;
extern int D_00810350;
extern int D_008104E0;
extern float D_70003600;

int func_00128640(char *arg0) {
    float x;
    float y;
    float z;
    float d;
    int r;

    if (*(unsigned char *)0x70003B8D != 0) {
        return 0;
    }
    if (D_0028A9A0 != 0) {
        return 0;
    }
    if (D_008104E0 == 0x28) {
        return 0;
    }
    func_001028D0(&D_70003600, &D_00810350, arg0 + 0xB0);
    x = *(float *)0x70003600;
    y = *(float *)0x70003604;
    z = *(float *)0x70003608;
    d = func_0011E748(y * y + x * x + z * z);
    if (d > 70.0f) {
        return 0;
    }
    if (d < 30.1f) {
        r = func_00128600(4);
        if (r == 0) {
            *(char *)(arg0 + 5) = 2;
        } else if (r == 1) {
            *(char *)(arg0 + 5) = 3;
        } else {
            *(char *)(arg0 + 5) = 5;
        }
    } else if (d < 60.1f) {
        r = func_00128600(0);
        if (r == 0) {
            *(char *)(arg0 + 5) = 2;
        } else if (r == 1) {
            *(char *)(arg0 + 5) = 4;
        } else {
            *(char *)(arg0 + 5) = 5;
        }
    } else {
        *(char *)(arg0 + 5) = 2;
    }
    *(char *)(arg0 + 6) = 0;
    *(char *)(arg0 + 7) = 0;
    return 1;
}

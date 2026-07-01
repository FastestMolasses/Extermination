// NEARMISS func_001963A0  (vram 0x001963A0, 0x5C4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.59% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order permutation: mwcc233 colors the switch(arg0[1]) dispatch temp into $a1 (target uses $v1, loading it via $a0 before the paddub self/other copies), which cascades into a repeated FP-register-coloring swap ($f1/$f3 vs $f1/$f2) in the position-clamp float update, occurring i...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 96.6% (mwcc233; 991202 = 91.4%) — logic fully recovered.
//
// Actor "approach/anim distance" tick, called with arg0 = self actor record, arg1 = other/target
// actor record (world position at arg1+0xA0/0xA4/0xA8, "band" float at arg1+0xB4).
//
// First dispatch: a small state machine on self[1] (0..4) that walks self's transform block at
// self+0x10..0x1C (a position/xy0/scale-ish float set) toward one of two fixed targets depending
// on whether arg1[0xA4] (target's Z/height) is <= 176.9:
//   state 0: zero self[8](s16)/self[2], set self+0x10..0x1C to a fixed pose A (>176.9: state->3)
//            or pose B (<=176.9: state->1), and advance self[1] to state+1 (0->1) as a side effect
//            before the branch overwrites it again for the two sub-cases.
//   state 1: smooth self+0x10 toward &D_008105D0 (func_0018C920, step 0.2), push self[0x14] into
//            D_008105D0 (func_0018C850); if still >176.9, snap to pose A, nudge self[0x14] by
//            (42+arg1[0xA4]-self[0x14]) clamped to <=266, re-sync D_008105D0, and jump to state 4.
//   state 2: same as state 1 but starts from func_00102948(&D_008105D0, self+0x10) (hard copy
//            instead of smoothed step); same nudge/clamp/jump-to-4 on the >176.9 branch.
//   state 3: nudge self[0x14] toward 42+arg1[0xA4] (clamp <=266), smooth self+0x10 toward
//            D_008105D0 (0.2 step), push self[0x14]; if arg1[0xA4]<=176.9 now, snap to pose B and
//            jump to state 2.
//   state 4: same nudge/clamp then func_0018C850(&D_008105D0, self[0x14], 0.2) (no func_0018C920
//            step); same <=176.9 -> pose B -> state 2 transition.
//
// Second dispatch: on arg1[0x230] (an incoming event/mode code), drives self's secondary
// transform block self+0x20..0x28:
//   9: if arg1[0xA4] > 176.9, sets D_00810702=3, sets self+0x20..0x28 to a fixed pose, smooth-steps
//      self+0x20 toward &D_008105E0 (func_0018C6A0, 0.3), pushes self[0x24] into D_008105E0
//      (func_0018C4B0), calls func_0018D7B0(self, 5) and returns immediately (skips the shared
//      tail below). Otherwise sets D_00810702=5 and smooth-steps self+0x20 toward arg1+0xB0
//      (func_0018C6A0, 0.4), then falls into the shared tail.
//   6, 7: smooth-steps self+0x20 toward arg1+0xB0 (func_0018C6A0, 0.4), falls into shared tail.
//   8: snaps self+0x20 toward arg1+0xA0 (func_0018C920, 0.1), falls into shared tail.
//   default: resets self[5]/self[6]/self[1] (self[5]=1 and self[0xA0]=0x28 if arg1[0xA4]>176.9,
//            else self[5]=0), snaps self+0x20 toward arg1+0xA0 (func_0018C920, 0.1), falls into
//            shared tail.
// Shared tail: func_0018C4B0(self+0x20, arg1[0xB4] + self[0x8C], 0.3), func_0018C0C0(self),
// func_0018D7B0(self, 5).

extern int func_0018C920(char *arg0, void *arg1, float maxstep);
extern void func_0018C850(void *dst, float val, float rate);
extern void func_0018C4B0(void *dst, float val, float rate);
extern void func_0018C6A0(void *dst, void *src, float rate);
extern void func_00102948(void *dst, void *src);
extern void func_0018C0C0(char *a0);
extern void func_0018D7B0(char *a0, int mode);

extern float D_008105D0;
extern float D_008105E0;
extern unsigned char D_00810702;

void func_001963A0(char *arg0, char *arg1)
{
    int st;
    float f;
    int mode;

    st = *(unsigned char *)(arg0 + 1);
    switch (st) {
    case 0:
        *(short *)(arg0 + 8) = 0;
        *(unsigned char *)(arg0 + 1) = st + 1;
        *(char *)(arg0 + 2) = 0;
        if (!(*(float *)(arg1 + 0xA4) <= 176.9f)) {
            *(int *)(arg0 + 0x10) = 0x42E60000;
            *(float *)(arg0 + 0x14) = 266.0f;
            *(int *)(arg0 + 0x18) = 0x43664CCD;
            *(int *)(arg0 + 0x1C) = 0x3F800000;
            *(unsigned char *)(arg0 + 1) = 3;
        } else {
            *(int *)(arg0 + 0x10) = 0x42E40000;
            *(float *)(arg0 + 0x14) = 173.7f;
            *(int *)(arg0 + 0x18) = 0x43416666;
            *(int *)(arg0 + 0x1C) = 0x3F800000;
            *(unsigned char *)(arg0 + 1) = 1;
        }
        break;

    case 1:
        func_0018C920(arg0 + 0x10, &D_008105D0, 0.2f);
        func_0018C850(&D_008105D0, *(float *)(arg0 + 0x14), 0.2f);
        if (!(*(float *)(arg1 + 0xA4) <= 176.9f)) {
            *(int *)(arg0 + 0x10) = 0x42E60000;
            *(float *)(arg0 + 0x14) = 266.0f;
            *(int *)(arg0 + 0x18) = 0x43664CCD;
            *(int *)(arg0 + 0x1C) = 0x3F800000;
            f = *(float *)(arg0 + 0x14);
            *(float *)(arg0 + 0x14) = f + ((42.0f + *(float *)(arg1 + 0xA4)) - f);
            if (!(*(float *)(arg0 + 0x14) <= 266.0f)) {
                *(float *)(arg0 + 0x14) = 266.0f;
            }
            func_00102948(&D_008105D0, arg0 + 0x10);
            *(unsigned char *)(arg0 + 1) = 4;
        }
        break;

    case 2:
        func_00102948(&D_008105D0, arg0 + 0x10);
        if (!(*(float *)(arg1 + 0xA4) <= 176.9f)) {
            *(int *)(arg0 + 0x10) = 0x42E60000;
            *(float *)(arg0 + 0x14) = 266.0f;
            *(int *)(arg0 + 0x18) = 0x43664CCD;
            *(int *)(arg0 + 0x1C) = 0x3F800000;
            f = *(float *)(arg0 + 0x14);
            *(float *)(arg0 + 0x14) = f + ((42.0f + *(float *)(arg1 + 0xA4)) - f);
            if (!(*(float *)(arg0 + 0x14) <= 266.0f)) {
                *(float *)(arg0 + 0x14) = 266.0f;
            }
            func_00102948(&D_008105D0, arg0 + 0x10);
            *(unsigned char *)(arg0 + 1) = 4;
        }
        break;

    case 3:
        f = *(float *)(arg0 + 0x14);
        *(float *)(arg0 + 0x14) = f + ((42.0f + *(float *)(arg1 + 0xA4)) - f);
        if (!(*(float *)(arg0 + 0x14) <= 266.0f)) {
            *(float *)(arg0 + 0x14) = 266.0f;
        }
        func_0018C920(arg0 + 0x10, &D_008105D0, 0.2f);
        func_0018C850(&D_008105D0, *(float *)(arg0 + 0x14), 0.2f);
        if (*(float *)(arg1 + 0xA4) <= 176.9f) {
            *(int *)(arg0 + 0x10) = 0x42E40000;
            *(float *)(arg0 + 0x14) = 173.7f;
            *(int *)(arg0 + 0x18) = 0x43416666;
            *(int *)(arg0 + 0x1C) = 0x3F800000;
            *(unsigned char *)(arg0 + 1) = 2;
        }
        break;

    case 4:
        f = *(float *)(arg0 + 0x14);
        *(float *)(arg0 + 0x14) = f + ((42.0f + *(float *)(arg1 + 0xA4)) - f);
        if (!(*(float *)(arg0 + 0x14) <= 266.0f)) {
            *(float *)(arg0 + 0x14) = 266.0f;
        }
        func_0018C850(&D_008105D0, *(float *)(arg0 + 0x14), 0.2f);
        if (*(float *)(arg1 + 0xA4) <= 176.9f) {
            *(int *)(arg0 + 0x10) = 0x42E40000;
            *(float *)(arg0 + 0x14) = 173.7f;
            *(int *)(arg0 + 0x18) = 0x43416666;
            *(int *)(arg0 + 0x1C) = 0x3F800000;
            *(unsigned char *)(arg0 + 1) = 2;
        }
        break;
    }

    mode = *(int *)(arg1 + 0x230);
    switch (mode) {
    case 9:
        if (!(*(float *)(arg1 + 0xA4) <= 176.9f)) {
            D_00810702 = 3;
            *(int *)(arg0 + 0x20) = 0x42E60000;
            *(float *)(arg0 + 0x24) = 254.0f;
            *(int *)(arg0 + 0x28) = 0x43764CCD;
            func_0018C6A0(arg0 + 0x20, &D_008105E0, 0.3f);
            func_0018C4B0(&D_008105E0, *(float *)(arg0 + 0x24), 0.3f);
            func_0018D7B0(arg0, 5);
            return;
        }
        D_00810702 = 5;
        func_0018C6A0(arg1 + 0xB0, arg0 + 0x20, 0.4f);
        goto shared;

    case 6:
    case 7:
        func_0018C6A0(arg1 + 0xB0, arg0 + 0x20, 0.4f);
        goto shared;

    case 8:
        func_0018C920(arg1 + 0xA0, arg0 + 0x20, 0.1f);
        goto shared;

    default:
        if (!(*(float *)(arg1 + 0xA4) <= 176.9f)) {
            *(char *)(arg0 + 5) = 1;
            *(short *)(arg0 + 0xA0) = 0x28;
        } else {
            *(char *)(arg0 + 5) = 0;
        }
        *(char *)(arg0 + 6) = 0;
        *(unsigned char *)(arg0 + 1) = 0;
        func_0018C920(arg1 + 0xA0, arg0 + 0x20, 0.1f);
        goto shared;
    }

shared:
    func_0018C4B0(arg0 + 0x20, *(float *)(arg1 + 0xB4) + *(float *)(arg0 + 0x8C), 0.3f);
    func_0018C0C0(arg0);
    func_0018D7B0(arg0, 5);
}

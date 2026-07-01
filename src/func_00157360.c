// NEARMISS func_00157360  (vram 0x00157360, 0x248 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.71% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring + arg-eval-order + FP odd/even-half coloring scheduling artifacts in a 0..4 switch state machine. Body/control-flow byte-shape correct (best 91.7% mwcc233). Residuals: (1) state-0 func_00102948 arg-eval order reversed by mwcc across both branches + default-keyframe store-block c...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (mwcc233 91.7%, mwcc991202 88.0%) -- logic fully recovered; residual is
// register-coloring / arg-eval-order / FP-odd-half-coloring scheduling only.
//
// A short state machine (state byte at arg1+4) driving a scripted camera/effect
// intro, dispatched ascending 0..4 (mwcc lowers to the target's descending compare
// cascade). arg2 is the camera record; arg2+0x10 is a float timer.
//   state 0: build two camera keyframe sets. If the region id D_00810700 == 7 use
//     the disc-authored presets (four func_00102948 copies from D_008105D0/E0 and
//     D_008101F0/0200 into arg2+0x20/+0x30); otherwise write the default keyframe
//     floats into D_008105D0..EC (43A78000, 436F0000, 44117333, 1.0f / 43A78000,
//     436D0000, 4405C000, 1.0f) and load them via func_00102948. Then zero the timer
//     and advance the state.
//   state 1: advance the timer by 1.0; once it passes 30.0, advance the state.
//   state 2: set bit (arg0+0x2E) in the per-region flag byte D_00810841[D_00810700],
//     fire func_001FB9F0(0x3EE, 0x1000, 0x1000, 0x1000) (a sound/rumble cue), zero
//     the timer, advance the state.
//   state 3: advance the timer by 1.0; once it passes 60.0, advance the state.
//   state 4: func_001B0460(1) and return 1 (sequence finished). All other states 0.
//
// mwcc 2.3.3 gets closest. Remaining deltas are pure compiler artifacts: in state 0
// mwcc evaluates the two func_00102948 args in the opposite order and colors the
// default-keyframe store block differently; the state-1/3 `add.s` picks the other
// odd/even half of the FP pair; the state-2 `1 << x` shift is colored to a different
// GPR. Body and control flow are byte-shape-correct -- parked as documented nearmiss.
extern void func_00102948(int *dst, int *src);
extern void func_001B0460(int a);
extern void func_001FB9F0(int a, int b, int c, int d);
extern int D_008101F0;
extern int D_00810200;
extern int D_008105D0;
extern int D_008105D4;
extern int D_008105D8;
extern int D_008105DC;
extern int D_008105E0;
extern int D_008105E4;
extern int D_008105E8;
extern int D_008105EC;
extern unsigned char D_00810700;
extern unsigned char D_00810841;

int func_00157360(char *arg0, char *arg1, char *arg2) {
    unsigned char st;
    unsigned char *p;
    float f;

    st = *(unsigned char *)(arg1 + 4);
    switch (st) {
    case 0:
        if (D_00810700 == 7) {
            func_00102948(&D_008105D0, (int *)(arg2 + 0x20));
            func_00102948(&D_008105E0, (int *)(arg2 + 0x30));
            func_00102948(&D_008101F0, (int *)(arg2 + 0x20));
            func_00102948(&D_00810200, (int *)(arg2 + 0x30));
        } else {
            D_008105D0 = 0x43A78000;
            D_008105D4 = 0x436F0000;
            D_008105D8 = 0x44117333;
            D_008105DC = 0x3F800000;
            D_008105E0 = 0x43A78000;
            D_008105E4 = 0x436D0000;
            D_008105E8 = 0x4405C000;
            D_008105EC = 0x3F800000;
            func_00102948(&D_008101F0, &D_008105D0);
            func_00102948(&D_00810200, &D_008105E0);
        }
        *(int *)(arg2 + 0x10) = 0;
        *(unsigned char *)(arg1 + 4) = *(unsigned char *)(arg1 + 4) + 1;
        goto ret0;
    case 1:
        f = *(float *)(arg2 + 0x10) + 1.0f;
        *(float *)(arg2 + 0x10) = f;
        if (f <= 30.0f) {
            goto ret0;
        }
        *(unsigned char *)(arg1 + 4) = *(unsigned char *)(arg1 + 4) + 1;
        goto ret0;
    case 2:
        p = &D_00810841 + D_00810700;
        *p = *p | (1 << *(unsigned char *)(arg0 + 0x2E));
        func_001FB9F0(0x3EE, 0x1000, 0x1000, 0x1000);
        *(int *)(arg2 + 0x10) = 0;
        *(unsigned char *)(arg1 + 4) = *(unsigned char *)(arg1 + 4) + 1;
        goto ret0;
    case 3:
        f = *(float *)(arg2 + 0x10) + 1.0f;
        *(float *)(arg2 + 0x10) = f;
        if (f <= 60.0f) {
            goto ret0;
        }
        *(unsigned char *)(arg1 + 4) = *(unsigned char *)(arg1 + 4) + 1;
        goto ret0;
    case 4:
        func_001B0460(1);
        return 1;
    }
ret0:
    return 0;
}

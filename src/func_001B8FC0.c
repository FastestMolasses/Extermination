// NEARMISS func_001B8FC0  (vram 0x001B8FC0, 0x528 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.11% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 13 residual instructions of ~340, in three clusters. (1) DISPATCH, 2 instrs, NOT a compiler wall but an EXPECTED-OBJECT bug: this function has TWO jump tables. mwcc emits each table into its OWN same-named `.rodata` section, each at offset 0, and in REVERSE order of appearance in the function (@3...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: camera-move sequencer, one step per frame. Two jump tables.
//   arg0 = the player/actor block, arg1 = the camera state block,
//   arg2 = the current move descriptor. D_008101E0 is the global camera object
//   (g): g+0x04 = its mode byte, g+0x10 / g+0x20 = its eye and target vectors,
//   g+0x6E = a clip id, g+0x70 = a keyframe cursor, g+0x74 = the cursor index,
//   g+0x78 = the current key value. D_008105D0..D8 and D_008105E0..E8 are the
//   working eye and target vectors; D_00810360 is the listener/home position.
//   arg1+0x04 = the sequencer phase (0 = enter, 1 = run), arg1+0x10..0x28 hold
//   the per-axis deltas. arg2+0x08 = the move kind (0..10, both tables are
//   11 entries), arg2+0x0C = its duration, arg2+0x10 = its elapsed frames,
//   arg2+0x14/0x18/0x1C = clip parameters, arg2+0x20 / 0x30 = the target eye
//   and target vectors.
//   phase 0 (enter): stage the eye/target pair for the move kind -- kinds 0/3
//   copy both from the descriptor, 1/5 copy the CURRENT working vectors back
//   into arg1, 4/7 aim the target at the home position, 9/10 do the same and
//   raise the target's Y by 6.0, kind 6 instead binds a keyframe track
//   (func_001C6120 on D_0028A490[arg2+0x1C]) into the camera object and starts
//   it. Kinds 3 and 6 return 1 (done immediately); everything else zeroes the
//   elapsed counter, advances the phase, and returns 0.
//   phase 1 (run): move kind 8 is the "settle" kind -- it returns 1 only once
//   both convergence tests (func_0018C6A0 / func_0018C4B0, tolerances 0.4 and
//   0.3) report all three axes done. Otherwise, when the elapsed count reaches
//   the duration the move is finished (return 1); until then tick the counter
//   and apply the per-kind update: kinds 1/5 interpolate the eye and target
//   along the staged deltas by t = elapsed/duration (kind 1 first eases t with
//   a sine curve, (1 + sin(pi*t - pi/2)) / 2), kind 7 re-aims the target at the
//   home position, kind 10 does that and raises Y by 6.0. All non-returning
//   paths publish the working vectors through func_001DD980 and return 0.
//
//   NOTE: the `volatile` on D_008105D0..D_008105E8 is a MATCHING DEVICE only
//   (it reproduces CW's un-reordered, un-CSE'd per-component accumulate); the
//   globals are ordinary floats.

extern void func_00102948(void *dst, void *src);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_001DD980(void *a, void *b);
extern int func_0018C6A0(void *p, void *q, float t);
extern int func_0018C4B0(void *p, float a, float b);
extern float func_0011E2A8(float x);
extern float *func_001C6120(int a, int b);
extern void func_0022EC30(void *p);

extern char D_008101E0[];
extern int D_0028A490[];
extern float D_00810360;
extern volatile float D_008105D0;
extern volatile float D_008105D4;
extern volatile float D_008105D8;
extern volatile float D_008105E0;
extern volatile float D_008105E4;
extern volatile float D_008105E8;

int func_001B8FC0(char *arg0, char *arg1, char *arg2) {
    char *g;
    int r;
    int m;
    float t;

    g = D_008101E0;
    switch (*(unsigned char *)(arg1 + 4)) {
    case 0:
        switch (*(int *)(arg2 + 8)) {
        case 0:
            func_00102948((void *)&D_008105D0, arg2 + 0x20);
            func_00102948((void *)&D_008105E0, arg2 + 0x30);
            func_00102948(g + 0x10, arg2 + 0x20);
            func_00102948(g + 0x20, arg2 + 0x30);
            break;
        case 1:
        case 5:
            func_00102948(arg1 + 0x10, (void *)&D_008105D0);
            func_00102948(arg1 + 0x20, (void *)&D_008105E0);
            break;
        case 3:
            func_00102948((void *)&D_008105D0, arg2 + 0x20);
            func_00102948((void *)&D_008105E0, arg2 + 0x30);
            func_00102948(g + 0x10, arg2 + 0x20);
            func_00102948(g + 0x20, arg2 + 0x30);
            return 1;
        case 4:
        case 7:
            func_00102948((void *)&D_008105D0, arg2 + 0x20);
            func_00102948((void *)&D_008105E0, &D_00810360);
            func_00102948(g + 0x10, arg2 + 0x20);
            func_00102948(g + 0x20, &D_00810360);
            break;
        case 9:
        case 10:
            func_00102948((void *)&D_008105D0, arg2 + 0x20);
            func_00102948((void *)&D_008105E0, &D_00810360);
            D_008105E4 += 6.0f;
            func_00102948(g + 0x10, (void *)&D_008105D0);
            func_00102948(g + 0x20, (void *)&D_008105E0);
            break;
        case 6:
            *(float **)(g + 0x70) = func_001C6120(D_0028A490[*(int *)(arg2 + 0x1C)], *(int *)(arg2 + 0x14));
            *(int *)(g + 0x74) = 0;
            *(float *)(g + 0x78) = **(float **)(g + 0x70);
            *(char *)(g + 4) = 3;
            *(short *)(g + 0x6E) = *(short *)(arg2 + 0x18);
            func_0022EC30(g);
            return 1;
        }
        *(int *)(arg2 + 0x10) = 0;
        *(unsigned char *)(arg1 + 4) = *(unsigned char *)(arg1 + 4) + 1;
        break;
    case 1:
        if (*(int *)(arg2 + 8) == 8) {
            r = func_0018C6A0(arg0 + 0xB0, (void *)&D_008105E0, 0.4f);
            r = r | func_0018C4B0((void *)&D_008105E0, *(float *)(arg0 + 0xB4), 0.3f);
            if (r == 7) {
                func_001DD980((void *)&D_008105D0, (void *)&D_008105E0);
                return 1;
            }
            break;
        }
        if (!(*(float *)(arg2 + 0x10) < *(float *)(arg2 + 0xC))) {
            func_001DD980((void *)&D_008105D0, (void *)&D_008105E0);
            return 1;
        }
        *(float *)(arg2 + 0x10) += 1.0f;
        m = *(int *)(arg2 + 8);
        switch (m) {
        case 7:
            func_00102948((void *)&D_008105E0, &D_00810360);
            func_00102948(g + 0x20, &D_00810360);
            break;
        case 10:
            func_00102948((void *)&D_008105E0, &D_00810360);
            D_008105E4 += 6.0f;
            func_00102948(g + 0x20, (void *)&D_008105E0);
            break;
        case 1:
        case 5:
            t = *(float *)(arg2 + 0x10) / *(float *)(arg2 + 0xC);
            if (m == 1) {
                t = (1.0f + func_0011E2A8(3.1415927f * t - 1.5707964f)) / 2.0f;
            }
            func_001028D0((void *)&D_008105D0, arg2 + 0x20, arg1 + 0x10);
            func_001028D0((void *)&D_008105E0, arg2 + 0x30, arg1 + 0x20);
            D_008105D0 = *(float *)(arg1 + 0x10) + D_008105D0 * t;
            D_008105D4 = *(float *)(arg1 + 0x14) + D_008105D4 * t;
            D_008105D8 = *(float *)(arg1 + 0x18) + D_008105D8 * t;
            D_008105E0 = *(float *)(arg1 + 0x20) + D_008105E0 * t;
            D_008105E4 = *(float *)(arg1 + 0x24) + D_008105E4 * t;
            D_008105E8 = *(float *)(arg1 + 0x28) + D_008105E8 * t;
            func_00102948(g + 0x10, (void *)&D_008105D0);
            func_00102948(g + 0x20, (void *)&D_008105E0);
            break;
        case 0:
        case 2:
        case 3:
        case 4:
        case 6:
        case 8:
        case 9:
            break;
        }
        break;
    }
    func_001DD980((void *)&D_008105D0, (void *)&D_008105E0);
    return 0;
}

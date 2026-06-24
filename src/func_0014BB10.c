// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Per-entity timed state update. arg1 is the entity state block, arg0 its
// owner/context.
//   If state +0x38 is active (!=0): if the entity's float at +0x68 is <= 50.0
//     just clear the +0x3C frame counter; otherwise bump +0x3C and once it
//     reaches 0x78 (120) frames clear both +0x38 (state) and +0x3C.
//   If +0x38 is idle (==0): if owner flag (arg0+0xA & 1) is set, arm state
//     +0x38 = 0xF0 and reset +0x3C; else evaluate func_001B3F10(owner, angle,
//     50.0) where angle = func_001B1470(func_0011E620(ctx+0xB0, ctx+0xB8) -
//     pi/2) with ctx = arg0+0x13C. On a true result, bump +0x3C and once it
//     reaches the per-difficulty threshold table D_00275400[D_0081050C & 3],
//     arm +0x38 = 0xF0 and reset +0x3C; otherwise reset +0x3C.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the
// 991202 build reaches only 95.5% (idiom-13 -- it routes the frame-counter test
// through `sltiu at; bnez at` and merges the two-exit epilogue into one). The
// 2.3.3 build keeps CW's named-GPR compare + duplicated `lq ra` epilogue, so
// this readable C is byte-identical. Verified objdiff 100.0% vs
// build/expected/func_0014BB10.o.
extern float func_0011E620(float, float);
extern float func_001B1470(float);
extern int func_001B3F10(char *, float, float);
extern unsigned char D_00275400;
extern unsigned char D_0081050C[8];

void func_0014BB10(char *arg0, char *arg1) {
    int v1;
    char *v0;

    if (*(int *)(arg1 + 0x38) != 0) {
        if (*(float *)(arg1 + 0x68) <= 50.0f) {
            *(int *)(arg1 + 0x3C) = 0;
        } else {
            v1 = *(int *)(arg1 + 0x3C) + 1;
            *(int *)(arg1 + 0x3C) = v1;
            if ((unsigned int)v1 >= 0x78U) {
                *(int *)(arg1 + 0x38) = 0;
                *(int *)(arg1 + 0x3C) = 0;
            }
        }
        return;
    } else {
        if (*(unsigned char *)(arg0 + 0xA) & 1) {
            *(int *)(arg1 + 0x38) = 0xF0;
            *(int *)(arg1 + 0x3C) = 0;
        } else {
            v0 = *(char **)(arg0 + 0x13C);
            if (func_001B3F10(arg0, func_001B1470(func_0011E620(*(float *)(v0 + 0xB0), *(float *)(v0 + 0xB8)) - 1.5707964f), 50.0f) != 0) {
                v1 = *(int *)(arg1 + 0x3C) + 1;
                *(int *)(arg1 + 0x3C) = v1;
                if ((unsigned int)v1 >= (unsigned char)*(&D_00275400 + (D_0081050C[0] & 3))) {
                    *(int *)(arg1 + 0x38) = 0xF0;
                    *(int *)(arg1 + 0x3C) = 0;
                }
            } else {
                *(int *)(arg1 + 0x3C) = 0;
            }
        }
    }
}
